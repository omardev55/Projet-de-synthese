import sys
import math
import random
import csv

# ------------------------------
# Paramètres du modèle (fixes)
# ------------------------------
GRID_SIZE = 300
N_AGENTS = 20000
DAYS = 730
BETA = 0.5   # force d'infection (dans exp(-BETA * Ni))

# ------------------------------
# PRNG simple et reproductible (Xorshift64)
# ------------------------------
class Xorshift:
    def __init__(self, seed):
        self.state = seed ^ 0xFFFFFFFFFFFFFFFF  # bon mélange initial
        if self.state == 0:
            self.state = 1

    def random(self):
        """Retourne float dans [0, 1)"""
        self.state ^= (self.state << 13) & 0xFFFFFFFFFFFFFFFF
        self.state ^= (self.state >> 7)  & 0xFFFFFFFFFFFFFFFF
        self.state ^= (self.state << 17) & 0xFFFFFFFFFFFFFFFF
        return self.state / 0xFFFFFFFFFFFFFFFF

# ------------------------------
# Tirage exponentiel (durées dE, dI, dR)
# ------------------------------
def neg_exp(mean, rng):
    # -mean * log(1 - U) avec U ~ Uniform(0,1)
    u = rng.random()
    while u == 0:  # évite log(0)
        u = rng.random()
    return math.ceil(-mean * math.log(u))  # ceil pour avoir entier >=1

# ------------------------------
# Classe Agent
# ------------------------------
class Agent:
    def __init__(self, status, de, di, dr, x, y):
        self.status = status          # 0=S, 1=E, 2=I, 3=R
        self.time_in_status = 0
        self.dE = de
        self.dI = di
        self.dR = dr
        self.x = x
        self.y = y

# ------------------------------
# Fonction principale
# ------------------------------
def main():
    if len(sys.argv) != 3:
        print("Usage: python multi_agent.py <rep_id> <output_csv>")
        sys.exit(1)

    rep_id = int(sys.argv[1])
    output_file = sys.argv[2]

    # Seed différent par réplication
    rng = Xorshift(rep_id * 123456789 + 987654321)

    # Création grille (liste de listes d'agents)
    grid = [[[] for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]

    agents = []
    for i in range(N_AGENTS):
        if i < 20:
            status = 2  # I
        else:
            status = 0  # S

        dE = neg_exp(3,   rng)
        dI = neg_exp(7,   rng)
        dR = neg_exp(365, rng)

        x = int(rng.random() * GRID_SIZE)
        y = int(rng.random() * GRID_SIZE)

        agent = Agent(status, dE, dI, dR, x, y)
        grid[x][y].append(agent)
        agents.append(agent)

    # Fichier de sortie
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['time', 'S', 'E', 'I', 'R'])

        for day in range(DAYS + 1):
            # Comptage
            counts = [0] * 4
            for ag in agents:
                counts[ag.status] += 1
            writer.writerow([day] + counts)

            if day == DAYS:
                break

            # Mélange aléatoire des agents → asynchrone
            random.shuffle(agents)   # ← important !

            for agent in agents:
                # 1. Déplacement aléatoire (téléportation complète)
                new_x = int(rng.random() * GRID_SIZE)
                new_y = int(rng.random() * GRID_SIZE)

                grid[agent.x][agent.y].remove(agent)
                agent.x = new_x
                agent.y = new_y
                grid[new_x][new_y].append(agent)

                # 2. Mise à jour du statut (toujours après déplacement)
                agent.time_in_status += 1

                if agent.status == 1:  # E → I ?
                    if agent.time_in_status >= agent.dE:
                        agent.status = 2
                        agent.time_in_status = 0

                elif agent.status == 2:  # I → R ?
                    if agent.time_in_status >= agent.dI:
                        agent.status = 3
                        agent.time_in_status = 0

                elif agent.status == 3:  # R → S ?
                    if agent.time_in_status >= agent.dR:
                        agent.status = 0
                        agent.time_in_status = 0

                elif agent.status == 0:  # S → E ?
                    ni = 0
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            nx = (agent.x + dx) % GRID_SIZE
                            ny = (agent.y + dy) % GRID_SIZE
                            ni += len(grid[nx][ny])   # tous les agents sur la cellule voisine

                    if ni > 0:   # optimisation : pas besoin si 0
                        p = 1 - math.exp(-BETA * ni)
                        if rng.random() < p:
                            agent.status = 1
                            agent.time_in_status = 0

    print(f"Réplication {rep_id} terminée → {output_file}")

if __name__ == "__main__":
    main()
