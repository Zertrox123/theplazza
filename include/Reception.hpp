#pragma once
#include <string>
#include <vector>
#include "Kitchen.hpp"

class Reception {
    public:
        Reception();
        void addKitchen(Kitchen kitchen);
        void updateOrder(std::vector<std::string> orderPerKitchen);
        void displayStatus(Kitchen kitchen);
        ~Reception();
    private:
        std::vector<std::string> orderPerKitchen;
    protected:
};
