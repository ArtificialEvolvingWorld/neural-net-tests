#include "DoublePendulumEntity.hh"

#include <cmath>

#include "ProceduralTexture.hh"

DoublePendulumEntity::DoublePendulumEntity(DoublePendulum initial)
  : rk(initial, 0.01),
    base(nullptr), base_position(0,0,0),
    cart(nullptr), rod1(nullptr), rod2(nullptr) { }

DoublePendulumEntity::~DoublePendulumEntity() {
  if(cart) {
    cart->remove();
  }
  if(rod1) {
    rod1->remove();
  }
  if(rod2) {
    rod2->remove();
  }
  if(base) {
    base->remove();
  }
}

void DoublePendulumEntity::set_position(irr::core::vector3df pos) {
  base_position = pos;
  if(base) {
    base->setPosition(pos);
  }
}

void DoublePendulumEntity::initialize(irr::IrrlichtDevice* device) {
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
  rod1 = smgr->addCubeSceneNode(1.0, base);
  driver->getMeshManipulator()->flipSurfaces(rod1->getMesh());
  rod1->setMaterialTexture(0, create_solid_texture(driver, 0, 255, 0));
  rod1->addShadowVolumeSceneNode();

  // Make the second rod
  rod2 = smgr->addCubeSceneNode(1.0, base);
  driver->getMeshManipulator()->flipSurfaces(rod2->getMesh());
  rod2->setMaterialTexture(0, create_solid_texture(driver, 0, 0, 255));
  rod2->addShadowVolumeSceneNode();
}

void DoublePendulumEntity::update(double deltaT) {
  rk.Step();
  DoublePendulum& current = rk.GetCurrent();

  // Set Cart position
  cart->setPosition({current.x, 0, 0});

  // Set Rod1 position/angle
  rod1->setScale({0.5,0.5,current.length1});
  rod1->setPosition({current.x + current.length1/2 * std::sin(current.theta1), -0.5, current.length1/2 * std::cos(current.theta1)});
  rod1->setRotation({0, current.theta1 * (180/3.1415926), 0});

  // Set Rod2 position/angle
  rod2->setScale({0.5,0.5,current.length2});
  rod2->setPosition({current.x + current.length1 * std::sin(current.theta1) + current.length2/2 * std::sin(current.theta2), -1.0, current.length1 * std::cos(current.theta1) + current.length2/2 * std::cos(current.theta2)});
  rod2->setRotation({0, current.theta2 * (180/3.1415926), 0});
}
