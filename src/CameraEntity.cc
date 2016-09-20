#include "CameraEntity.hh"

#include <algorithm>
#include <cmath>

#include "EventReceiver.hh"

CameraEntity::CameraEntity()
  : device(nullptr), camera(nullptr),
    sky_vector(0,0,1),
    theta(M_PI/2), phi(0), type(PinToSurface),
    turn_speed(0.1), movement_speed(1.0) { }

CameraEntity::~CameraEntity() {
  if(camera) {
    camera->remove();
  }
}

void CameraEntity::initialize(irr::IrrlichtDevice* device) {
  this->device = device;
  auto smgr = device->getSceneManager();
  camera = smgr->addCameraSceneNode();
  InitCameraPerspective();
}

void CameraEntity::update(double deltaT, const EventReceiver& events) {
  if(!camera || !device) {
    return;
  }

  auto lower_right_int = device->getVideoDriver()->getViewPort().LowerRightCorner;
  irr::core::vector2df lower_right(lower_right_int.X, lower_right_int.Y);
  auto cursor_pos = device->getCursorControl()->getRelativePosition();
  auto disp = (cursor_pos - irr::core::position2df(0.5,0.5)) * lower_right;

  // Re-center the mouse
  device->getCursorControl()->setPosition(0.5f, 0.5f);

  theta += disp.Y * turn_speed * (M_PI/180);
  theta = std::max(0.0, std::min(theta, M_PI)); // Clamp to range of 0 to pi
  phi -= disp.X * turn_speed * (M_PI/180);

  auto forward = GetForward();
  auto right = GetRight();

  irr::core::vector3df direction(0,0,0);
  if(events.IsKeyDown(irr::KEY_KEY_W)){
    direction += forward;;
  }
  if(events.IsKeyDown(irr::KEY_KEY_S)){
    direction -= forward;
  }
  if(events.IsKeyDown(irr::KEY_KEY_A)){
    direction -= right;
  }
  if(events.IsKeyDown(irr::KEY_KEY_D)){
    direction += right;
  }
  if(events.IsKeyDown(irr::KEY_SPACE)){
    direction += sky_vector;
  }
  if(events.IsKeyDown(irr::KEY_KEY_C)){
    direction -= sky_vector;
  }

  if(direction.getLength()) {
    direction /= direction.getLength();
    position += deltaT * movement_speed * direction;
  }
  camera->setPosition(position);
  camera->setTarget(position+LookDir());
}

irr::core::vector3df CameraEntity::LookDir() {
  return {std::cos(phi)*std::sin(theta),
      std::sin(phi)*std::sin(theta),
      std::cos(theta)};
}

irr::core::vector3df CameraEntity::GetRight() {
  return {std::sin(phi),
      -std::cos(phi),
      0};
}

irr::core::vector3df CameraEntity::GetForward() {
  switch(type) {
    case Flying:
      return LookDir();

    case PinToSurface:
      return {std::cos(phi),
          std::sin(phi),
          0};

    default:
      return {1, 0, 0};
  }
}

void CameraEntity::SetLookAt(irr::core::vector3df look_at) {
  auto dir = look_at - position;
  phi = std::atan2(dir.Y, dir.X);
  theta = std::acos(dir.Z/dir.getLength());
}

void CameraEntity::InitCameraOrtho(){
  irr::core::matrix4 matproj;

  matproj.buildProjectionMatrixOrthoRH(15,15,0,200);
  // Because the z has the negative sign, when the x should.
  matproj(2,2) *= -1;
  matproj(0,0) *= -1;

  camera->setProjectionMatrix(matproj,true);
  camera->setPosition(position);
  camera->setTarget(position + GetForward());
  camera->setUpVector(sky_vector);
}

void CameraEntity::InitCameraPerspective(){
  camera->setPosition(position);
  camera->setTarget(position + GetForward());
  camera->setUpVector(sky_vector);

  camera->setAspectRatio(4.0/3.0);
  camera->setFOV(1.0);

  irr::core::matrix4 matproj = camera->getProjectionMatrix();
  matproj(0,0) *= -1;
  camera->setProjectionMatrix(matproj);
}
