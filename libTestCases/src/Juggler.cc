#include "Juggler.hh"

#include <iostream>

Juggler::Juggler(size_t num_hands, size_t num_balls)
  : mass_hands(num_hands, 1), mass_balls(num_balls, 1),
    g(1), ball_size(1), hand_size(1), ball_hand_damping(1),
    force_on_hands(num_hands, {0,0}),
    balls_caught(num_hands, -1), hands_closed(num_hands, false),
    hand_positions(num_hands, {0,0}), hand_velocities(num_hands, {0,0}),
    ball_positions(num_balls, {0,0}), ball_velocities(num_balls, {0,0}) { }

Juggler operator+(const Juggler& a,
                  const Juggler& b) {
  auto output = a;

  for(size_t i=0; i<a.hand_positions.size(); i++) {
    output.hand_positions[i] += b.hand_positions[i];
    output.hand_velocities[i] += b.hand_velocities[i];
  }
  for(size_t i=0; i<a.ball_positions.size(); i++) {
    output.ball_positions[i] += b.ball_positions[i];
    output.ball_velocities[i] += b.ball_velocities[i];
  }

  return output;
}

Juggler operator*(const Juggler& a,
                  double scaling) {
  auto output = a;

  for(size_t i=0; i<a.hand_positions.size(); i++) {
    output.hand_positions[i] *= scaling;
    output.hand_velocities[i] *= scaling;
  }
  for(size_t i=0; i<a.ball_positions.size(); i++) {
    output.ball_positions[i] *= scaling;
    output.ball_velocities[i] *= scaling;
  }

  return output;
}

Juggler Juggler::derivative() const {
  auto output = *this;

  for(size_t i=0; i<hand_positions.size(); i++) {
    output.hand_positions[i] = hand_velocities[i];
    output.hand_velocities[i] = force_on_hands[i]/mass_hands[i];
  }
  for(size_t i=0; i<ball_positions.size(); i++) {
    output.ball_positions[i] = ball_velocities[i];
    output.ball_velocities[i] = {0, -g}; // Gravity on balls
  }

  // Force between nearby balls
  for(size_t i=0; i<ball_positions.size(); i++ ) {
    for(size_t j=i+1; j<ball_positions.size(); j++) {
      auto displacement = ball_positions[j] - ball_positions[i];
      double distance = displacement.Mag();
      double rel_distance = distance/ball_size;
      if(rel_distance < 1.5) {
        // (1.0/distance) to ensure that balls are infinitely hard at center.
        // 1/(1+e^-t) as a step function, to remove force outside of ball radius.
        // Not just a simple cutoff, because Runge-Kutta does better with smooth functions.
        double force_mag = 200.0 * (1.0/(rel_distance-0.8)) * (1.0 / (1.0 + std::exp((rel_distance - 1.0)/0.1)));
        auto force = force_mag * (displacement / distance);
        output.ball_velocities[i] -= force/mass_balls[i];
        output.ball_velocities[j] += force/mass_balls[j];
      }
    }
  }

  // Force between ball and closed hand.
  // Damping, to bring them to the same velocity.
  for(size_t ihand = 0; ihand<hand_positions.size(); ihand++) {
    size_t iball = balls_caught[ihand];
    if(iball < ball_positions.size()) {
      auto rel_velocity = ball_velocities[iball] - hand_velocities[ihand];

      auto damping = -ball_hand_damping * rel_velocity;
      output.ball_velocities[iball] += damping/mass_balls[iball];
      output.hand_velocities[ihand] -= damping/mass_hands[ihand];
    }
  }

  return output;
}

void Juggler::open_hand(size_t ihand) {
  if(ihand >= hand_positions.size() || !hands_closed[ihand]) {
    return;
  }

  hands_closed[ihand] = false;
  balls_caught[ihand] = -1;
}

void Juggler::close_hand(size_t ihand) {
  if(ihand >= hand_positions.size() || hands_closed[ihand]) {
    return;
  }

  hands_closed[ihand] = true;

  double min_dist2 = std::numeric_limits<double>::max();
  size_t closest=-1;
  for(size_t iball=0; iball<ball_positions.size(); iball++) {
    double dist2 = (hand_positions[ihand] - ball_positions[iball]).Mag2();
    if(dist2 < hand_size*hand_size &&
       dist2 < min_dist2) {
      closest = iball;
      min_dist2 = dist2;
    }
  }

  if(closest < ball_positions.size()) {
    balls_caught[ihand] = closest;
  }
}

void Juggler::set_hand_state(size_t ihand, bool set_closed) {
  if(set_closed){
    close_hand(ihand);
  } else {
    open_hand(ihand);
  }
}

std::ostream& operator<<(std::ostream& os, const Juggler& j) {
  // const char* separator = (j.hand_positions.size() + j.ball_positions.size() < 6) ? "\t" : "\n";

  // os << j.hand_positions.size() << " hands, " << j.ball_positions.size() << " balls:";

  // for(size_t i=0; i<j.hand_positions.size(); i++) {
  //   os << "Hand: " << j.hand_positions[i] << separator;
  // }
  // for(size_t i=0; i<j.ball_positions.size(); i++) {
  //   os << "Ball: " << j.ball_positions[i] << separator;
  // }

  os << "------------------------------\n";
  for(size_t i=0; i<j.hand_positions.size(); i++) {
    os << "Hand: " << j.hand_positions[i] << ", " << j.hand_velocities[i] << "\n";
  }
  for(size_t i=0; i<j.ball_positions.size(); i++) {
    os << "Ball: " << j.ball_positions[i] << ", " << j.ball_velocities[i] << "\n";
  }

  return os;
}
