#ifndef _DOUBLEPENDULUMENTITY_H_
#define _DOUBLEPENDULUMENTITY_H_

#include <irrlicht.h>

#include "Entity.hh"
#include "DoublePendulum.hh"
#include "RungeKutta.hh"

class DoublePendulumEntity : public Entity {
public:
  DoublePendulumEntity(DoublePendulum initial);
  ~DoublePendulumEntity();

  virtual void initialize(irr::IrrlichtDevice* device);
  virtual void update(double deltaT, const EventReceiver& events);
  void set_position(irr::core::vector3df pos);

private:
  RungeKutta<DoublePendulum> rk;

  irr::scene::ISceneNode* base;
  irr::core::vector3df base_position;

  irr::scene::IMeshSceneNode* cart;
  irr::scene::IMeshSceneNode* rod1;
  irr::scene::IMeshSceneNode* rod2;
};


#endif /* _DOUBLEPENDULUMENTITY_H_ */
