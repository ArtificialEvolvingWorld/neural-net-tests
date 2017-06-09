#ifndef _SINGLEPENDULUMENTITY_H_
#define _SINGLEPENDULUMENTITY_H_

#include <irrlicht.h>

#include "Entity.hh"
#include "SinglePendulum.hh"
#include "RungeKutta.hh"

class SinglePendulumEntity : public Entity {
public:
  SinglePendulumEntity(SinglePendulum initial);
  ~SinglePendulumEntity();

  virtual void initialize(irr::IrrlichtDevice* device);
  virtual void update(double deltaT, const EventReceiver& events);

  irr::core::vector3df get_position() const { return base_position; }
  void set_position(irr::core::vector3df pos);

private:
  RungeKutta<SinglePendulum> rk;

  irr::scene::ISceneNode* base;
  irr::core::vector3df base_position;

  irr::scene::IMeshSceneNode* cart;
  irr::scene::IMeshSceneNode* rod;
};


#endif /* _SINGLEPENDULUMENTITY_H_ */
