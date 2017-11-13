var connection = null;
var message_sent_box = document.getElementById("text-message-sent");
var message_received_box = document.getElementById("text-message-received");

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
        send_message({"action": "get_overview"});

    } else if(parsed.hasOwnProperty('num_queued') &&
              parsed.hasOwnProperty('num_generations')) {
        document.getElementById('label-current-gens').innerHTML = parsed['num_generations'];
        document.getElementById('label-total-gens').innerHTML = parsed['num_queued'] + parsed['num_generations'];
    }
}

function queue_generations(num_gens) {
    send_message({"action": "advance_n_generations",
                  "num_generations": num_gens,
                 });
}

function queue_n_generations() {
    var input_spinner = document.getElementById('button-n-gen').querySelector('input');
    var num_gens = parseInt(input_spinner.value);
    queue_generations(num_gens);
}

display_websocket_state('disconnected');
window.setTimeout(connect, 0);
