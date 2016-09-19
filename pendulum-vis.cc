#include <memory>

#include <irrlicht.h>

#include "VisualTestViewer.hh"
#include "DoublePendulum.hh"
#include "DoublePendulumEntity.hh"
#include "TwoPendulum.hh"
#include "TwoPendulumEntity.hh"

int main(){
  VisualTestViewer viewer(640, 480);

  {
    DoublePendulum initial;
    initial.length2 = 2;
    initial.mass2 = 1;
    initial.length1 = 4;
    initial.mass1 = 2;
    initial.theta1 = 1*(3.1415926/180);

    auto entity = std::make_unique<DoublePendulumEntity>(initial);
    entity->set_position({0, 5, 0});

    viewer.AddObject(std::move(entity));
  }

  {
    TwoPendulum initial;
    initial.length2 = 2;
    initial.mass2 = 1;
    initial.length1 = 4;
    initial.mass1 = 2;
    initial.theta1 = 1*(3.1415926/180);

    auto entity = std::make_unique<TwoPendulumEntity>(initial);
    entity->set_position({0, 0, 0});

    viewer.AddObject(std::move(entity));
  }

  viewer.Run();
}
