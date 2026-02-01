import numpy as np
import csv
import sys

# Paramètres
rho = 1 / 365.0
beta = 0.5
sigma = 1 / 3.0
gamma = 1 / 7.0

# Conditions initiales
S, E, I, R = 0.999, 0.0, 0.001, 0.0

# Temps
t_start = 0.0
t_end = 730.0
h = 0.1                # pas de temps – 0.1 est un bon compromis
n_steps = int((t_end - t_start) / h) + 1

# Pour sortie CSV
writer = csv.writer(sys.stdout)
writer.writerow(['time', 'S', 'E', 'I', 'R'])
writer.writerow([t_start, S, E, I, R])

t = t_start
for _ in range(1, n_steps):
    dS = rho * R - beta * I * S
    dE = beta * I * S - sigma * E
    dI = sigma * E - gamma * I
    dR = gamma * I - rho * R
    
    S += h * dS
    E += h * dE
    I += h * dI
    R += h * dR
    
    t += h
    writer.writerow([round(t, 2), round(S, 6), round(E, 6), round(I, 6), round(R, 6)])
