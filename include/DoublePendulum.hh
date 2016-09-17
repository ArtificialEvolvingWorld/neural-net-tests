#ifndef _DOUBLEPENDULUM_H_
#define _DOUBLEPENDULUM_H_

#include <ostream>

struct DoublePendulum {
  // Simulation-level variables
  double mass_cart;
  double mass1, mass2;
  double length1, length2;
  double g;

  // Force on cart
  double Fext;

  // Frame-level variables
  double x, theta1, theta2;
  double v, omega1, omega2;

  DoublePendulum();

  DoublePendulum operator+(const DoublePendulum& other) const;
  DoublePendulum operator*(double scaling) const;

  DoublePendulum derivative() const;

  friend std::ostream& operator<<(std::ostream& os, const DoublePendulum& p);
};



#endif /* _DOUBLEPENDULUM_H_ */
