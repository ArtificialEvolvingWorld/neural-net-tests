#include <sstream>

#include "DoublePendulum.hh"
#include "Juggler.hh"
#include "Regression1D.hh"
#include "RungeKutta.hh"
#include "SinglePendulum.hh"
#include "SinglePendulumFitness.hh"
#include "TwoPendulum.hh"
#include "XorFitness.hh"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

namespace py = pybind11;

typedef std::vector<GVector<2> > point_list;
PYBIND11_MAKE_OPAQUE(point_list)

PYBIND11_MODULE(TestCases, m) {
  m.doc() = "Description";

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

  m.def("single_pendulum_fitness_func_with_velocity", single_pendulum_fitness_func_with_velocity,
        py::arg("initial_state"),
        py::arg("deltaT") = 0.1,
        py::arg("num_steps") = 10000,
        py::arg("max_angle_balanced") = 5.0*(3.1415926/180),
        py::doc("Returns a fitness function for single pendulum balancing, with"
                "velocity. Neural nets used should take 2 inputs, and return one"
                "output. max_angle_balanced is the angle from vertical, in radians, for"
                "the pole to be considered balanced.")
  );

  m.def("single_pendulum_fitness_func_no_velocity", single_pendulum_fitness_func_no_velocity,
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

  // auto regression_1d_fitness_obj = py::cast<FitnessFunc>(regression_1d_fitness_func());
  // m.add_object("regression_1d_fitness", regression_1d_fitness_obj);

  m.def("regression_1d_fitness", regression_1d_fitness_func,
        py::arg("data") = std::vector<float>({0.0,1.1,2.4,3.9,4.2,5.0,6.5}),
        py::arg("label") = std::vector<float>({0.0,1.1,2.4,3.9,4.2,5.0,6.5})
  );

  py::class_<DoublePendulum>(m, "DoublePendulum")
    .def(py::init<>())
    .def(py::init<const DoublePendulum&>())
    .def_readwrite("mass_cart", &DoublePendulum::mass_cart)
    .def_readwrite("mass1", &DoublePendulum::mass1)
    .def_readwrite("mass2", &DoublePendulum::mass2)
    .def_readwrite("length1", &DoublePendulum::length1)
    .def_readwrite("length2", &DoublePendulum::length2)
    .def_readwrite("g", &DoublePendulum::g)
    .def_readwrite("Fext", &DoublePendulum::Fext)
    .def_readwrite("x", &DoublePendulum::x)
    .def_readwrite("theta1", &DoublePendulum::theta1)
    .def_readwrite("theta2", &DoublePendulum::theta2)
    .def_readwrite("v", &DoublePendulum::v)
    .def_readwrite("omega1", &DoublePendulum::omega1)
    .def_readwrite("omega2", &DoublePendulum::omega2)
    ;

  py::class_<TwoPendulum>(m, "TwoPendulum")
    .def(py::init<>())
    .def(py::init<const TwoPendulum&>())
    .def_readwrite("mass_cart", &TwoPendulum::mass_cart)
    .def_readwrite("mass1", &TwoPendulum::mass1)
    .def_readwrite("mass2", &TwoPendulum::mass2)
    .def_readwrite("length1", &TwoPendulum::length1)
    .def_readwrite("length2", &TwoPendulum::length2)
    .def_readwrite("g", &TwoPendulum::g)
    .def_readwrite("Fext", &TwoPendulum::Fext)
    .def_readwrite("x", &TwoPendulum::x)
    .def_readwrite("theta1", &TwoPendulum::theta1)
    .def_readwrite("theta2", &TwoPendulum::theta2)
    .def_readwrite("v", &TwoPendulum::v)
    .def_readwrite("omega1", &TwoPendulum::omega1)
    .def_readwrite("omega2", &TwoPendulum::omega2)
    ;

  py::class_<Juggler>(m, "Juggler")
    .def(py::init<size_t, size_t>(),
         py::arg("num_hands"),
         py::arg("num_balls"))
    .def(py::init<const Juggler&>())
    .def_readwrite("hand_positions", &Juggler::hand_positions)
    .def_readwrite("ball_positions", &Juggler::ball_positions)
    .def_readwrite("ball_velocities", &Juggler::ball_velocities)
    .def_readwrite("hand_size", &Juggler::hand_size)
    .def("close_hand", &Juggler::close_hand)
    .def("open_hand", &Juggler::open_hand)
    ;

  py::bind_vector<point_list>(m, "GVector2_vector");

  py::class_<GVector<2> >(m, "GVector2")
    .def(py::init<double, double>(),
         py::arg("x"),
         py::arg("y"))
    .def(py::init<const GVector<2>&>())
    .def_property("x",
                  [](const GVector<2>& v) { return (double)v.X(); },
                  [](GVector<2>& v, double x) { v.X() = x; })
    .def_property("y",
                  [](const GVector<2>& v) { return (double)v.Y(); },
                  [](GVector<2>& v, double y) { v.Y() = y; })
    .def("__repr__",
         [](const GVector<2>& v) {
           std::stringstream ss;
           ss << "GVector2(" << v.X() << ", " << v.Y() << ")";
           return ss.str();
         })
    ;
}
