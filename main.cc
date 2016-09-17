#include <iostream>

#include "DoublePendulum.hh"
#include "RungeKutta.hh"

int main() {
  DoublePendulum initial;
  initial.theta1 = 1*(3.1415926/180);

  RungeKutta<DoublePendulum> rk(initial, 0.01);

  for(int i=0; i<100000; i++) {
    // DoublePendulum current = rk.GetCurrent();
    // std::cout << current << std::endl;
    rk.Step();
  }

  std::cout << rk.GetCurrent() << std::endl;
}
