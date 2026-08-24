# The Plazza

Moteur de simulation distribuée à haute concurrence en C++20.

## Présentation

The Plazza (G-CCP-400) modélise la réception et la préparation de commandes dans une chaîne de pizzerias via une architecture distribuée multi-processus et multi-threads.

Architecture :
- **Processus Réception** : Parse les commandes et les distribue équitablement aux cuisines.
- **Processus Cuisines (enfants)** : Chaque cuisine gère un Thread Pool de cuisiniers avec sémaphores et mutexes.
- **Communication Inter-Processus (IPC)** : Named Pipes (FIFOs) et sockets de domaine Unix.
- **Gestion dynamique de charge** : Création automatique de nouvelles cuisines et arrêt des cuisines inactives.

## Prérequis

- Compilateur compatible C++20 (g++ 11+ ou clang++ 13+)
- Make

## Compilation et Lancement

```bash
# Compiler avec le standard C++20
make

# Lancer la simulation : ./plazza <multiplicateur_temps> <cuisiniers_par_cuisine> <temps_recharge_ms>
./plazza 2 5 2000
```
