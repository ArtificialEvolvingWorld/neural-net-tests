#include "SinglePendulumEntity.hh"

#include <cmath>

#include "ProceduralTexture.hh"

SinglePendulumEntity::SinglePendulumEntity(SinglePendulum initial)
  : rk(initial, 0.01),
    base(nullptr), base_position(0,0,0),
    cart(nullptr), rod(nullptr) { }

SinglePendulumEntity::~SinglePendulumEntity() {
  if(cart) {
    cart->remove();
  }
  if(rod) {
    rod->remove();
  }
  if(base) {
    base->remove();
  }
}

void SinglePendulumEntity::set_position(irr::core::vector3df pos) {
  base_position = pos;
  if(base) {
    base->setPosition(pos);
  }
}

void SinglePendulumEntity::initialize(irr::IrrlichtDevice* device) {
  auto smgr = device->getSceneManager();
  auto driver = device->getVideoDriver();

  base = smgr->addEmptySceneNode();
  base->setPosition(base_position);

  // Make the cart
  cart = smgr->addCubeSceneNode(
    1.0, base, -1,
    irr::core::vector3df(0,0,0), // position
    irr::core::vector3df(0,0,0), // position
    irr::core::vector3df(4,1,1) // scale
  );
  driver->getMeshManipulator()->flipSurfaces(cart->getMesh());
  cart->setMaterialTexture(0, create_solid_texture(driver, 255, 0, 0));
  cart->addShadowVolumeSceneNode();

  // Make the first rod
  rod = smgr->addCubeSceneNode(1.0, base);
  driver->getMeshManipulator()->flipSurfaces(rod->getMesh());
  rod->setMaterialTexture(0, create_solid_texture(driver, 0, 255, 0));
  rod->addShadowVolumeSceneNode();
}

void SinglePendulumEntity::update(double deltaT, const EventReceiver& /*events*/) {
  rk.Step();
  SinglePendulum& current = rk.GetCurrent();

  // Set Cart position
  cart->setPosition({current.x, 0, 0});

  // Set Rod position/angle
  rod->setScale({0.5,0.5,current.length});
  rod->setPosition({current.x + current.length/2 * std::sin(current.theta), -0.5, current.length/2 * std::cos(current.theta)});
  rod->setRotation({0, current.theta * (180/3.1415926), 0});
}
