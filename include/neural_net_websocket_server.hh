#pragma once

#include <thread>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class neural_net_websocket_server {
public:
  typedef websocketpp::server<websocketpp::config::asio> server_t;

  neural_net_websocket_server(int port);
  ~neural_net_websocket_server();

private:
  void run() { server.run(); }

  void on_first_message(websocketpp::connection_hdl handle,
                        server_t::message_ptr msg);
  void on_regular_message(websocketpp::connection_hdl handle,
                          server_t::message_ptr msg);

  std::thread thread;
  server_t server;
  std::vector<websocketpp::connection_hdl> live_connections;
};
