#ifndef _VISUAL_TEST_VIEWER_H_
#define _VISUAL_TEST_VIEWER_H_

#include <memory>
#include <vector>

#include "Entity.hh"

class EventReceiver;
class PanningCamera;
namespace irr {
  class IrrlichtDevice;
  namespace scene {
    class ISceneManager;
    class IMeshSceneNode;
  }
  namespace gui {
    class IGUIEnvironment;
    class IGUIStaticText;
  }
  namespace video {
    class IVideoDriver;
  }
}

class VisualTestViewer {
public:
  VisualTestViewer(int width, int height);
  ~VisualTestViewer();

  void add_object(std::unique_ptr<Entity> entity) {
    entity->initialize(device);
    entities.push_back(std::move(entity));
  }
  void run();

private:
  //void initialize_camera();
  void initialize_objects();
  void update_camera(double dT);
  void update_fps();

  std::unique_ptr<EventReceiver> receiver;
  irr::IrrlichtDevice* device;
  irr::video::IVideoDriver* driver;
  irr::gui::IGUIEnvironment* guienv;
  irr::scene::ISceneManager* smgr;

  std::vector<std::unique_ptr<Entity> > entities;

  //std::unique_ptr<PanningCamera> camera;

  irr::gui::IGUIStaticText* fps_text;
};

#endif /* _VISUAL_TEST_VIEWER_H_ */