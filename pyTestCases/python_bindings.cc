#include "SinglePendulum.hh"
#include "SinglePendulumFitness.hh"
#include "XorFitness.hh"
#include "RungeKutta.hh"

#include "pybind11/pybind11.h"

namespace py = pybind11;

PYBIND11_PLUGIN(TestCases) {
  py::module m("TestCases", "Description");

  py::module::import("pyneat");
  typedef std::function<double(NeuralNet&)> FitnessFunc;

  py::class_<SinglePendulum>(m, "SinglePendulum")
    .def(py::init<>())
    .def(py::init<const SinglePendulum&>())
    .def_readwrite("mass_cart", &SinglePendulum::mass_cart)
    .def_readwrite("mass_rod", &SinglePendulum::mass_rod)
    .def_readwrite("length", &SinglePendulum::length)
    .def_readwrite("g", &SinglePendulum::g)
    .def_readwrite("Fext", &SinglePendulum::Fext)
    .def_readwrite("x", &SinglePendulum::x)
    .def_readwrite("theta", &SinglePendulum::theta)
    .def_readwrite("v", &SinglePendulum::v)
    .def_readwrite("omega", &SinglePendulum::omega)
    ;

  py::class_<RungeKutta<SinglePendulum> >(m, "SinglePendulum_RungeKutta")
    .def(py::init<SinglePendulum, double>())
    .def_property_readonly("current", &RungeKutta<SinglePendulum>::GetCurrent)
    .def("Step", (void (RungeKutta<SinglePendulum>::*)())&RungeKutta<SinglePendulum>::Step)
    .def_property_readonly("time", &RungeKutta<SinglePendulum>::GetTime)
    ;

  m.def("single_pendulum_fitness_func", single_pendulum_fitness_func,
        py::arg("initial_state"),
        py::arg("deltaT") = 0.1,
        py::arg("num_steps") = 10000,
        py::arg("max_angle_balanced") = 5.0*(3.1415926/180),
        py::doc("Returns a fitness function for single pendulum balancing, with"
                "velocity. Neural nets used should take 2 inputs, and return one"
                "output. max_angle_balanced is the angle from vertical, in radians, for"
                "the pole to be considered balanced.")
  );

  m.def("xor_fitness_func", xor_fitness_func);

  return m.ptr();
}
