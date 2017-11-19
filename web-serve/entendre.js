var connection = null;
var message_sent_box = document.getElementById("text-message-sent");
var message_received_box = document.getElementById("text-message-received");

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
        var item = ('<option value="' + gen["index"] + '">' +
                    'Generation ' + gen["index"] + '</option>');
        new_items += item;
        all_generation_info[gen["index"]] = gen;
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
    var num_species = gen_info["species_sizes"].length;
    for(var spec_num=0; spec_num<num_species; spec_num += 1) {
        var item = ('<option value="' + spec_num + '">' +
                       'Species ' + spec_num + '</option>');
        listing += item;
    }
    species_box.innerHTML = listing;
    species_box.selectedIndex = 0;
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

    var org_box = document.getElementById('select-organism');

    var listing = "";
    var num_organisms = gen_info["species_sizes"][species_index]
    for(var org_num=0; org_num<num_organisms; org_num += 1) {
        var item = ('<option value="' + org_num + '">' +
                    'Organism ' + org_num + '</option>');
        listing += item;
    }
    org_box.innerHTML = listing;
    org_box.selectedIndex = 0;
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
