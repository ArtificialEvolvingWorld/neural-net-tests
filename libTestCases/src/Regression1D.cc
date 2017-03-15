#include "Regression1D.hh"

#include <cmath>
#include <vector>
#include <iostream>
#include <limits>


std::function<double(NeuralNet&)> regression_1d_fitness_func(std::vector<float> data, std::vector<float> label) {
  return [=](NeuralNet& net) {
    if(net.num_connections() == 0) {
      return 0.0;
    }

    auto n = data.size();

    double error = 0;
    for (auto i=0u; i<data.size(); i++) {
      double y = net.evaluate({data[i]})[0];
      error += std::pow((label[i]-y)/n, 2);
    }


    //double penalty = 1 + 0.6*net.num_connections() + 0.4*net.num_nodes();
    double penalty = (net.num_nodes() > 6) ? 0 : 1;
    //double fitness = 10.-penalty*error/(2*n);
    double fitness = 10.-error/(2*n);
    return std::max(fitness,0.0)*penalty;
  };
}
