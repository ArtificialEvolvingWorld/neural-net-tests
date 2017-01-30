#ifndef _XORFITNESS_H_
#define _XORFITNESS_H_

#include <functional>

#include "NeuralNet.hh"

std::function<double(NeuralNet&)> xor_fitness_func();

#endif /* _XORFITNESS_H_ */
