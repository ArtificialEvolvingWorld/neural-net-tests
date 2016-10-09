#include "VisualTestViewer.hh"
#include "JugglerEntity.hh"
#include "CameraEntity.hh"

int main() {
  VisualTestViewer viewer(800, 600);

  {
    auto entity = std::make_unique<CameraEntity>();
    entity->SetMode(CameraEntity::PinToSurface);
    entity->SetPosition({10,-10,10});
    entity->SetLookAt({0,00,0});
    entity->SetMovementSpeed(20);
    viewer.AddObject(std::move(entity));
  }

  {
    Juggler initial(2,3);
    initial.hand_positions[0] = {-1, 0};
    initial.hand_positions[1] = {+1, 0};

    initial.ball_positions[0] = {-1, 1};

    initial.ball_positions[1] = {+1, 1};
    initial.ball_velocities[1] = {0, 5};

    initial.ball_positions[2] = {0, 5};

    initial.hand_size = 2;
    initial.close_hand(0);


    auto entity = std::make_unique<JugglerEntity>(initial);
    viewer.AddObject(std::move(entity));
  }

  viewer.Run();
}
