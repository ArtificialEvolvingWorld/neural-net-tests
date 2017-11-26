#include "JSONNeatController.hh"

#include <stdexcept>

#include "XorFitness.hh"

JSONNeatController::JSONNeatController()
  : num_generations_queued(0) { }

ServerResponse JSONNeatController::request(const std::string& command) {
  try {
    return request_maythrow(command);
  } catch(std::domain_error& e) {
    std::cout << "Error in parsing: " << e.what() << "\n"
              << command << std::endl;
    return {"", ""};
  }
}

ServerResponse JSONNeatController::request_maythrow(const std::string& command) {
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

  if(j.count("network_details_requested")) {
    auto& req = j["network_details_requested"];
    unsigned int generation_num = req["generation_num"];
    unsigned int species_num = req["species_num"];
    unsigned int organism_num = req["organism_num"];
    output_reply["network_details"] = get_network_info(
      generation_num, species_num, organism_num);
  }

  if(j.count("reset") &&
     j["reset"]) {
    reset();
    output_broadcast["reset"] = true;
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

void JSONNeatController::reset() {
  all_generations.clear();
  bg_thread = nullptr;
  num_generations_queued = 0;
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
  unsigned int species_index = 0;
  for(auto& species : gen->GetSpecies()) {
    json species_info;

    species_info["index"] = species_index++;
    species_info["id"] = species.id;
    species_info["age"] = species.age;
    species_info["organisms"] = json::array();
    unsigned int org_index = 0;
    for(auto& org : species.organisms) {
      json organism_info;

      organism_info["index"] = org_index++;
      organism_info["fitness"] = org.fitness;

      species_info["organisms"].push_back(organism_info);
    }

    j["species"].push_back(species_info);
  }

  return j;
}

json JSONNeatController::get_network_info(unsigned int generation_num,
                                          unsigned int species_num,
                                          unsigned int organism_num) const {
  json j;

  j["generation_num"] = generation_num;
  j["species_num"] = species_num;
  j["organism_num"] = organism_num;

  // Get the population, if present
  if(generation_num >= all_generations.size()) {
    return j;
  }
  Population* pop = all_generations[generation_num].get();

  // Get the species, if present
  auto& all_species = pop->GetSpecies();
  if(species_num >= all_species.size()) {
    return j;
  }
  auto& species = all_species[species_num];

  // Get the organism, if present
  if(organism_num >= species.organisms.size()) {
    return j;
  }
  auto& org = species.organisms[organism_num];
  NeuralNet* net = org.network();

  // Pull out node information
  json all_node_info = json::array();
  for(unsigned int i=0; i<net->num_nodes(); i++) {
    json node_info;
    node_info["type"] = to_string(net->get_node_type(i));
    node_info["func"] = to_string(net->get_activation_func(i));
    all_node_info.push_back(node_info);
  }
  j["nodes"] = all_node_info;

  // Pull out connection information
  json all_conn_info = json::array();
  for(unsigned int i=0; i<net->num_connections(); i++) {
    auto conn = net->get_connection(i);
    json conn_info;
    conn_info["origin"] = conn.origin;
    conn_info["dest"] = conn.dest;
    conn_info["weight"] = conn.weight;
    conn_info["type"] = to_string(conn.type);
    all_conn_info.push_back(conn_info);
  }
  j["connections"] = all_conn_info;

  return j;
}
