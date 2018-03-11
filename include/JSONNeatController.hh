#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Population.hh"
#include "PopulationBackgroundThread.hh"

#include "nlohmann/json.hpp"
using nlohmann::json;

struct ServerResponse {
  std::string response;
  std::string broadcast;
};

class JSONNeatController {
public:
  JSONNeatController();

  /// Make a request from the webpage to the controller.
  /**
     Return value contains the response to be sent to the client that
     sent the command, and a response to be broadcast to all connected
     clients.
   */
  ServerResponse request(const std::string& command);

  /// Called periodically, to see if updates are needed.
  /**
     The return value is a JSON-encoded string to be broadcast to all
     connected clients.  If empty, no broadcast should be sent.
   */
  std::string update_check();

private:
  ServerResponse request_maythrow(const std::string& command);

  void get_overview(json& j) const;
  void get_generation_count(json& j) const;
  json get_generation_info(unsigned int generation_num) const;
  json get_network_info(unsigned int generation_num,
                        unsigned int species_num,
                        unsigned int organism_num) const;


  void reset();
  void queue_n_generations(int num_gens);

  Population make_population();
  std::function<double(NeuralNet&)> get_fitness_func();

  unsigned int num_generations_queued;
  std::vector<std::unique_ptr<Population> > all_generations;
  std::unique_ptr<PopulationBackgroundThread> bg_thread;
};
