import TestCases
import pyneat

from pygui.single_pendulum_opts import SinglePendulumOptions
from pygui.single_pendulum_diagnostics import SinglePendulumDiagnostics
from pygui.xor_diagnostics import XorDiagnostics
from pygui.regression_1d import Regression1DOptions, Regression1DDiagnostics, regression_default_prob

class FunctionConfig:
    def __init__(self, name, generator, num_inputs, num_outputs,
                 options_widget = None, diagnostics_widget = None,
                 default_prob = None, output_activation_func = None):
        self.name = name
        self.generator = generator
        self.num_inputs = num_inputs
        self.num_outputs = num_outputs
        self.options_widget = options_widget
        self.diagnostics_widget = diagnostics_widget
        self.default_prob = default_prob
        self.output_activation_func = output_activation_func


fitness_functions = [
    FunctionConfig('xor',
                   TestCases.xor_fitness_func, 2, 1, None, XorDiagnostics, None, None),

    FunctionConfig('Single Pendulum, With Velocity',
                   TestCases.single_pendulum_fitness_func_with_velocity, 2, 1,
                   SinglePendulumOptions, SinglePendulumDiagnostics(True), None, None),

    FunctionConfig('Single Pendulum, No Velocity',
                   TestCases.single_pendulum_fitness_func_no_velocity, 1, 1,
                   SinglePendulumOptions, SinglePendulumDiagnostics(False), None, None),

    FunctionConfig('1d Regression',
                   TestCases.regression_1d_fitness, 1, 1,
                   Regression1DOptions, Regression1DDiagnostics, regression_default_prob,
                   pyneat.ActivationFunction.Identity),

    ]
