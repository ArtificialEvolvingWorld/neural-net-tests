#ifndef _TWOPENDULUM_H_
#define _TWOPENDULUM_H_

#include <ostream>

struct TwoPendulum {
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

  TwoPendulum();

  TwoPendulum operator+(const TwoPendulum& other) const;
  TwoPendulum operator*(double scaling) const;

  TwoPendulum derivative() const;

  friend std::ostream& operator<<(std::ostream& os, const TwoPendulum& p);
};



#endif /* _TWOPENDULUM_H_ */
