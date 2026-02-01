# Projet de Synthèse M2 CHPS – Modélisation Épidémiologique SEIRS & Multi-Agent

**Auteur** : Omar  LACHIRI
**Date de rendu** : 1er février 2026  
**Contact** : *omarlachiri112@gmail.com*

---

## Description du projet

Ce projet étudie la dynamique d'une épidémie selon deux approches complémentaires :

1. **Partie 1** : Résolution numérique du modèle compartimental **SEIRS**
   (*Susceptible – Exposed – Infectious – Recovered – Susceptible*) à l'aide
   d'équations différentielles ordinaires (ODE).
2. **Partie 2** : Modèle **multi-agent stochastique** sur une grille torique 2D
   simulant la propagation d'une maladie (infection probabiliste, déplacements
   aléatoires, transitions d'état asynchrones).

Le travail a été réalisé individuellement dans le respect strict des consignes.
Des **LLM (ex. Grok)** ont été utilisés comme aide à la structuration et au
débogage, mais toutes les implémentations ont été **adaptées et personnalisées**
afin d'éviter toute similarité excessive avec d'autres travaux.

---

## Structure du dépôt

```text
.
├── README.md                  # Ce fichier
├── rapport.pdf                # Rapport final (< 30 pages)
│
├── partie1/                   # Résolution ODE SEIRS
│   ├── python/
│   │   ├── seirs_euler.py     # Méthode Euler manuelle
│   │   └── seirs_rk4.py       # Runge-Kutta via scipy.solve_ivp
│   ├── cpp/
│   │   ├── seirs_euler.cpp    # Euler manuel
│   │   ├── seirs_rk4.cpp      # RK4 manuel
│   │   └── Makefile
│   └── results/               # CSV des trajectoires
│
├── partie2/                   # Modèle multi-agent
│   ├── c/
│   │   ├── multi_agent.c      # Implémentation native C
│   │   └── Makefile
│   ├── cpp/
│   │   ├── multi_agent.cpp    # Implémentation native C++ (optionnel)
│   │   └── Makefile
│   ├── python/
│   │   └── multi_agent.py     # Implémentation Python
│   └── results/
│       ├── c/                 # 30 réplications
│       ├── cpp/               # 3 réplications (si implémenté)
│       └── python/            # 3 réplications
│
├── analysis/                  # Notebooks Jupyter
│   ├── analysis_part1.ipynb   # Comparaison ODE (méthodes / langages)
│   └── analysis_part2.ipynb   # Moyennes, pics, ANOVA / Kruskal-Wallis
│
└── figures/                   # Images exportées des notebooks
```
---

## Instructions pour reproduire / vérifier
**Dépendances** 
Python
numpy

scipy

matplotlib

pandas

jupyter
--- 

**Installation :**
pip install numpy scipy matplotlib pandas jupyter
C / C++
gcc / g++

Aucune bibliothèque externe requise

## Lancer les simulations
**Partie 1 – ODE** 
Python
```python partie1/python/seirs_euler.py > partie1/results/python_euler.csv
python partie1/python/seirs_rk4.py   > partie1/results/python_rk4.csv
C++
cd partie1/cpp
make
./euler > ../results/cpp_euler.csv
./rk4   > ../results/cpp_rk4.csv
Partie 2 – Modèle multi-agent
Python (3 réplications)
for i in {1..3}; do
  python partie2/python/multi_agent.py $i partie2/results/python/replication_$i.csv
done
C (30 réplications – très rapide)
cd partie2/c
make
for i in {1..30}; do
  ./multi_agent $i ../results/c/replication_$i.csv
done
```
**Analyses**
Lancer Jupyter :

```jupyter notebook
Puis ouvrir :

analysis/analysis_part1.ipynb → comparaison des méthodes ODE

analysis/analysis_part2.ipynb → moyennes, pics, tests statistiques

Remarques importantes
Toutes les implémentations respectent strictement les contraintes du sujet :
```
**Partie 1 : bibliothèques autorisées (scipy, Boost.odeint)**

**Partie 2 : code natif sans framework externe**
```
Les 30 réplications en C permettent une analyse statistique robuste
(hauteur et date du premier pic d'infectés).
```
**Le rapport rapport.pdf contient :**
```
Sommaire

Introduction

Méthodologie

Résultats

Figures avec légendes

Discussion statistique
```

