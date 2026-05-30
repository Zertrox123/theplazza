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
        Kitchen(double mult, int nb_cooks, int regen);
        ~Kitchen();
        void loop(std::string id);
        bool takeStock(std::string name);
        void cookStart();
        void cookEnd();
        int getBusy();
    private:
        double mult; 
        int nb_cooks; 
        int regen; 
        bool run; 
        Queue orders;
        PlazzaCondVar bell; 
        PlazzaMutex bell_mx;
        PlazzaMutex st_mx;
        std::vector<Cook*> cooks; 
        std::vector<std::thread> threads; 
        int st_dough;
        int st_tomato;
        int st_gruyere;
        int st_ham;
        int st_mush;
        int st_steak;
        int st_egg;
        int st_goat;
        int st_chief;
        int nb_busy;
};
