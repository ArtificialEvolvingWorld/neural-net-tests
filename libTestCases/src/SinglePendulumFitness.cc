#include "SinglePendulumFitness.hh"

#include <cmath>

#include "RungeKutta.hh"

double normalize_theta(double theta) {
  return std::remainder(theta, 2*M_PI);
}

std::function<double(NeuralNet&)> single_pendulum_fitness_func(
  SinglePendulum initial_state, double deltaT,
  int num_steps, double max_angle_balanced) {

  return [=](NeuralNet& net) {
    RungeKutta<SinglePendulum> rk(initial_state, deltaT);
    int num_success = 0;
    for(int i=0; i<num_steps; i++) {
      auto& current = rk.GetCurrent();

      double val = net.evaluate({normalize_theta(current.theta)})[0];
      current.Fext = 5*(2*val-1);

      rk.Step();

      if(std::abs(normalize_theta(current.theta)) < max_angle_balanced) {
        num_success++;
      }
    }
    return num_success;
  };
}
