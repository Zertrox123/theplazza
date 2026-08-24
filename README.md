# The Plazza

Simulation concurrente et distribuée en C++20 qui gère les commandes d'une pizzeria

Architecture :
- Une réception centrale qui reçoit les commandes et les envoie aux cuisines
- Plusieurs processus cuisines enfants qui gèrent chacun un thread pool de cuisiniers
- Communication par Named Pipes (FIFO) et sockets Unix
- Répartition automatique de la charge et fermeture des cuisines inactives

## Build et lancement

```bash
make
./plazza <multiplier> <cooks_per_kitchen> <restock_time_ms>
# Exemple :
./plazza 2 5 2000
```
