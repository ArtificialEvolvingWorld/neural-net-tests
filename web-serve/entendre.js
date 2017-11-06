var connection = null;
var message_sent_box = document.getElementById("text-message-sent");
var message_received_box = document.getElementById("text-message-received");

function connect() {
    connection = new WebSocket("ws://" + location.hostname + ":11223");
    connection.onmessage = on_receive_message;
}

function send_message() {
    var obj = {"time": new Date().getTime() / 1000,
               "message": "asdf",
              };
    var message = JSON.stringify(obj);
    connection.send(message);

    message_sent_box.innerText = message + "\n" + message_sent_box.innerText;
}

function on_receive_message(event) {
    var message = event.data;
    message_received_box.innerText = message + "\n" + message_received_box.innerText;
}
