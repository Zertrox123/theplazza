#pragma once
#include "Plazza.hpp"
#include "ipc.hpp"
#include <string>
#include <functional>

class Kitchen;

class Cook { 
    public:
        Cook(int id, Queue& q, PlazzaCondVar& bell, PlazzaMutex& mx, bool& run, double mult, std::string kid, Kitchen& k);
        void work();
    private:
        int id;
        Queue& q;
        PlazzaCondVar& bell;
        PlazzaMutex& mx;
        bool& run;
        double mult;
        std::string kid;
        Kitchen& k;
};
