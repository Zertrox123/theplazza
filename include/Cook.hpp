#pragma once
#include "SafeQueue.hpp"
#include "PlazzaCondVar.hpp"
#include "PlazzaMutex.hpp"
#include "ipc.hpp"
#include <string>

class Cook { 
    public:
        Cook(int id, SafeQueue<Order>& queue, PlazzaCondVar& bell, PlazzaMutex& bellMutex, bool& isRunning, double multiplier, std::string kitchenId);
        void work();
    private:
        int id;
        SafeQueue<Order>& queue;
        PlazzaCondVar& bell;
        PlazzaMutex& bellMutex;
        bool& isRunning;
        double multiplier;
        std::string kitchenId;
};