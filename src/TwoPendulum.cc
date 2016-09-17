#include "TwoPendulum.hh"

#include <cmath>

TwoPendulum::TwoPendulum()
  : mass_cart(1),
    mass1(1), mass2(1),
    length1(1), length2(2),
    g(1),
    Fext(0),
    x(0), theta1(0), theta2(0),
    v(0), omega1(0), omega2(0) { }

TwoPendulum TwoPendulum::operator+(const TwoPendulum& other) const {
  TwoPendulum output = other;

  output.x += x;
  output.theta1 += theta1;
  output.theta2 += theta2;
  output.v += v;
  output.omega1 += omega1;
  output.omega2 += omega2;

  return output;
}

TwoPendulum TwoPendulum::operator*(double scaling) const {
  TwoPendulum output = *this;

  output.x *= scaling;
  output.theta1 *= scaling;
  output.theta2 *= scaling;
  output.v *= scaling;
  output.omega1 *= scaling;
  output.omega2 *= scaling;

  return output;
}

TwoPendulum TwoPendulum::derivative() const {
  TwoPendulum output = *this;

  output.x = v;
  output.theta1 = omega1;
  output.theta2 = omega2;

  // Double-derivative of x, theta1, theta2
  // Derivation done in two-pendulum-math.py
  double costheta1 = std::cos(theta1);
  double costheta2 = std::cos(theta2);
  double sintheta1 = std::sin(theta1);
  double sintheta2 = std::sin(theta2);

  double A = (- 3*costheta1*costheta1*mass1
              - 3*costheta2*costheta2*mass2
              + 4*(mass1 + mass2 + mass_cart));

  double B1 = length1*mass1*sintheta1*omega1*omega1;
  double B2 = length2*mass2*sintheta2*omega2*omega2;


  double x_ddot = ((+ 4*Fext
                    + 2*B1
                    + 2*B2
                    - 3*costheta1*g*mass1*sintheta1
                    - 3*costheta2*g*mass2*sintheta2)
                   / A);

  double theta1_ddot = (3*(- 4*Fext*costheta1
                           - 2*costheta1*B1
                           - 2*costheta1*B2
                           + 3*costheta1*costheta2*g*mass2*sintheta2
                           - 3*costheta2*costheta2*g*mass2*sintheta1
                           + 4*g*(mass1+mass2+mass_cart)*sintheta1)
                        /
                        (2*length1*A));

  double theta2_ddot = (3*(- 4*Fext*costheta2
                           - 2*costheta2*B1
                           - 2*costheta2*B2
                           - 3*costheta1*costheta1*g*mass1*sintheta2
                           + 3*costheta1*costheta2*g*mass1*sintheta1
                           + 4*g*(mass1+mass2+mass_cart)*sintheta2)
                        /
                        (2*length2*A));

  output.v = x_ddot;
  output.omega1 = theta1_ddot;
  output.omega2 = theta2_ddot;

  return output;
}

std::ostream& operator<<(std::ostream& os, const TwoPendulum& p) {
  os << "x: " << p.x << "\t"
     << "theta1: " << p.theta1 << "\t"
     << "theta2: " << p.theta2;
  return os;
}
