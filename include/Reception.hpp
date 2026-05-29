#pragma once
#include <string>
#include <vector>
#include "Kitchen.hpp"

class Reception {
    public:
        Reception(std::vector<std::string> cooksPerKitchen, int max_capacity, std::vector<int> active_kitchens, int next_kitchen_id);
        void addKitchen(Kitchen kitchen);
        void updateOrder(std::vector<std::string> orderPerKitchen);
        void displayStatus(Kitchen kitchen);
        ~Reception();
    private:
        std::vector<std::string> cooksPerKitchen;
        int max_capacity;
        std::vector<int> active_kitchens;
        int next_kitchen_id;
    protected:
};
