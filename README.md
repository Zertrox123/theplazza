# The Plazza

High-concurrency distributed simulation engine in modern C++20.

## Overview

Simulates a pizzeria workload distribution system using child processes, IPC Named Pipes / Unix Domain Sockets, and Thread Pools.

## Getting Started

### Build & Run

```bash
# Compile with C++20
make

# Run simulation: ./plazza <multiplier> <cooks_per_kitchen> <restock_time_ms>
./plazza 2 5 2000
```
