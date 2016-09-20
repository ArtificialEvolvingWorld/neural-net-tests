#ifndef _TWOPENDULUMENTITY_H_
#define _TWOPENDULUMENTITY_H_

#include <irrlicht.h>

#include "Entity.hh"
#include "TwoPendulum.hh"
#include "RungeKutta.hh"

class TwoPendulumEntity : public Entity {
public:
  TwoPendulumEntity(TwoPendulum initial);
  ~TwoPendulumEntity();

  virtual void initialize(irr::IrrlichtDevice* device);
  virtual void update(double deltaT, const EventReceiver& events);
  void set_position(irr::core::vector3df pos);

private:
  RungeKutta<TwoPendulum> rk;

  irr::scene::ISceneNode* base;
  irr::core::vector3df base_position;

  irr::scene::IMeshSceneNode* cart;
  irr::scene::IMeshSceneNode* rod1;
  irr::scene::IMeshSceneNode* rod2;
};


#endif /* _TWOPENDULUMENTITY_H_ */
