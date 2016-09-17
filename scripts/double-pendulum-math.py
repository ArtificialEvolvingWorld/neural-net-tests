#!/usr/bin/env python3

import sympy
from sympy import diff
from sympy.vector import CoordSysCartesian

t, mc, m1, m2, L1, L2, g, Fext = sympy.symbols("t mc m1 m2 L1 L2 g Fext")
x, theta1, theta2 = sympy.symbols("x theta1 theta2")
x = x(t)
theta1 = theta1(t)
theta2 = theta2(t)

N = CoordSysCartesian('N')
gvec = -g * N.j

cart_cm = x*N.i
rod1_cm = cart_cm + (L1/2)*(sympy.sin(theta1)*N.i + sympy.cos(theta1)*N.j)
rod2_cm = (cart_cm +
           L1*(sympy.sin(theta1)*N.i + sympy.cos(theta1)*N.j) +
           (L2/2)*(sympy.sin(theta2)*N.i + sympy.cos(theta2)*N.j) )

v_cart_cm = diff(cart_cm,t)
v_rod1_cm = diff(rod1_cm,t)
v_rod2_cm = diff(rod2_cm,t)

Tc = (mc/2)*v_cart_cm.dot(v_cart_cm)
T1 = (m1/2)*v_rod1_cm.dot(v_rod1_cm) + (m1*L1**2/12)*diff(theta1,t)
T2 = (m2/2)*v_rod2_cm.dot(v_rod2_cm) + (m2*L2**2/12)*diff(theta2,t)

T = Tc + T1 + T2

U1 = -m1*rod1_cm.dot(gvec)
U2 = -m2*rod2_cm.dot(gvec)

U = U1 + U2

L = T-U

expr_x = sympy.diff(L, x) - sympy.diff(sympy.diff(L,diff(x,t)), t) + Fext
expr_theta1 = sympy.diff(L, theta1) - sympy.diff(sympy.diff(L,diff(theta1,t)), t)
expr_theta2 = sympy.diff(L, theta2) - sympy.diff(sympy.diff(L,diff(theta2,t)), t)


subs = {diff(x,t):sympy.symbols('xdot'),
        diff(x,t,t):sympy.symbols('xddot'),
        diff(theta1,t):sympy.symbols('theta1dot'),
        diff(theta1,t,t):sympy.symbols('theta1ddot'),
        diff(theta2,t):sympy.symbols('theta2dot'),
        diff(theta2,t,t):sympy.symbols('theta2ddot'),
        }


print('T =',T,'\n')
print('U =',U,'\n')
print('0 =',expr_x,'\n')
print('0 =',expr_theta1,'\n')
print('0 =',expr_theta2,'\n')

res = sympy.solve([expr_x, expr_theta1, expr_theta2],
                  diff(x,t,t),
                  diff(theta1,t,t), diff(theta2,t,t) )


xddot_sol = res[diff(x,t,t)].subs(subs)
t1ddot_sol = res[diff(theta1,t,t)].subs(subs)
t2ddot_sol = res[diff(theta2,t,t)].subs(subs)

print('xddot =',xddot_sol,'\n')
print('theta1ddot =',t1ddot_sol,'\n')
print('theta2ddot =',t2ddot_sol,'\n')

import IPython; IPython.embed()
