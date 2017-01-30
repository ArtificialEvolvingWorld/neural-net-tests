#ifndef _SINGLEPENDULUM_H_
#define _SINGLEPENDULUM_H_

#include <ostream>

struct SinglePendulum {
  // Simulation-level variables
  double mass_cart;
  double mass_rod;
  double length;
  double g;

  // Force on cart
  double Fext;

  // Frame-level variables
  double x, theta;
  double v, omega;

  SinglePendulum();

  SinglePendulum operator+(const SinglePendulum& other) const;
  SinglePendulum operator*(double scaling) const;

  SinglePendulum derivative() const;

  friend std::ostream& operator<<(std::ostream& os, const SinglePendulum& p);
};



#endif /* _SINGLEPENDULUM_H_ */
