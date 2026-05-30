#include "Kitchen.hpp"
#include "ipc.hpp"
#include <iostream>

Kitchen::Kitchen(double multiplier, int cooksPerKitchen, int replaceTime)
    : multiplier(multiplier), cooksPerKitchen(cooksPerKitchen), replaceTime(replaceTime), isRunning(true)
{
    stockDough = 5;
    stockTomato = 5;
    stockGruyere = 5;
    stockHam = 5;
    stockMushrooms = 5;
    stockSteak = 5;
    stockEggplant = 5;
    stockGoatCheese = 5;
    stockChiefLove = 5;
}

Kitchen::~Kitchen() {
    isRunning = false;
    bell.notifyAll();
    for (long unsigned int i = 0; i < cookThreads.size(); i++) {
        if (cookThreads[i].joinable()) {
            cookThreads[i].join();
        }
    }
}

void Kitchen::loop(std::string id) {
    IPC ipc;
    ipc.init_slave(id);
    for (int i = 0; i < cooksPerKitchen; i++) {
        Cook* newCook = new Cook(i + 1, orders, bell, bellMutex, isRunning, multiplier, id);
        cooks.push_back(newCook);
        cookThreads.push_back(std::thread(&Cook::work, newCook));
    }
    std::vector<int> queueId;
    int inactiveLoops = 0;
    int timeLastRegen = 0;
    while (isRunning) {
        std::vector<Order> nouvellesCommandes = ipc.get_orders();
        bool newOrders = false;
        for (long unsigned int i = 0; i < nouvellesCommandes.size(); i++) {
            Order commande = nouvellesCommandes[i];
            bool isKnow = false;
            for (long unsigned int j = 0; j < queueId.size(); j++) {
                if (QueueId[j] == commande.unique_id) {
                    isKnow = true;
                    break;
                }
            }
            if (isKnow == false) {
                orders.push(commande);
                queueId.push_back(commande.unique_id);
                bell.notifyOne();
                newOrders = true;
            }
        }
        if (newOrders == true) {
            inactiveLoops = 0;
        } else if (orders.isEmpty() == true) {
            inactiveLoops++;
        }
        timeLastRegen += 50;
        if (timeLastRegen >= replaceTime) {
            stockDough++;
            stockTomato++;
            stockGruyere++;
            stockHam++;
            stockMushrooms++;
            stockSteak++;
            stockEggplant++;
            stockGoatCheese++;
            stockChiefLove++;
            timeLastRegen = 0; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (inactiveLoops >= 100) {
            std::cout << "Cuisine " << id << "inactive depuis 5 secondes (fermeture)" << std::endl;
            isRunning = false;
            break;
        }
    }
}