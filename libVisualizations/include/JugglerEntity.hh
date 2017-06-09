#ifndef _JUGGLERENTITY_H_
#define _JUGGLERENTITY_H_

#include <irrlicht.h>

#include "Entity.hh"
#include "Juggler.hh"
#include "RungeKutta.hh"

class JugglerEntity : public Entity {
public:
  JugglerEntity(Juggler initial);
  ~JugglerEntity();

  virtual void initialize(irr::IrrlichtDevice* device);
  virtual void update(double deltaT, const EventReceiver& events);

  irr::core::vector3df get_position() const { return base_position; }
  void set_position(irr::core::vector3df pos);

private:
  RungeKutta<Juggler> rk;

  irr::scene::ISceneNode* base;
  irr::core::vector3df base_position;

  std::vector<irr::scene::IMeshSceneNode*> hands;
  std::vector<irr::scene::IMeshSceneNode*> balls;
};

#endif /* _JUGGLERENTITY_H_ */
