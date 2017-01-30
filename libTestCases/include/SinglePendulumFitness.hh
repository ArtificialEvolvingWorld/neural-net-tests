#ifndef _SINGLEPENDULUMFITNESS_H_
#define _SINGLEPENDULUMFITNESS_H_

#include <functional>

#include "SinglePendulum.hh"
#include "NeuralNet.hh"

std::function<double(NeuralNet&)> single_pendulum_fitness_func(
  SinglePendulum initial_state, double deltaT,
  int num_steps, double max_angle_balanced);

#endif /* _SINGLEPENDULUMFITNESS_H_ */
