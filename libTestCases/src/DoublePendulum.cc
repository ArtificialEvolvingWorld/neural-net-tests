#include "DoublePendulum.hh"

#include <cmath>

DoublePendulum::DoublePendulum()
  : mass_cart(1),
    mass1(1), mass2(1),
    length1(1), length2(2),
    g(1),
    Fext(0),
    x(0), theta1(0), theta2(0),
    v(0), omega1(0), omega2(0) { }

DoublePendulum DoublePendulum::operator+(const DoublePendulum& other) const {
  DoublePendulum output = other;

  output.x += x;
  output.theta1 += theta1;
  output.theta2 += theta2;
  output.v += v;
  output.omega1 += omega1;
  output.omega2 += omega2;

  return output;
}

DoublePendulum DoublePendulum::operator*(double scaling) const {
  DoublePendulum output = *this;

  output.x *= scaling;
  output.theta1 *= scaling;
  output.theta2 *= scaling;
  output.v *= scaling;
  output.omega1 *= scaling;
  output.omega2 *= scaling;

  return output;
}

DoublePendulum DoublePendulum::derivative() const {
  DoublePendulum output = *this;

  output.x = v;
  output.theta1 = omega1;
  output.theta2 = omega2;

  // Double-derivative of x, theta1, theta2
  // Derivation done in double-pendulum-math.py
  double costheta1 = std::cos(theta1);
  double costheta2 = std::cos(theta2);
  double sintheta1 = std::sin(theta1);
  double sintheta2 = std::sin(theta2);

  double sinthetadiff = std::sin(theta1 - theta2);
  double costhetadiff = std::cos(theta1 - theta2);

  double m_tot = mass1 + mass2 + mass_cart;

  double A = (-4*mass2*(mass1 + 2*mass2)*costhetadiff*costheta1*costheta2 + mass2*(mass1 + 4*mass2)*costheta2*costheta2 + 4*mass2*m_tot*costhetadiff*costhetadiff + (mass1 + 2*mass2)*(mass1 + 2*mass2)*costheta1*costheta1 - (mass1 + 4*mass2)*m_tot);
  double B = (-length1*omega1*omega1*sinthetadiff - g*sintheta2);
  double C = (2*Fext + length1*mass1*omega1*omega1*sintheta1 + 2*length1*mass2*omega1*omega1*sintheta1 + length2*mass2*omega2*omega2*sintheta2);
  double D = (-2*(mass1 + 2*mass2)*costhetadiff*costheta1 + (mass1 + 4*mass2)*costheta2);
  double E = (2*mass2*costhetadiff*costheta2 - (mass1 + 2*mass2)*costheta1);
  double F = (length2*mass2*omega2*omega2*sinthetadiff - g*mass1*sintheta1 - 2*g*mass2*sintheta1);
  double G = ((mass1 + 2*mass2)*costheta1*costheta2 - 2*m_tot*costhetadiff);

  double x_ddot = ((-mass2*B*D + E*F - (mass1 - 4*mass2*costhetadiff*costhetadiff + 4*mass2)*C/2)
                   /
                   A);

  double theta1_ddot = ((2*mass2*B*G - E*C + 2*(mass1 + mass2*sintheta2*sintheta2 + mass_cart)*F)
                        /
                        (length1*A));

  double theta2_ddot = ((-2*((mass1 + 2*mass2)*(mass1 + 2*mass2)*costheta1*costheta1 - (mass1 + 4*mass2)*m_tot)*B + D*C + 2*G*F)
                        /
                        (length2*A));


  output.v = x_ddot;
  output.omega1 = theta1_ddot;
  output.omega2 = theta2_ddot;

  return output;
}

std::ostream& operator<<(std::ostream& os, const DoublePendulum& p) {
  os << "x: " << p.x << "\t"
     << "theta1: " << p.theta1 << "\t"
     << "theta2: " << p.theta2;
  return os;
}
