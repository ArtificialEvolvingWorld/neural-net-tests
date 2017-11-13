#include "neural_net_websocket_server.hh"

#include <algorithm>

#include "json.hpp"
using nlohmann::json;

#include "XorFitness.hh"

neural_net_websocket_server::neural_net_websocket_server(int port)
  : stop_periodic_check(false), num_queued(0) {
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
    auto conn = server.get_con_from_hdl(handle, ec);
    if(conn) {
      conn->close(websocketpp::close::status::going_away, "");
    }
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

      if(bg_thread) {
        while(true) {
          auto next_gen = bg_thread->get_next_generation();
          if(!next_gen) {
            break;
          }

          all_generations.push_back(std::move(next_gen));
          num_queued--;

          json j;
          j["info"] = "generation_finished";
          j["num_queued"] = num_queued;
          j["num_generations"] = all_generations.size();
          broadcast_all(j.dump());
        }
      }

      do_periodic_check();
    });
}

void neural_net_websocket_server::on_regular_message(websocketpp::connection_hdl handle,
                                                     server_t::message_ptr msg) {
  std::cout << "----------------------" << std::endl;
  std::cout << "Regular message received" << std::endl;
  auto j = json::parse(msg->get_payload());

  if(j["action"] == "get_overview") {
    std::cout << "Sending full program overview" << std::endl;
    server.send(handle, get_overview(), websocketpp::frame::opcode::text);
  } else if (j["action"] == "advance_n_generations") {
    int num_gens = j["num_generations"];
    std::cout << "Advancing " << num_gens << " generations" << std::endl;
    if(!bg_thread) {
      bg_thread = std::make_unique<PopulationBackgroundThread>(make_population());
    }
    num_queued += num_gens;
    bg_thread->perform_reproduction(xor_fitness_func(), num_gens);
  }
}


void neural_net_websocket_server::broadcast_all(std::string message) {
  live_connections.erase(
    std::remove_if(live_connections.begin(), live_connections.end(),
                   [&](auto& conn) {
                     websocketpp::lib::error_code ec;
                     this->server.send(conn, message, websocketpp::frame::opcode::text, ec);
                     return ec;
                   }), live_connections.end());
}

std::string neural_net_websocket_server::get_overview() const {
  json j;

  j["response"] = "overview";
  j["num_queued"] = num_queued;
  j["num_generations"] = all_generations.size();

  return j.dump();
}

Population neural_net_websocket_server::make_population() {
  auto seed = Genome::ConnectedSeed(2, 1);
  return Population(seed,
                    std::make_shared<RNG_MersenneTwister>(),
                    std::make_shared<Probabilities>());
}
