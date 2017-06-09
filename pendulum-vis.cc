#include <memory>

#include "VisualTestViewer.hh"
#include "DoublePendulum.hh"
#include "DoublePendulumEntity.hh"
#include "TwoPendulum.hh"
#include "TwoPendulumEntity.hh"
#include "SinglePendulum.hh"
#include "SinglePendulumEntity.hh"
#include "CameraEntity.hh"

int main(){
  VisualTestViewer viewer(800, 600);

  {
    auto entity = std::make_unique<CameraEntity>();
    entity->SetMode(CameraEntity::PinToSurface);
    entity->SetPosition({10,-10,10});
    entity->SetLookAt({0,40,0});
    entity->SetMovementSpeed(20);
    viewer.add_object(std::move(entity));
  }

  for(int i=0; i<101; i++) {
    DoublePendulum initial;
    initial.length2 = 2;
    initial.mass2 = 1;
    initial.length1 = 4;
    initial.mass1 = 2;
    initial.theta1 = ((100-i)/100.0)*(3.1415926/180);

    auto entity = std::make_unique<DoublePendulumEntity>(initial);
    entity->set_position({-10, i*3, 0});

    viewer.add_object(std::move(entity));
  }

  for(int i=0; i<101; i++) {
    TwoPendulum initial;
    initial.length2 = 2;
    initial.mass2 = 1;
    initial.length1 = 4;
    initial.mass1 = 2;
    initial.theta1 = ((100-i)/100.0)*(3.1415926/180);

    auto entity = std::make_unique<TwoPendulumEntity>(initial);
    entity->set_position({+10, i*3, 0});

    viewer.add_object(std::move(entity));
  }

  for(int i=0; i<101; i++) {
    SinglePendulum initial;
    initial.length = 2;
    initial.theta = ((100-i)/100.0)*(3.1415926/180);

    auto entity = std::make_unique<SinglePendulumEntity>(initial);
    entity->set_position({+30, i*3, 0});

    viewer.add_object(std::move(entity));
  }

  viewer.run();
}
