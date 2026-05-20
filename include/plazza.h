#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace Ingredient {
    enum list_ingr {
        Dough,
        Tomato,
        Eggplant,
        GoatChesse,
        ChiefLove,
    };
    Ingredient::list_ingr to_string(Ingredient::list_ingr ingr);
    std::string from_string(std::string ingr);
}

enum PizzaType
{
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8
};

enum PizzaSize
{
    S = 1,
    M = 2,
    L = 4,
    XL = 8,
    XXL = 16
};

class Pizza {
    public:
        PizzaType type;
        PizzaSize size;
        std::vector<Ingredient::list_ingr> ReqIngredient;
};

class Cook {
    public:
        Cook();
        void start();
        ~Cook();
    private:
};

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