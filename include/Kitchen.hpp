#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Cook.hpp"
#include "Pizza.hpp"

class Kitchen {
    public:
        Kitchen();
        void add_cook(Cook cook);
        void handle_command(int order_count);
        void actual_time(long int actual_time);
        void handle_stock(std::unordered_map<std::string, int> stock_ingredient);
        const void get_status();
        const void set_status(int stat);
        ~Kitchen();
    private:
        int _multiplier;
        int order_count;
        std::vector<Pizza> waiting_order;
        long int actual_time;
        std::unordered_map<std::string, int> stock_ingredient;
        int status;
};
