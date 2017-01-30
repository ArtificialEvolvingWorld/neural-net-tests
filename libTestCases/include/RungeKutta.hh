#ifndef _RUNGEKUTTA_H_
#define _RUNGEKUTTA_H_

#include <functional>

//typename F must act as a field.
//It must be possible to add together two Fs to receive an F
// and multiply an F by a double to receive and F.
template<typename F>
class RungeKutta{

  // Helper struct, allows for derivative method instead of derivative function.
  template<typename T>
  struct function_gen {
    static std::function<T(const T&,double)> derivative_method() {
      return [](const T& t, double /*time*/) {
        return t.derivative();
      };
    }
  };

public:
  RungeKutta(F initial, std::function<F(const F&,double)> derivative,
             double deltaT, double start_time=0)
    : m_current(initial), m_der(derivative),
      m_deltaT(deltaT), m_time(start_time) { }

  template<typename Dummy = void>
  RungeKutta(F initial, double deltaT, double start_time = 0)
    : m_current(initial),
      m_deltaT(deltaT), m_time(start_time) {
    m_der = function_gen<F>::derivative_method();
  }

  void Step(double dT){
    RungeKuttaStep(m_current, m_time, m_der, dT);
  }

  void Step(){ Step(m_deltaT); }
  F& GetCurrent(){ return m_current; }
  double GetTime(){ return m_time; }

private:
  F m_current;
  std::function<F(const F&,double)> m_der;
  double m_deltaT;
  double m_time;
};

template<typename F, typename D>
void RungeKuttaStep(F& state, double& time, D der, double dT){
  F k1 = der(state, time)*dT;
  F k2 = der(state + k1*0.5, time + 0.5*dT)*dT;
  F k3 = der(state + k2*0.5, time + 0.5*dT)*dT;
  F k4 = der(state + k3, time + dT)*dT;
  state = state + (k1 + k2*2 + k3*2 + k4) * (1.0/6.0);
  time += dT;
}

#endif /* _RUNGEKUTTA_H_ */
