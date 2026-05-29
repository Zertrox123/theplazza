#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Cook.hpp"
#include "Pizza.hpp"

class Kitchen {
    public:
        Kitchen();
        ~Kitchen();
        void add_cook(Cook cook);
        void handle_command(int order_count);
        void actual_time(long int kactual_time);
        void loop(int id);
        void handle_stock(std::unordered_map<std::string, int> stock_ingredient);
        void get_status() const;
        void set_status(int stat) const;
    private:
        int _multiplier;
        int order_count;
        std::vector<Pizza> waiting_order;
        std::vector<Cook> cooks;
        long int kactual_time;
        std::unordered_map<std::string, int> stock_ingredient;
        int status;
};
