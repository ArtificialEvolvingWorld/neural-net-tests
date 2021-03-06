#include "VisualTestViewer.hh"

#include <chrono>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <thread>

#include <irrlicht.h>

#include "EventReceiver.hh"
#include "PanningCamera.hh"
#include "ProceduralTexture.hh"

VisualTestViewer::VisualTestViewer(int window_id) {
  irr::SIrrlichtCreationParameters params;
  params.WindowId = reinterpret_cast<void*>(window_id);
  setup(params);
}

VisualTestViewer::VisualTestViewer(int width, int height) {
  irr::SIrrlichtCreationParameters params;
  params.WindowSize = irr::core::dimension2d<irr::u32>(width, height);
  setup(params);
}

VisualTestViewer::~VisualTestViewer() {
  device->drop();
}

void VisualTestViewer::setup(irr::SIrrlichtCreationParameters& params) {
  receiver = std::make_unique<EventReceiver>();

  params.DriverType = irr::video::EDT_OPENGL;
  params.Bits = 16;
  params.EventReceiver = receiver.get();
  params.Stencilbuffer = true;

  device = createDeviceEx(params);

  if(!device) {
    throw std::runtime_error("Could not open irrlicht device");
  }

  driver = device->getVideoDriver();
  guienv = device->getGUIEnvironment();
  smgr = device->getSceneManager();
  device->setWindowCaption(L"Neural Net Tests");

  initialize_objects();
}

void VisualTestViewer::initialize_objects() {
  // Add a light source
  auto light = smgr->addLightSceneNode(0, {25,-25,50}, {1,1,1}, 100);

  // Add the FPS counter
  fps_text = guienv->addStaticText(L"",
                                   irr::core::rect<irr::s32>(10,10,260,22), true);
}

void VisualTestViewer::update_fps() {
  // Adjust the FPS counter
  std::wstringstream ss;
  ss << "FPS: " << driver->getFPS()
     << " Mouse: (" << receiver->mouse().x << "," << receiver->mouse().y << ")"
     << " Mouse: (" << device->getCursorControl()->getRelativePosition().X << ","
     << device->getCursorControl()->getRelativePosition().Y << ")";
  auto wstring = ss.str();
  fps_text->setText(wstring.c_str());
}

void VisualTestViewer::run() {
  auto prev_time = device->getTimer()->getTime();

  while(device->run()) {
    // Find dT
    auto now = device->getTimer()->getTime();
    double dT = (now - prev_time) / 1000.0;
    if(dT < 0.01) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    prev_time = now;

    for(auto& entity : entities) {
      entity->update(dT, *receiver);
    }

    // // Adjust force based on the controls
    // double force = 0;
    // if(receiver->IsKeyDown(irr::KEY_KEY_A)) {
    //   force = -1;
    // } else if(receiver->IsKeyDown(irr::KEY_KEY_D)) {
    //   force = +1;
    // }


    update_fps();

    // Draw everything
    driver->beginScene(true, true, irr::video::SColor(255,100,101,140));
    smgr->drawAll();
    guienv->drawAll();
    driver->endScene();
  }
}
