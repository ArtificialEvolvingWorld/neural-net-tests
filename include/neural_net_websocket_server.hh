#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "Population.hh"
#include "PopulationBackgroundThread.hh"

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

  void broadcast_all(std::string message);
  void do_periodic_check();

  std::string get_overview() const;

  std::thread thread;
  server_t server;
  std::vector<websocketpp::connection_hdl> live_connections;
  std::atomic_bool stop_periodic_check;

  // Maybe everything here to bottom should be refactored into
  // separate class?

  unsigned int num_queued = 0;

  Population make_population();
  std::function<double(NeuralNet&)> get_fitness_func();

  std::vector<std::unique_ptr<Population> > all_generations;
  std::unique_ptr<PopulationBackgroundThread> bg_thread;
};
