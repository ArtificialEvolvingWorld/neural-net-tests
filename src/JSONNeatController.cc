#include "JSONNeatController.hh"

#include "XorFitness.hh"

JSONNeatController::JSONNeatController()
  : num_generations_queued(0) { }


ServerResponse JSONNeatController::request(const std::string& command) {
  std::cout << "----------------------" << std::endl;
  std::cout << "Regular message received" << std::endl;

  auto j = json::parse(command);

  json output_reply;
  json output_broadcast;

  if(j["action"] == "get_overview") {
    std::cout << "Sending full program_overview" << std::endl;
    get_overview(output_reply);
  }
  if(j["action"] == "advance_n_generations") {
    int num_gens = j["num_generations"];
    queue_n_generations(num_gens);
  }

  return { output_reply.empty() ? "" : output_reply.dump(),
      output_broadcast.empty() ? "" : output_broadcast.dump() };
}

std::string JSONNeatController::update_check() {
  json j;

  if(bg_thread) {
    while(true) {
      auto next_gen = bg_thread->get_next_generation();
      if(!next_gen) {
        break;
      }

      all_generations.push_back(std::move(next_gen));
      num_generations_queued--;

      j["info"] = "generation_finished";
      j["num_queued"] = num_generations_queued;
      j["num_generations"] = all_generations.size();
    }
  }

  return j.empty() ? "" : j.dump();
}

void JSONNeatController::get_overview(json& j) const {
  j["num_queued"] = num_generations_queued;
  j["num_generations"] = all_generations.size();
}

Population JSONNeatController::make_population() {
  auto seed = Genome::ConnectedSeed(2, 1);
  return Population(seed,
                    std::make_shared<RNG_MersenneTwister>(),
                    std::make_shared<Probabilities>());
}

void JSONNeatController::queue_n_generations(int num_gens) {
  std::cout << "Queueing " << num_gens << " generations" << std::endl;
  if(!bg_thread) {
    bg_thread = std::make_unique<PopulationBackgroundThread>(make_population());
  }
  num_generations_queued += num_gens;
  bg_thread->perform_reproduction(xor_fitness_func(), num_gens);
}
