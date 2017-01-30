#include "XorFitness.hh"

#include <cmath>

double raw_xor_fitness(NeuralNet& net) {
  if(net.num_connections() == 0) {
    return 0.0;
  }

  double error = 0;
  for(int x=0; x<=1; x++) {
    for(int y=0; y<=1; y++) {
      double correct_answer = x^y;

      double val = net.evaluate({double(x),double(y)})[0];
      //error += std::abs(val - correct_answer);
      error += std::pow(val - correct_answer, 2);
    }
  }

  return std::pow(4.0 - error, 2);
  //return 4 - error;
}

std::function<double(NeuralNet&)> xor_fitness_func() {
  return raw_xor_fitness;
}
