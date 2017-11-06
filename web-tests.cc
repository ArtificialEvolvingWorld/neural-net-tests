#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "http_serve_background.hh"
#include "neural_net_websocket_server.hh"

volatile sig_atomic_t keep_running = 1;

void signal_handler(int /*sig*/) {
  keep_running = 0;
}

int main() {
  try {
    http_serve_background http_server(
      eweb::server_opts()
      .http_port(8080)
      .http_callback(eweb::file_server("web-serve"))
      .close_on_sigint(false));

    neural_net_websocket_server websocket_server(11223);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while(keep_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  } catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
