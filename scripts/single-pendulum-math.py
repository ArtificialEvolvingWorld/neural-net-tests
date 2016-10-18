#!/usr/bin/env python3

import sympy
from sympy import diff
from sympy.vector import CoordSysCartesian

t, mc, m_rod, L, g, Fext = sympy.symbols("t mc m_rod L g Fext")
x, theta = sympy.symbols("x theta")
x = x(t)
theta = theta(t)

N = CoordSysCartesian('N')
gvec = -g * N.j

cart_cm = x*N.i
rod_cm = cart_cm + (L/2)*(sympy.sin(theta)*N.i + sympy.cos(theta)*N.j)

v_cart_cm = diff(cart_cm,t)
v_rod_cm = diff(rod_cm,t)

Tc = (mc/2)*v_cart_cm.dot(v_cart_cm)
T_rod = (m_rod/2)*v_rod_cm.dot(v_rod_cm) + (m_rod*L**2/12)*diff(theta,t)

T = Tc + T_rod
U = -m_rod*rod_cm.dot(gvec)

L = T-U

expr_x = sympy.diff(L, x) - sympy.diff(sympy.diff(L,diff(x,t)), t) + Fext
expr_theta = sympy.diff(L, theta) - sympy.diff(sympy.diff(L,diff(theta,t)), t)


subs = {diff(x,t):sympy.symbols('xdot'),
        diff(x,t,t):sympy.symbols('xddot'),
        diff(theta,t):sympy.symbols('thetadot'),
        diff(theta,t,t):sympy.symbols('thetaddot'),
        }


print('T =',T,'\n')
print('U =',U,'\n')
print('0 =',expr_x,'\n')
print('0 =',expr_theta,'\n')

res = sympy.solve([expr_x, expr_theta],
                  diff(x,t,t),
                  diff(theta,t,t) )


xddot_sol = res[diff(x,t,t)].subs(subs)
t1ddot_sol = res[diff(theta,t,t)].subs(subs)

print('xddot =',xddot_sol,'\n')
print('thetaddot =',t1ddot_sol,'\n')

import IPython; IPython.embed()
