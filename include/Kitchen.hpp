#pragma once
#include <string> 
#include <vector> 
#include <thread> 
#include "Cook.hpp" 
#include "Plazza.hpp" 
#include "ipc.hpp" 
#include <functional>

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
        Queue orders;
        PlazzaCondVar bell; 
        PlazzaMutex bellMutex; 
        std::vector<Cook*> cooks; 
        std::vector<std::thread> cookThreads; 
        int stockDough;
        int stockTomato;
        int stockGruyere;
        int stockHam;
        int stockMushrooms;
        int stockSteak;
        int stockEggplant;
        int stockGoatCheese;
        int stockChiefLove;
};
