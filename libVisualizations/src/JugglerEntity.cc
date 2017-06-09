#include "JugglerEntity.hh"

#include "ProceduralTexture.hh"

namespace {
  irr::core::vector3df convert(GVector<2> vec) {
    return {vec.X(), 0, vec.Y()};
  }
}

JugglerEntity::JugglerEntity(Juggler initial)
  : rk(initial, 0.01),
    base(nullptr), base_position(0,0,0) { }

JugglerEntity::~JugglerEntity() {
  for(auto hand : hands) {
    hand->remove();
  }

  for(auto ball : balls) {
    ball->remove();
  }
}


void JugglerEntity::initialize(irr::IrrlichtDevice* device) {
  auto smgr = device->getSceneManager();
  auto driver = device->getVideoDriver();

  base = smgr->addEmptySceneNode();
  base->setPosition(base_position);

  const auto& current = rk.GetCurrent();

  for(size_t i=0; i<current.hand_positions.size(); i++) {
    auto new_hand = smgr->addCubeSceneNode(1.0, base);
    driver->getMeshManipulator()->flipSurfaces(new_hand->getMesh());
    new_hand->setMaterialTexture(0, create_solid_texture(driver, 255, 0, 0));
    new_hand->addShadowVolumeSceneNode();

    hands.push_back(new_hand);
  }

  for(size_t i=0; i<current.ball_positions.size(); i++) {
    auto new_ball = smgr->addSphereSceneNode(1.0, 16, base);
    driver->getMeshManipulator()->flipSurfaces(new_ball->getMesh());
    new_ball->setMaterialTexture(0, create_solid_texture(driver, 0, 255, 0));
    new_ball->addShadowVolumeSceneNode();

    balls.push_back(new_ball);
  }
}

void JugglerEntity::update(double deltaT, const EventReceiver& events) {
  rk.Step();
  auto& current = rk.GetCurrent();

  for(size_t i=0; i<current.hand_positions.size(); i++) {
    hands[i]->setPosition(convert(current.hand_positions[i]));
  }
  for(size_t i=0; i<current.ball_positions.size(); i++) {
    balls[i]->setPosition(convert(current.ball_positions[i]));
  }
}

void JugglerEntity::set_position(irr::core::vector3df pos) {
  base_position = pos;
  if(base) {
    base->setPosition(pos);
  }
}
