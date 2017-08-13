from scipy.optimize import fsolve
import math
"""
t = 100000.0
ub = 1600.0 #3400.0
ua = 2850.0
uc = 2730.0
uo = 1776.0
tan_a = 0.24
tan_b = 0.18
c = - ub*uo + ub*uc + uo*uo - uc*uo
#print c
"""

t = 1.0#00000.0
ub = 1600.0
ua = 2730.0
uc = 3400.0
uo = 1776.0
tan_a = 0.4201
tan_b = 0.6598
res = -tan_a *((ua-uo)/t)*((uo-ub)/t)

def equations(x):
    return tan_a*x*x - tan_a *((ua-uo)/t)*((uo-ub)/t) - x*((ua-ub)/t)
#(((ub-uc)*x)/(x*x - ub*uo + ub*uc + uo*uo - uc*uo) - tan_b)#, ((ub-ua)*x)/(x*x - ub*uo + ua*ub + uo*uo - ua*uo) - tan_a)
#x = f, y = u0

x = fsolve(equations, (1))

print equations(x)
x = abs(x)
print x

alph = math.atan((1000.0 - 950.0)/x) - 0.64
l = 24.2/math.sin(alph)

print alph
print l
