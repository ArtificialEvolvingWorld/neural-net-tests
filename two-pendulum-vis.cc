#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include <irrlicht.h>

#include "EventReceiver.hh"
#include "ProceduralTexture.hh"
#include "TrapMouse.hh"
#include "PanningCamera.hh"

#include "TwoPendulum.hh"
#include "RungeKutta.hh"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int main(){
  TwoPendulum initial;
  initial.length1 = 2;
  initial.length2 = 4;
  initial.theta1 = 1*(3.1415926/180);

  RungeKutta<TwoPendulum> rk(initial, 0.01);

  EventReceiver receiver;
  IrrlichtDevice* device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640,480), 16,
        		     false, true, false, &receiver);
  if (!device){
    return 1;
  }

  device->setWindowCaption(L"Hello World! - Rewriting test");

  auto driver = device->getVideoDriver();
  auto smgr = device->getSceneManager();
  auto guienv = device->getGUIEnvironment();

  auto text = guienv->addStaticText(L"",
                                    rect<s32>(10,10,260,22), true);

  // Set the ambient light
  //smgr->setAmbientLight({0.1,0.1,0.1});

  PanningCamera camera(device);
  camera.setPosition({0,-10,0});
  camera.setTarget({0,0,0});
  camera.setSkyVector({0,0,1});

  // Make the cart
  scene::IMeshSceneNode* cart = smgr->addCubeSceneNode(
    1.0, NULL, -1,
    core::vector3df(0,0,0), // position
    core::vector3df(0,0,0), // position
    core::vector3df(4,1,1) // scale
  );
  driver->getMeshManipulator()->flipSurfaces(cart->getMesh());
  cart->setPosition(core::vector3df(0,0,0));
  cart->setMaterialTexture(0, create_solid_texture(driver, 255, 0, 0));
  cart->addShadowVolumeSceneNode();

  // Make the first rod
  scene::IMeshSceneNode* rod1 = smgr->addCubeSceneNode(
    1.0, NULL, -1,
    core::vector3df(0,0,0), // position
    core::vector3df(0,0,0), // rotation
    core::vector3df(0.5,0.5,initial.length1) // scale
  );
  driver->getMeshManipulator()->flipSurfaces(rod1->getMesh());
  rod1->setPosition(core::vector3df(0,0,0));
  rod1->setMaterialTexture(0, create_solid_texture(driver, 0, 255, 0));
  rod1->addShadowVolumeSceneNode();

  // Make the second rod
  scene::IMeshSceneNode* rod2 = smgr->addCubeSceneNode(
    1.0, NULL, -1,
    core::vector3df(0,0,0), // position
    core::vector3df(0,0,0), // rotation
    core::vector3df(0.5,0.5,initial.length2) // scale
  );
  driver->getMeshManipulator()->flipSurfaces(rod2->getMesh());
  rod2->setPosition(core::vector3df(0,0,0));
  rod2->setMaterialTexture(0, create_solid_texture(driver, 0, 0, 255));
  rod2->addShadowVolumeSceneNode();


  // Add a light source
  scene::ILightSceneNode* light = smgr->addLightSceneNode(0, {25,-25,50}, {1,1,1}, 100);

  u32 prev_time = device->getTimer()->getTime();
  float movement_speed = 5.0;
  float camera_movement_speed = 5.0;

  while(device->run()){
    // Find dT
    auto now = device->getTimer()->getTime();
    double dT = (now - prev_time) / 1000.0;

    if(dT < 0.01) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    prev_time = now;

    // if(!device->isWindowActive()){
    //   device->yield();
    //   continue;
    // }

    rk.Step();
    TwoPendulum& current = rk.GetCurrent();

    if(device->isWindowActive()) {
      camera.Apply(camera_movement_speed * dT);
    }

    // Adjust force based on the controls
    if(receiver.IsKeyDown(irr::KEY_KEY_A)) {
      current.Fext = -1;
    } else if(receiver.IsKeyDown(irr::KEY_KEY_D)) {
      current.Fext = +1;
    } else {
      current.Fext = 0;
    }

    // Set Cart position
    cart->setPosition({current.x, 0, 0});

    // Set Rod1 position/angle
    rod1->setPosition({current.x-1 + current.length1/2 * std::sin(current.theta1), 0, current.length1/2 * std::cos(current.theta1)});
    rod1->setRotation({0, current.theta1 * (180/3.1415926), 0});

    // Set Rod2 position/angle
    rod2->setPosition({current.x+1 + current.length2/2 * std::sin(current.theta2), 0, current.length2/2 * std::cos(current.theta2)});
    rod2->setRotation({0, current.theta2 * (180/3.1415926), 0});

    // Adjust the FPS counter
    std::wstringstream ss;
    ss << "FPS: " << driver->getFPS()
       << " Mouse: (" << receiver.mouse().x << "," << receiver.mouse().y << ")"
       << " Mouse: (" << device->getCursorControl()->getRelativePosition().X << ","
       << device->getCursorControl()->getRelativePosition().Y << ")";
    auto wstring = ss.str();
    text->setText(wstring.c_str());

    // Draw everything
    driver->beginScene(true, true, SColor(255,100,101,140));
    smgr->drawAll();
    guienv->drawAll();
    driver->endScene();
  }

  device->drop();
  return 0;
}
