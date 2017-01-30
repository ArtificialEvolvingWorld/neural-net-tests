#ifndef _JUGGLER_H_
#define _JUGGLER_H_

#include <limits>
#include <ostream>
#include <vector>

#include "GVector.hh"

struct Juggler {
  // Simulation-level variables
  std::vector<double> mass_hands;
  std::vector<double> mass_balls;
  double g;
  double ball_size;
  double hand_size;
  double ball_hand_damping;
  std::vector<GVector<2> > hand_centers;
  double max_hand_distance;

  // Direct frame-by-frame input
  std::vector<GVector<2> > force_on_hands;

  // Derived properties
  std::vector<size_t> balls_caught;
  std::vector<bool> hands_closed;

  // Frame-level variables
  std::vector<GVector<2> > hand_positions;
  std::vector<GVector<2> > hand_velocities;
  std::vector<GVector<2> > ball_positions;
  std::vector<GVector<2> > ball_velocities;

  Juggler(size_t num_hands, size_t num_balls);

  Juggler derivative() const;

  void open_hand(size_t ihand);
  void close_hand(size_t ihand);
  void set_hand_state(size_t ihand, bool set_closed);

  friend std::ostream& operator<<(std::ostream& os, const Juggler& j);
};

Juggler operator+(const Juggler& a, const Juggler& b);
Juggler operator*(const Juggler& a, double scaling);

#endif /* _JUGGLER_H_ */
