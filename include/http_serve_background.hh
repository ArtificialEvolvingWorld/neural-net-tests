#pragma once

#include <memory>
#include <thread>

#include "eweb.hh"

class http_serve_background {
public:
  http_serve_background(eweb::server_opts config)
    : server(config) {

    thread = std::thread([this]() {
        server.run();
      });
  }

  ~http_serve_background() {
    server.close();
    thread.join();
  }

private:
  eweb::server server;
  std::thread thread;
};
