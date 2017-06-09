#include <sstream>

#include "DoublePendulum.hh"
#include "SinglePendulum.hh"
#include "Juggler.hh"
#include "TwoPendulum.hh"

#include "CameraEntity.hh"
#include "DoublePendulumEntity.hh"
#include "JugglerEntity.hh"
#include "SinglePendulumEntity.hh"
#include "TwoPendulumEntity.hh"
#include "VisualTestViewer.hh"

#include "pybind11/pybind11.h"

namespace py = pybind11;

template<typename T, typename... Args>
T* make_obj(VisualTestViewer& viewer, Args... args) {
  auto entity = std::make_unique<T>(std::forward<Args>(args)...);
  auto output = entity.get();
  viewer.add_object(std::move(entity));
  return output;
}

PYBIND11_MODULE(Visualizations, m) {
  m.doc() = "Visualizations for the neural networks";
  py::module::import("TestCases");

  py::class_<VisualTestViewer>(m, "VisualTestViewer")
    .def(py::init<int, int>(),
         py::arg("width"),
         py::arg("height"),
         py::doc("Create a new window with the specified dimensions"))
    .def("run", &VisualTestViewer::run)
    .def("make_camera", make_obj<CameraEntity>,
         py::return_value_policy::reference_internal)
    .def("make_double_pendulum", make_obj<DoublePendulumEntity, DoublePendulum>,
         py::return_value_policy::reference_internal)
    .def("make_two_pendulum", make_obj<TwoPendulumEntity, TwoPendulum>,
         py::return_value_policy::reference_internal)
    .def("make_single_pendulum", make_obj<SinglePendulumEntity, SinglePendulum>,
         py::return_value_policy::reference_internal)
    .def("make_juggler", make_obj<JugglerEntity, Juggler>,
         py::return_value_policy::reference_internal)
    ;

  py::class_<irr::core::vector3df>(m, "vector3df")
    .def(py::init<double, double, double>())
    .def_readwrite("X", &irr::core::vector3df::X)
    .def_readwrite("Y", &irr::core::vector3df::Y)
    .def_readwrite("Z", &irr::core::vector3df::Z)
    .def("__repr__",
         [](irr::core::vector3df& vec) {
           std::stringstream out;
           out << "vector3df(" << vec.X << ", " << vec.Y << ", " << vec.Z << ")";
           return out.str();
         })
    ;

  py::class_<CameraEntity>(m, "CameraEntity")
    .def_property("turn_speed",
                  &CameraEntity::GetTurnSpeed,
                  &CameraEntity::SetTurnSpeed)
    .def_property("movement_speed",
                  &CameraEntity::GetMovementSpeed,
                  &CameraEntity::SetMovementSpeed)
    .def_property("mode",
                  &CameraEntity::GetMode,
                  &CameraEntity::SetMode)
    .def_property("position",
                  &CameraEntity::GetPosition,
                  &CameraEntity::SetPosition)
    .def("look_at",
         &CameraEntity::SetLookAt)
    ;

  py::enum_<CameraEntity::CameraType>(m, "CameraType")
    .value("Flying", CameraEntity::Flying)
    .value("PinToSurface", CameraEntity::PinToSurface)
    ;

  py::class_<DoublePendulumEntity>(m, "DoublePendulumEntity")
    .def_property("position",
                  &DoublePendulumEntity::get_position,
                  &DoublePendulumEntity::set_position)
    ;

  py::class_<TwoPendulumEntity>(m, "TwoPendulumEntity")
    .def_property("position",
                  &TwoPendulumEntity::get_position,
                  &TwoPendulumEntity::set_position)
    ;

  py::class_<SinglePendulumEntity>(m, "SinglePendulumEntity")
    .def_property("position",
                  &SinglePendulumEntity::get_position,
                  &SinglePendulumEntity::set_position)
    ;

  py::class_<JugglerEntity>(m, "JugglerEntity")
    .def_property("position",
                  &JugglerEntity::get_position,
                  &JugglerEntity::set_position)
    ;
}
