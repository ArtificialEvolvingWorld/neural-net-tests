#include "neural_net_websocket_server.hh"

#include "json.hpp"
using nlohmann::json;

neural_net_websocket_server::neural_net_websocket_server(int port) {
  server.set_message_handler(
    [this](websocketpp::connection_hdl handle, server_t::message_ptr msg) {
      on_first_message(handle, msg);
    });

  // server.set_access_channels(websocketpp::log::alevel::all);
  // server.set_error_channels(websocketpp::log::elevel::all);

  server.init_asio();
  server.listen(port);
  server.start_accept();

  thread = std::thread([this](){ run(); });
}

neural_net_websocket_server::~neural_net_websocket_server() {
  server.stop_listening();
  for(auto& handle : live_connections) {
    auto conn = server.get_con_from_hdl(handle);
    conn->close(websocketpp::close::status::going_away, "");
  }
  thread.join();
}

void neural_net_websocket_server::on_first_message(websocketpp::connection_hdl handle,
                                                   server_t::message_ptr msg) {
  std::cout << "First connection received" << std::endl;
  // In future, password validation will be done here.
  auto payload = msg->get_payload();
  if(true) {
    auto conn = server.get_con_from_hdl(handle);
    conn->set_message_handler(
      [this](websocketpp::connection_hdl handle, server_t::message_ptr msg) {
        on_regular_message(handle, msg);
      });

    json j;
    j["ack"] = true;
    server.send(handle, j.dump(), websocketpp::frame::opcode::text);

    live_connections.push_back(handle);
  }
}

void neural_net_websocket_server::on_regular_message(websocketpp::connection_hdl handle,
                                                     server_t::message_ptr msg) {
  std::cout << "Regular message received" << std::endl;
  auto j = json::parse(msg->get_payload());
  j["ack"] = true;
  server.send(handle, j.dump(), websocketpp::frame::opcode::text);
}
