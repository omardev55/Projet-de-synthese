import numpy as np
from scipy.integrate import solve_ivp
import csv
import sys

def seirs(t, y):
    S, E, I, R = y
    dS = (1/365) * R - 0.5 * I * S
    dE = 0.5 * I * S - (1/3) * E
    dI = (1/3) * E - (1/7) * I
    dR = (1/7) * I - (1/365) * R
    return [dS, dE, dI, dR]

y0 = [0.999, 0.0, 0.001, 0.0]
t_span = (0, 730)
t_eval = np.arange(0, 730.1, 0.1)

sol = solve_ivp(seirs, t_span, y0, method='RK45', t_eval=t_eval, rtol=1e-8, atol=1e-8)

writer = csv.writer(sys.stdout)
writer.writerow(['time', 'S', 'E', 'I', 'R'])

for t, y in zip(sol.t, sol.y.T):
    writer.writerow([round(t, 2), round(y[0],6), round(y[1],6), round(y[2],6), round(y[3],6)])
