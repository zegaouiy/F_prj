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

def equations(p):
    x,y = p
    return (tan_a*x*x - tan_a *((ua-y)/t)*((y-ub)/t) - x*((ua-ub)/t), tan_b*x*x - tan_b *((uc-y)/t)*((y-ub)/t) - x*((uc-ub)/t)) 
#x = f, y = u0

x, y =  fsolve(equations, (-1,-1))

print equations((x,y))
print x
print y



