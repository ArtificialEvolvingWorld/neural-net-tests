var connection = null;
var message_sent_box = document.getElementById("text-message-sent");
var message_received_box = document.getElementById("text-message-received");
var total_bytes_sent = 0;
var total_bytes_received = 0;

var all_generation_info = {};

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

function on_generation_select() {
    var generation_box = document.getElementById('select-generation');
    var gen_index = generation_box.options[generation_box.selectedIndex].value;
    var gen_info = all_generation_info[gen_index];

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

    plot_generation_fitness(gen_info);
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
    var generation_box = document.getElementById('select-generation');
    var gen_index = generation_box.options[generation_box.selectedIndex].value;
    var gen_info = all_generation_info[gen_index];

    var species_box = document.getElementById('select-species');
    var species_index = species_box.options[species_box.selectedIndex].value;
    var spec_info = gen_info.species[species_index]

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

    plot_species_fitness(gen_info, spec_info);
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

function on_organism_keydown(event) {
    if(event.key === 'ArrowLeft') {
        document.getElementById('select-species').focus();
    }
}
document.getElementById('select-organism').addEventListener('keydown',on_organism_keydown);

display_websocket_state('disconnected');
window.setTimeout(connect, 0);
