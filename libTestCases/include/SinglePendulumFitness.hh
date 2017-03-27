#ifndef _SINGLEPENDULUMFITNESS_H_
#define _SINGLEPENDULUMFITNESS_H_

#include <functional>

#include "SinglePendulum.hh"
#include "NeuralNet.hh"

std::function<double(NeuralNet&)> single_pendulum_fitness_func_no_velocity(
  SinglePendulum initial_state, double deltaT,
  int num_steps, double max_angle_balanced);

std::function<double(NeuralNet&)> single_pendulum_fitness_func_with_velocity(
  SinglePendulum initial_state, double deltaT,
  int num_steps, double max_angle_balanced);

#endif /* _SINGLEPENDULUMFITNESS_H_ */
