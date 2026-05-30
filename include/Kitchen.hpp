#pragma once
#include <string> 
#include <vector> 
#include <thread> 
#include "Cook.hpp" 
#include "SafeQueue.hpp" 
#include "PlazzaCondVar.hpp" 
#include "PlazzaMutex.hpp" 
#include "ipc.hpp" 

class Kitchen { 
    public:
        Kitchen(double multiplier, int cooksPerKitchen, int replaceTime);
        ~Kitchen();
        void loop(std::string id);
    private:
        double multiplier; 
        int cooksPerKitchen; 
        int replaceTime; 
        bool isRunning; 
        Queue<Order> orders; 
        PlazzaCondVar bell; 
        PlazzaMutex bellMutex; 
        std::vector<Cook*> cooks; 
        std::vector<std::thread> cookThreads; 
};
