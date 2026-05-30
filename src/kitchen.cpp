#include "Kitchen.hpp"
#include "ipc.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/file.h>

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

bool Kitchen::takeStock(std::string pizzaName)
{
    stockMutex.lock();
    bool ok = false;
    if (pizzaName == "Margarita" || pizzaName == "margarita") {
        if (stockDough > 0 && stockTomato > 0 && stockGruyere > 0) {
            stockDough--;
            stockTomato--;
            stockGruyere--;
            ok = true;
        }
    } else if (pizzaName == "Regina" || pizzaName == "regina") {
        if (stockDough > 0 && stockTomato > 0 && stockGruyere > 0 && stockHam > 0 && stockMushrooms > 0) {
            stockDough--;
            stockTomato--;
            stockGruyere--;
            stockHam--;
            stockMushrooms--;
            ok = true;
        }
    } else if (pizzaName == "Americana" || pizzaName == "americana") {
        if (stockDough > 0 && stockTomato > 0 && stockGruyere > 0 && stockSteak > 0) {
            stockDough--;
            stockTomato--;
            stockGruyere--;
            stockSteak--;
            ok = true;
        }
    } else if (pizzaName == "Fantasia" || pizzaName == "fantasia") {
        if (stockDough > 0 && stockTomato > 0 && stockEggplant > 0 && stockGoatCheese > 0 && stockChiefLove > 0) {
            stockDough--;
            stockTomato--;
            stockEggplant--;
            stockGoatCheese--;
            stockChiefLove--;
            ok = true;
        }
    }
    stockMutex.unlock();
    return ok;
}

void Kitchen::loop(std::string id) {
    IPC ipc;
    ipc.init_slave("k" + id);
    for (int i = 0; i < cooksPerKitchen; i++) {
        Cook* newCook = new Cook(i + 1, orders, bell, bellMutex, isRunning, multiplier, id, *this);
        cooks.push_back(newCook);
        cookThreads.push_back(std::thread(&Cook::work, newCook));
    }
    std::vector<int> queueId;
    std::vector<int> statusId;
    int inactiveLoops = 0;
    int timeLastRegen = 0;
    while (isRunning) {
        std::vector<Order> nouvellesCommandes = ipc.get_orders();
        bool newOrders = false;
        for (long unsigned int i = 0; i < nouvellesCommandes.size(); i++) {
            Order commande = nouvellesCommandes[i];
            if (commande.id == OSTATUS) {
                if (commande.sender == 0) {
                    bool already = false;
                    for (long unsigned int s = 0; s < statusId.size(); s++) {
                        if (statusId[s] == commande.unique_id) {
                            already = true;
                            break;
                        }
                    }
                    if (already == false) {
                        statusId.push_back(commande.unique_id);
                        int busy = orders.getSize();
                        if (busy > cooksPerKitchen)
                            busy = cooksPerKitchen;
                        stockMutex.lock();
                        std::vector<std::string> args = {
                            id,
                            std::to_string(busy),
                            std::to_string(cooksPerKitchen),
                            std::to_string(orders.getSize()),
                            std::to_string(stockDough),
                            std::to_string(stockTomato),
                            std::to_string(stockGruyere),
                            std::to_string(stockHam),
                            std::to_string(stockMushrooms),
                            std::to_string(stockSteak),
                            std::to_string(stockEggplant),
                            std::to_string(stockGoatCheese),
                            std::to_string(stockChiefLove)
                        };
                        stockMutex.unlock();
                        ipc.send_order_broadcast(OSTATUS, std::stoi(id) + 1, args);
                    }
                }
                continue;
            }
            bool isKnow = false;
            for (long unsigned int j = 0; j < queueId.size(); j++) {
                if (queueId[j] == commande.unique_id) {
                    isKnow = true;
                    break;
                }
            }
            if (isKnow == false && orders.getSize() < 2 * cooksPerKitchen) {
                orders.push(commande);
                queueId.push_back(commande.unique_id);
                bell.notifyOne();
                newOrders = true;
            }
        }
        for (long unsigned int j = 0; j < queueId.size(); ) {
            bool found = false;
            for (long unsigned int i = 0; i < nouvellesCommandes.size(); i++) {
                if (queueId[j] == nouvellesCommandes[i].unique_id) {
                    found = true;
                    break;
                }
            }
            if (found == false)
                queueId.erase(queueId.begin() + j);
            else
                j++;
        }
        if (newOrders == true) {
            inactiveLoops = 0;
        } else if (orders.isEmpty() == true && queueId.empty() == true) {
            inactiveLoops++;
        }
        timeLastRegen += 50;
        if (timeLastRegen >= replaceTime) {
            stockMutex.lock();
            stockDough++;
            stockTomato++;
            stockGruyere++;
            stockHam++;
            stockMushrooms++;
            stockSteak++;
            stockEggplant++;
            stockGoatCheese++;
            stockChiefLove++;
            stockMutex.unlock();
            timeLastRegen = 0;
            bell.notifyAll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (inactiveLoops >= 100 && std::stoi(id) >= 3) {
            std::string msg = "Cuisine " + id + "inactive depuis 5 secondes (fermeture)\n";
            flock(fileno(stdout), LOCK_EX);
            write(fileno(stdout), msg.c_str(), msg.size());
            flock(fileno(stdout), LOCK_UN);
            isRunning = false;
            break;
        }
    }
}
