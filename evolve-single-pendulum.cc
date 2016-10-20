#include <cmath>

#include "Genome.hh"
#include "Population.hh"

#include "SinglePendulum.hh"
#include "RungeKutta.hh"

double normalize_theta(double theta) {
  return std::remainder(theta, 2*M_PI);
}

int main() {
  SinglePendulum initial;
  initial.theta = 1 * (3.1415926/180);

  double deltaT = 0.1;
  int num_steps = 10000;
  double threshold = 10 * (3.1415926/180);

  auto seed = Genome()
    .AddNode(NodeType::Bias)
    .AddNode(NodeType::Input)
    .AddNode(NodeType::Output)
    .AddConnection(0,2,true,1.)
    .AddConnection(1,2,true,1.);

  // auto seed = Genome()
  //   .AddNode(NodeType::Bias)
  //   .AddNode(NodeType::Input)
  //   .AddNode(NodeType::Input)
  //   .AddNode(NodeType::Output)
  //   .AddConnection(0,3,true,1.)
  //   .AddConnection(1,3,true,1.)
  //   .AddConnection(2,3,true,1.);

  auto prob = std::make_shared<Probabilities>();
  prob->genetic_c1 = 4.0;
  prob->genetic_c2 = 2.0;
  prob->culling_ratio = 0.2;

  auto rand = std::make_shared<Uniform>(0,1);

  Population pop(seed, rand, prob);


  auto fitness = [&](auto&& net) {
    RungeKutta<SinglePendulum> rk(initial, deltaT);

    int num_success = 0;
    for(int i=0; i<num_steps; i++) {
      auto& current = rk.GetCurrent();
      //double val = net.evaluate({normalize_theta(current.theta), current.omega})[0];
      double val = net.evaluate({normalize_theta(current.theta)})[0];
      current.Fext = 5*(2*val-1);

      rk.Step();

      if(std::abs(normalize_theta(current.theta)) < threshold) {
        num_success++;
      }
    }
    return num_success;
  };

  for(int generation=0; generation<100; generation++) {
    auto next_gen = pop.Reproduce(fitness);

    auto best = pop.BestNet();
    auto mean_stddev = pop.MeanStdDev();
    std::cout << " ----------- Gen " << generation << " ----------------" << std::endl;
    std::cout << pop.NumSpecies() << " species total" << std::endl;
    std::cout << "Best (nodes, conn) = (" << best->num_nodes() << ", " << best->num_connections()
              << ")" << std::endl;
    std::cout << "Best Fitness: " << fitness(*best) << std::endl;
    std::cout << "Ave Fitness: " << mean_stddev.first << " +- " << mean_stddev.second << std::endl;

    pop = std::move(next_gen);
  }
}
