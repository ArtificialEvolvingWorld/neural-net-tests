#ifndef _CAMERAENTITY_H_
#define _CAMERAENTITY_H_

#include <irrlicht.h>

#include "Entity.hh"

class CameraEntity : public Entity {
public:
  enum CameraType {
    Flying,
    PinToSurface
  };

  CameraEntity();
  ~CameraEntity();

  virtual void initialize(irr::IrrlichtDevice* device);
  virtual void update(double deltaT, const EventReceiver& events);

  void SetMode(CameraType type) {
    this->type = type;
  }

  void SetPosition(irr::core::vector3df pos) {
    position = pos;
  }

  void SetLookAt(irr::core::vector3df look_at);

private:
  void InitCameraOrtho();
  void InitCameraPerspective();

  irr::core::vector3df GetForward();
  irr::core::vector3df GetRight();
  irr::core::vector3df LookDir();

  irr::IrrlichtDevice* device;
  irr::scene::ICameraSceneNode* camera;

  irr::core::vector3df sky_vector;
  irr::core::vector3df position;
  double theta;
  double phi;
  CameraType type;


  double turn_speed; // Speed of turning, in degrees/pixel
  double movement_speed; // Speed of movement, in units/second
};

#endif /* _CAMERAENTITY_H_ */
