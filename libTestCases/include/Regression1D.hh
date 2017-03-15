#pragma once

#include <functional>

#include "NeuralNet.hh"

std::function<double(NeuralNet&)> regression_1d_fitness_func(std::vector<float> data, std::vector<float> label);
