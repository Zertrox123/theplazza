#pragma once
#include "Plazza.hpp"
#include "ipc.hpp"
#include <string>
#include <functional>

class Kitchen;

class Cook { 
    public:
        Cook(int id, Queue& queue, PlazzaCondVar& bell, PlazzaMutex& bellMutex, bool& isRunning, double multiplier, std::string kitchenId, Kitchen& kitchen);
        void work();
    private:
        int id;
        Queue& queue;
        PlazzaCondVar& bell;
        PlazzaMutex& bellMutex;
        bool& isRunning;
        double multiplier;
        std::string kitchenId;
        Kitchen& kitchen;
};
