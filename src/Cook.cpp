#include "Cook.hpp"
#include "Kitchen.hpp"
#include "ipc.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/file.h>

static void kprint(std::string msg)
{
    flock(fileno(stdout), LOCK_EX);
    write(fileno(stdout), msg.c_str(), msg.size());
    flock(fileno(stdout), LOCK_UN);
}

Cook::Cook(int id, Queue& queue, PlazzaCondVar& bell, PlazzaMutex& bellMutex, bool& isRunning, double multiplier, std::string kitchenId, Kitchen& kitchen)
    : id(id), queue(queue), bell(bell), bellMutex(bellMutex), isRunning(isRunning), multiplier(multiplier), kitchenId(kitchenId), kitchen(kitchen) {}

void Cook::work() {
    while (isRunning) {
        Order currentOrder;
        std::unique_lock<std::mutex> lock(bellMutex.get());
        bell.wait(lock, [this]() {
            return !queue.isEmpty() || !isRunning;
        });
        if (!isRunning && queue.isEmpty()) {
            break; 
        }
        if (queue.tryPop(currentOrder)) {
            lock.unlock();
            std::string pizzaName = currentOrder.args[0];
            while (isRunning && kitchen.takeStock(pizzaName) == false)
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (!isRunning)
                continue;
            kprint("[Cuisine " + kitchenId + "] Cook " + std::to_string(id) + " prepare une " + pizzaName + "\n");
            int baseTime = 0;
            if (pizzaName == "Margarita" || pizzaName == "margarita") {
                baseTime = 1; 
            } else if (pizzaName == "Regina" || pizzaName == "regina") {
                baseTime = 2; 
            } else if (pizzaName == "Americana" || pizzaName == "americana") {
                baseTime = 2; 
            } else if (pizzaName == "Fantasia" || pizzaName == "fantasia") {
                baseTime = 4; 
            }
            double totalTimeSeconds = baseTime * multiplier; 
            int sleepTimeMs = totalTimeSeconds * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
            kprint("[Cuisine " + kitchenId + "] Cook " + std::to_string(id) + " a TERMINE la " + pizzaName + "\n");
            IPC ipc;
            ipc.setId("k" + kitchenId);
            ipc.set_order_done(currentOrder); 
        }
    }
}
