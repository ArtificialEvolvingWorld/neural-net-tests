#include "SinglePendulum.hh"

#include <cmath>

SinglePendulum::SinglePendulum()
  : mass_cart(1),
    mass_rod(1), length(1),
    g(1), Fext(0),
    x(0), theta(0),
    v(0), omega(0) { }

SinglePendulum SinglePendulum::operator+(const SinglePendulum& other) const {
  SinglePendulum output = other;

  output.x += x;
  output.theta += theta;
  output.v += v;
  output.omega += omega;

  return output;
}

SinglePendulum SinglePendulum::operator*(double scaling) const {
  SinglePendulum output = *this;

  output.x *= scaling;
  output.theta *= scaling;
  output.v *= scaling;
  output.omega *= scaling;

  return output;
}

SinglePendulum SinglePendulum::derivative() const {
  SinglePendulum output = *this;

  output.x = v;
  output.theta = omega;

  // Double-derivative of x, theta1, theta2
  // Derivation done in single-pendulum-math.py
  double costheta = std::cos(theta);
  double sintheta = std::sin(theta);

  double x_ddot = (Fext + length*mass_rod*omega*omega*sintheta/2 - g*mass_rod*sin(2*theta)/2)/(mass_rod*sintheta*sintheta + mass_cart);
  double theta_ddot = (2*g*(mass_rod + mass_cart)*sintheta - (2*Fext + length*mass_rod*omega*omega*sintheta)*costheta)/(length*(mass_rod*sintheta*sintheta + mass_cart));

  output.v = x_ddot;
  output.omega = theta_ddot;

  return output;
}

std::ostream& operator<<(std::ostream& os, const SinglePendulum& p) {
  os << "x: " << p.x << "\t"
     << "theta: " << p.theta;
  return os;
}
