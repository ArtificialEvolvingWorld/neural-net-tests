var connection = null;
var message_sent_box = document.getElementById("text-message-sent");
var message_received_box = document.getElementById("text-message-received");
var total_bytes_sent = 0;
var total_bytes_received = 0;

var all_generation_info = {};
var active_network = {generation_num: -1,
                      species_num: -1,
                      organism_num: -1};

function connect() {
    connection = new WebSocket("ws://" + location.hostname + ":11223");
    connection.onopen = on_websocket_open;
    connection.onmessage = on_receive_message;
    connection.onerror = on_websocket_close;
    connection.onclose = on_websocket_close;

    display_websocket_state('connecting');
}

function reconnect() {
    alert('Not implemented yet');
}

function send_message(obj) {
    var message = JSON.stringify(obj);
    connection.send(message);

    total_bytes_sent += message.length;
    document.getElementById('total-bytes-sent').innerHTML = format_bytes(total_bytes_sent);

    message_sent_box.innerHTML = message + "\n" + message_sent_box.innerHTML;
}

function display_websocket_state(state) {
    var status_span = document.getElementById("status-span");
    if(state == 'connecting') {
        status_span.className = 'status-connecting';
        status_span.innerHTML = 'Connecting';
    } else if (state == 'connected') {
        status_span.className = 'status-connected';
        status_span.innerHTML = 'Connected';
    } else if (state == 'disconnected') {
        status_span.className = 'status-disconnected';
        status_span.innerHTML = 'Disconnected';
    }
}

function on_websocket_open() {
    display_websocket_state('connected');
    send_message({"username": "username",
                  "password": "password",
                 });
}

function on_websocket_close() {
    display_websocket_state('disconnected');
}

function on_receive_message(event) {
    var message = event.data;
    message_received_box.innerHTML = message + "\n" + message_received_box.innerHTML;

    total_bytes_received += message.length;
    document.getElementById('total-bytes-received').innerHTML = format_bytes(total_bytes_received);

    var parsed = JSON.parse(message);
    if(parsed.hasOwnProperty('authenticated')) {
        send_message({"overview_requested": true});

    }

    if(parsed.hasOwnProperty('num_queued') &&
       parsed.hasOwnProperty('num_generations')) {
        document.getElementById('label-current-gens').innerHTML = parsed['num_generations'];
        document.getElementById('label-total-gens').innerHTML = parsed['num_queued'] + parsed['num_generations'];
    }

    if(parsed.hasOwnProperty('generation_information')) {
        display_new_generations(parsed['generation_information']);
    }

    if(parsed.hasOwnProperty('network_details')) {
        display_network(parsed['network_details']);
    }
}

function format_bytes(bytes) {
    if(bytes < Math.pow(1024,1)) {
        return bytes + ' bytes';
    } else if (bytes < Math.pow(1024,2)) {
        return (bytes/1024).toFixed(1) + ' kB';
    } else if (bytes < Math.pow(1024,3)) {
        return (bytes/1024/1024).toFixed(2) + ' MB';
    } else {
        return (bytes/1024/1024/1024).toFixed(3) + ' GB';
    }
}

function queue_generations(num_gens) {
    send_message({"advance_n_generations": num_gens});
}

function queue_n_generations() {
    var input_spinner = document.getElementById('button-n-gen').querySelector('input');
    var num_gens = parseInt(input_spinner.value);
    queue_generations(num_gens);
}

function display_new_generations(generations) {
    var generation_box = document.getElementById('select-generation');

    var new_items = "";
    for(var i = 0; i<generations.length; i += 1) {
        var gen = generations[i];
        var item = ('<option value="' + gen.index + '">' +
                    'Generation ' + gen.index + '</option>');
        new_items += item;
        all_generation_info[gen.index] = gen;
    }
    generation_box.innerHTML += new_items;
}

function get_selected_generation() {
    var generation_box = document.getElementById('select-generation');
    var gen_index = generation_box.options[generation_box.selectedIndex].value;
    return all_generation_info[gen_index];
}

function get_selected_species() {
    var gen_info = get_selected_generation();

    var species_box = document.getElementById('select-species');
    var species_index = species_box.options[species_box.selectedIndex].value;
    return gen_info.species[species_index];
}

function on_generation_select() {
    display_selected_generation();
    display_selected_species();
    var gen_info = get_selected_generation();
    plot_generation_fitness(gen_info);
}

function display_selected_generation() {
    var gen_info = get_selected_generation();

    var species_box = document.getElementById('select-species');

    species_box.innerHTML = "";

    var listing = "";
    var num_species = gen_info.species.length;
    for(var spec_num=0; spec_num<num_species; spec_num += 1) {
        var item = ('<option value="' + spec_num + '">' +
                       'Species ' + spec_num + '</option>');
        listing += item;
    }
    species_box.innerHTML = listing;
    species_box.selectedIndex = 0;
}

function plot_generation_fitness(gen_info) {
    var fitnesses = []
    for(var spec_num=0; spec_num<gen_info.species.length; spec_num += 1) {
        var species = gen_info.species[spec_num];
        for(var org_num=0; org_num<species.organisms.length; org_num += 1) {
            fitnesses.push(species.organisms[org_num].fitness);
        }
    }

    var title = 'Gen. ' + gen_info.index + ' Fitness';

    var plot_data = { x: fitnesses,
                      type: 'histogram',
                      name: title
                    };

    var layout = { margin: { t: 40 },
                   title: title,
                   yaxis: {
                       title: 'Counts',
                       //range: [display_min, display_max]
                   },
                   showlegend: false };

    var options = { showLink: false,
                    staticPlot: false,
                    displayModeBar: false };

    Plotly.newPlot('population-stat-box', [plot_data], layout, options);
}

function on_generation_keydown(event) {
    if(event.key === 'ArrowRight') {
        document.getElementById('select-species').focus();
    }
}
document.getElementById('select-generation').addEventListener('keydown',on_generation_keydown);

function on_species_select() {
    display_selected_species();

    var gen_info = get_selected_generation();
    var spec_info = get_selected_species();
    plot_species_fitness(gen_info, spec_info);
}

function display_selected_species() {
    var gen_info = get_selected_generation();
    var spec_info = get_selected_species();

    var org_box = document.getElementById('select-organism');

    var listing = "";
    var num_organisms = spec_info.organisms.length;
    for(var org_num=0; org_num<num_organisms; org_num += 1) {
        var item = ('<option value="' + org_num + '">' +
                    'Organism ' + org_num + '</option>');
        listing += item;
    }
    org_box.innerHTML = listing;
    org_box.selectedIndex = 0;
}

function plot_species_fitness(gen_info, spec_info) {
    var fitnesses = []
    for(var org_num=0; org_num<spec_info.organisms.length; org_num += 1) {
        fitnesses.push(spec_info.organisms[org_num].fitness);
    }

    var title = ('Gen. ' + gen_info.index + ', ' +
                 'Spec. ' + spec_info.id + ' Fitness');

    var plot_data = { x: fitnesses,
                      type: 'histogram',
                      name: title
                    };

    var layout = { margin: { t: 40 },
                   title: title,
                   yaxis: {
                       title: 'Counts',
                       //range: [display_min, display_max]
                   },
                   showlegend: false };

    var options = { showLink: false,
                    staticPlot: false,
                    displayModeBar: false };

    Plotly.newPlot('population-stat-box', [plot_data], layout, options);
}

function on_species_keydown(event) {
    if(event.key === 'ArrowLeft') {
        document.getElementById('select-generation').focus();
    } else if(event.key === 'ArrowRight') {
        document.getElementById('select-organism').focus();
    }
}
document.getElementById('select-species').addEventListener('keydown',on_species_keydown);

function on_organism_select() {
    var gen_info = get_selected_generation();
    var spec_info = get_selected_species();

    var org_box = document.getElementById('select-organism');
    var org_index = org_box.options[org_box.selectedIndex].value;
    org_index = parseInt(org_index);

    active_network.generation_num = gen_info.index;
    active_network.species_num = spec_info.index;
    active_network.organism_num = org_index;
    send_message({network_details_requested: active_network});
}

function on_organism_keydown(event) {
    if(event.key === 'ArrowLeft') {
        document.getElementById('select-species').focus();
    }
}
document.getElementById('select-organism').addEventListener('keydown',on_organism_keydown);

function display_network(details) {
    if(details.generation_num != active_network.generation_num ||
       details.species_num != active_network.species_num ||
       details.organism_num != active_network.organism_num) {
        return;
    }

    type_color_table = {
        Input: '#6174DD',
        Bias: '#1FC291',
        Output: '#FF5748',
    };

    func_color_table = {
        Identity: '#68C6D3',
        Sigmoid: '#EE8A2A',
        Tanh: '#B17516',
        Relu: '#B1B0AA',
        Gaussian: '#2CB11F',
        Sin: '#F6DE39',
        Cos: '#C5B12C',
        Abs: '#E685E7',
        Square: '#F050E6',
    };

    var nodes = [];
    var i_input = 0;
    var i_output = 0;
    var i_hidden = 0;
    for(var i=0; i<details.nodes.length; i += 1) {
        var type = details.nodes[i].type;
        var func = details.nodes[i].func;
        var label = 'Unknown';
        if(type == 'Input') {
            label = 'Input ' + i_input;
            i_input += 1;
        } else if(type == 'Output') {
            label = 'Output ' + i_output;
            i_output += 1;
        } else if(type == 'Bias') {
            label = 'Bias';
        } else if(type == 'Hidden') {
            label = 'Hidden ' + i_hidden;
            i_hidden += 1;
        }

        var color = (type_color_table.hasOwnProperty(type) ? type_color_table[type] :
                     func_color_table.hasOwnProperty(func) ? func_color_table[func] :
                     'Blue');
        console.log(color);

        nodes.push({id: i,
                    label: label,
                    color: color,
                   });
    }
    nodes = new vis.DataSet(nodes);

    var edges = [];
    for(var i=0; i<details.connections.length; i += 1) {
        var conn = details.connections[i];
        edges.push({from: conn.origin,
                    to: conn.dest,
                    arrows: 'to',
                    value: Math.abs(conn.weight),
                    dashes: conn.type == 'Recurrent',
                    color: {color: '#6174DD'},
                   });
    }
    edges = new vis.DataSet(edges);

    var container = document.getElementById("network-display-box");
    var data = {nodes: nodes,
                edges: edges};
    var options = {height: "400",
                   edges: {scaling: {min: 1,
                                     max: 6}},
                  };
    new vis.Network(container, data, options);
}

display_websocket_state('disconnected');
window.setTimeout(connect, 0);
