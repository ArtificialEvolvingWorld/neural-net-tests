#include "neural_net_websocket_server.hh"

#include <algorithm>
#include <iostream>

#include "json.hpp"
using nlohmann::json;

neural_net_websocket_server::neural_net_websocket_server(int port)
  : stop_periodic_check(false) {
  server.set_message_handler(
    [this](websocketpp::connection_hdl handle, server_t::message_ptr msg) {
      on_first_message(handle, msg);
    });

  server.clear_access_channels(websocketpp::log::alevel::all);
  server.clear_error_channels(websocketpp::log::elevel::all);

  server.init_asio();
  server.listen(port);
  server.start_accept();
  do_periodic_check();

  thread = std::thread([this](){ run(); });
}

neural_net_websocket_server::~neural_net_websocket_server() {
  server.stop_listening();
  for(auto& handle : live_connections) {
    websocketpp::lib::error_code ec;
    server.close(handle, websocketpp::close::status::going_away, "", ec);
  }
  stop_periodic_check = true;
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
    j["authenticated"] = true;
    server.send(handle, j.dump(), websocketpp::frame::opcode::text);

    live_connections.push_back(handle);
  }
}

void neural_net_websocket_server::do_periodic_check() {
  server.set_timer(
    100,
    [this](const websocketpp::lib::error_code& ec) {
      if(ec || stop_periodic_check) {
        return;
      }

      std::string broadcast = controller.update_check();
      if(broadcast.size()) {
        broadcast_all(broadcast);
      }

      do_periodic_check();
    });
}

void neural_net_websocket_server::on_regular_message(websocketpp::connection_hdl handle,
                                                     server_t::message_ptr msg) {
  auto response = controller.request(msg->get_payload());

  if(response.response.size()) {
    server.send(handle, response.response, websocketpp::frame::opcode::text);
  }
  if(response.broadcast.size()) {
    broadcast_all(response.broadcast);
  }
}


void neural_net_websocket_server::broadcast_all(const std::string& message) {
  live_connections.erase(
    std::remove_if(live_connections.begin(), live_connections.end(),
                   [&](auto& conn) {
                     websocketpp::lib::error_code ec;
                     this->server.send(conn, message, websocketpp::frame::opcode::text, ec);
                     return ec;
                   }), live_connections.end());
}
