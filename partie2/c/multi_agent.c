#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define GRID_SIZE 300
#define N_AGENTS 20000
#define DAYS 730
#define BETA 0.5

typedef struct {
    int status;           // 0=S, 1=E, 2=I, 3=R
    int time_in_status;
    int dE, dI, dR;
    int x, y;
} Agent;

unsigned long long xorshift_state;

double xorshift_rand() {
    xorshift_state ^= xorshift_state << 13;
    xorshift_state ^= xorshift_state >> 7;
    xorshift_state ^= xorshift_state << 17;
    return (double)xorshift_state / (double)(0xFFFFFFFFFFFFFFFFULL);
}

void xorshift_seed(unsigned long long seed) {
    xorshift_state = seed;
    if (xorshift_state == 0) xorshift_state = 1;
}

int neg_exp(double mean) {
    double u = xorshift_rand();
    if (u <= 1e-10 || u >= 1.0 - 1e-10) u = 0.5;
    return (int)ceil(-mean * log(u));
}

int *cell_agents[GRID_SIZE][GRID_SIZE];
int cell_counts[GRID_SIZE][GRID_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <rep_id> <output.csv>\n", argv[0]);
        return 1;
    }

    int rep_id = atoi(argv[1]);
    char *output_file = argv[2];

    xorshift_seed((unsigned long long)rep_id * 123456789ULL + 987654321ULL);

    memset(cell_counts, 0, sizeof(cell_counts));
    memset(cell_agents, 0, sizeof(cell_agents));

    Agent *agents = malloc(N_AGENTS * sizeof(Agent));
    if (!agents) {
        perror("malloc agents");
        return 1;
    }

    for (int i = 0; i < N_AGENTS; i++) {
        agents[i].status = (i < 20) ? 2 : 0;
        agents[i].time_in_status = 0;
        agents[i].dE = neg_exp(3.0);
        agents[i].dI = neg_exp(7.0);
        agents[i].dR = neg_exp(365.0);
        agents[i].x = (int)(xorshift_rand() * GRID_SIZE);
        agents[i].y = (int)(xorshift_rand() * GRID_SIZE);

        int x = agents[i].x, y = agents[i].y;
        int *new_list = realloc(cell_agents[x][y], (cell_counts[x][y] + 1) * sizeof(int));
        if (!new_list) {
            fprintf(stderr, "realloc failed\n");
            free(agents);
            return 1;
        }
        cell_agents[x][y] = new_list;
        cell_agents[x][y][cell_counts[x][y]] = i;
        cell_counts[x][y]++;
    }

    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        perror("fopen");
        free(agents);
        return 1;
    }

    fprintf(fp, "time,S,E,I,R\n");

    int order[N_AGENTS];
    for (int i = 0; i < N_AGENTS; i++) order[i] = i;

    for (int day = 0; day <= DAYS; day++) {
        int counts[4] = {0,0,0,0};
        for (int i = 0; i < N_AGENTS; i++)
            counts[agents[i].status]++;

        fprintf(fp, "%d,%d,%d,%d,%d\n", day, counts[0], counts[1], counts[2], counts[3]);

        if (day == DAYS) break;

        // Fisher-Yates shuffle
        for (int i = N_AGENTS - 1; i > 0; i--) {
            int j = (int)(xorshift_rand() * (i + 1));
            int temp = order[i];
            order[i] = order[j];
            order[j] = temp;
        }

        for (int idx = 0; idx < N_AGENTS; idx++) {
            int i = order[idx];
            Agent *a = &agents[i];

            // Déplacement
            int old_x = a->x, old_y = a->y;
            int new_x = (int)(xorshift_rand() * GRID_SIZE);
            int new_y = (int)(xorshift_rand() * GRID_SIZE);

            // Retirer de l'ancienne cellule
            int *list = cell_agents[old_x][old_y];
            int pos = -1;
            for (int k = 0; k < cell_counts[old_x][old_y]; k++) {
                if (list[k] == i) {
                    pos = k;
                    break;
                }
            }
            if (pos >= 0) {
                list[pos] = list[cell_counts[old_x][old_y] - 1];
                cell_counts[old_x][old_y]--;
            }

            // Ajouter à la nouvelle
            int *new_list = realloc(cell_agents[new_x][new_y], (cell_counts[new_x][new_y] + 1) * sizeof(int));
            if (!new_list) {
                fprintf(stderr, "realloc failed\n");
                fclose(fp);
                free(agents);
                return 1;
            }
            cell_agents[new_x][new_y] = new_list;
            cell_agents[new_x][new_y][cell_counts[new_x][new_y]] = i;
            cell_counts[new_x][new_y]++;

            a->x = new_x;
            a->y = new_y;

            // Mise à jour statut
            a->time_in_status++;

            if (a->status == 1 && a->time_in_status >= a->dE) {
                a->status = 2;
                a->time_in_status = 0;
            } else if (a->status == 2 && a->time_in_status >= a->dI) {
                a->status = 3;
                a->time_in_status = 0;
            } else if (a->status == 3 && a->time_in_status >= a->dR) {
                a->status = 0;
                a->time_in_status = 0;
            } else if (a->status == 0) {
                int ni = 0;
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int nx = (a->x + dx + GRID_SIZE) % GRID_SIZE;
                        int ny = (a->y + dy + GRID_SIZE) % GRID_SIZE;
                        ni += cell_counts[nx][ny];
                    }
                }
                if (ni > 0) {
                    double p = 1.0 - exp(-BETA * (double)ni);
                    if (xorshift_rand() < p) {
                        a->status = 1;
                        a->time_in_status = 0;
                    }
                }
            }
        }
    }

    fclose(fp);

    // Libération mémoire
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            free(cell_agents[i][j]);
        }
    }
    free(agents);

    printf("Réplication %d terminée → %s\n", rep_id, output_file);
    return 0;
}
