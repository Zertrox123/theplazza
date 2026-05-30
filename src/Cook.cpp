#include "Cook.hpp" 
#include "ipc.hpp" 
#include <iostream> 
#include <thread> 
#include <chrono> 

Cook::Cook(int id, Queue<Order>& queue, PlazzaCondVar& bell, PlazzaMutex& bellMutex, bool& isRunning, double multiplier, std::string kitchenId)
    : id(id), queue(queue), bell(bell), bellMutex(bellMutex), isRunning(isRunning), multiplier(multiplier), kitchenId(kitchenId) {}

void Cook::work() {
    double totalTimeSeconds = baseTime * multiplier; 
    int sleepTimeMs = totalTimeSeconds * 1000;

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
            std::cout << "[Cuisine " << kitchenId << "] Cook " << id << " prepare une " << pizzaName << std::endl;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
            std::cout << "\n[Cuisine " << _kitchen_id << "] Cook " << _id << " a TERMINE la " << pizza_name << std::endl << "$> ";
            IPC ipc;
            ipc.setId(kitchenId);
            ipc.set_order_done(currentOrder); 
        }
    }
}