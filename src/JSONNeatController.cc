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

  if(j.count("overview_requested") &&
     j["overview_requested"]) {
    get_overview(output_reply);
  }


  if(j.count("advance_n_generations") &&
     j["advance_n_generations"] > 0) {
    int num_gens = j["advance_n_generations"];
    queue_n_generations(num_gens);
    get_generation_count(output_broadcast);
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

      get_generation_count(j);
      j["generation_information"].push_back(
        get_generation_info(all_generations.size()-1)
      );
    }
  }

  return j.empty() ? "" : j.dump();
}

void JSONNeatController::get_overview(json& j) const {
  get_generation_count(j);
  for(unsigned int i=0; i<all_generations.size(); i++) {
    j["generation_information"].push_back( get_generation_info(i) );
  }
}

void JSONNeatController::get_generation_count(json& j) const {
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

json JSONNeatController::get_generation_info(unsigned int generation_num) const {
  json j;
  j["index"] = generation_num;

  if(generation_num >= all_generations.size()) {
    return j;
  }

  Population* gen = all_generations[generation_num].get();

  j["species"] = json::array();
  for(auto& species : gen->GetSpecies()) {
    json species_info;

    species_info["id"] = species.id;
    species_info["age"] = species.age;
    species_info["organisms"] = json::array();
    unsigned int org_index = 0;
    for(auto& org : species.organisms) {
      json organism_info;

      organism_info["index"] = org_index;
      organism_info["fitness"] = org.fitness;

      org_index++;
      species_info["organisms"].push_back(organism_info);
    }

    j["species"].push_back(species_info);
  }

  return j;
}
