#!/usr/bin/env python2

import sympy
from sympy import diff

sympy.init_printing()

# From http://robotfantastic.org/total-derivatives-in-sympy.html
def difftotal(expr, diffby, diffmap):
    """Take the total derivative with respect to a variable.

    Example:

        theta, t, theta_dot = symbols("theta t theta_dot")
        difftotal(cos(theta), t, {theta: theta_dot})

    returns

        -theta_dot*sin(theta)
    """
    fnexpr = expr.subs({s:s(diffby) for s in diffmap})
    diffexpr = sympy.diff(fnexpr, diffby)
    derivmap = {sympy.Derivative(v(diffby), diffby):dv
                for v,dv in diffmap.iteritems()}
    finaldiff = diffexpr.subs(derivmap)
    return finaldiff.subs({s(diffby):s for s in diffmap})

t, mc, m1, m2, L1, L2, g, Fext = sympy.symbols("t mc m1 m2 L1 L2 g Fext")
x, x_dot, x_ddot = sympy.symbols("x x_dot x_ddot")
theta1, theta1_dot, theta1_ddot = sympy.symbols("theta1 theta1_dot theta1_ddot")
theta2, theta2_dot, theta2_ddot = sympy.symbols("theta2 theta2_dot theta2_ddot")
# x, theta1, theta2 = sympy.symbols("x theta1 theta2")
# x = x(t)
# x_dot = sympy.Derivative(x,t)
# x_ddot = sympy.Derivative(x,t,t)
# theta1 = theta1(t)
# theta1_dot = sympy.Derivative(theta1,t)
# theta1_ddot = sympy.Derivative(theta1,t,t)
# theta2 = theta2(t)
# theta2_dot = sympy.Derivative(theta2,t)
# theta2_ddot = sympy.Derivative(theta2,t,t)

derivatives = {x:x_dot, x_dot:x_ddot,
               theta1:theta1_dot, theta1_dot:theta1_ddot,
               theta2:theta2_dot, theta2_dot:theta2_ddot,
               }

Tc = mc*x_dot**2/2
T1 = m1*x_dot**2/2 + (m1*L1*L1/6)*theta1_dot**2 + m1*L1*theta1_dot*x_dot*sympy.cos(theta1)/2
T2 = m2*x_dot**2/2 + (m2*L2*L2/6)*theta2_dot**2 + m2*L2*theta2_dot*x_dot*sympy.cos(theta2)/2
T = Tc + T1 + T2

print "T =",T,"\n"

U1 = m1*g*(L1/2)*sympy.cos(theta1)
U2 = m2*g*(L2/2)*sympy.cos(theta2)
U = U1 + U2

print "U =",U,"\n"

L = T - U

expr_x = sympy.diff(L, x) - difftotal(sympy.diff(L,x_dot), t, derivatives) + Fext
expr_theta1 = sympy.diff(L, theta1) - difftotal(sympy.diff(L,theta1_dot), t, derivatives)
expr_theta2 = sympy.diff(L, theta2) - difftotal(sympy.diff(L,theta2_dot), t, derivatives)
# expr_x = sympy.diff(L, x) - sympy.diff(sympy.diff(L,x_dot), t) + Fext
# expr_theta1 = sympy.diff(L, theta1) - sympy.diff(sympy.diff(L,theta1_dot), t)
# expr_theta2 = sympy.diff(L, theta2) - sympy.diff(sympy.diff(L,theta2_dot), t)

print "0 =",expr_x,"\n"
print "0 =",expr_theta1,"\n"
print "0 =",expr_theta2,"\n"

# Because otherwise sympy tries to do a bunch of unnecessary trig substitutions
subs = {sympy.cos(theta1):sympy.symbols('costheta1'),
        sympy.sin(theta1):sympy.symbols('sintheta1'),
        sympy.cos(theta2):sympy.symbols('costheta2'),
        sympy.sin(theta2):sympy.symbols('sintheta2'),
        }
expr_x = expr_x.subs(subs)
expr_theta1 = expr_theta1.subs(subs)
expr_theta2 = expr_theta2.subs(subs)

res = sympy.solve([expr_x, expr_theta1, expr_theta2],
                  x_ddot, theta1_ddot, theta2_ddot)

print x_ddot,"=",res[x_ddot].simplify(),"\n"
print theta1_ddot,"=",res[theta1_ddot].simplify(),"\n"
print theta2_ddot,"=",res[theta2_ddot].simplify(),"\n"

import IPython; IPython.embed()
