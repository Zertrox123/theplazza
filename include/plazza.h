#pragma once
#include <string>
#include <vector>

namespace Ingredient {
    enum list_ingr{
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
};

class Kitchen {
    public:
        Kitchen();
        void add_cook(Cook cook);
    private:
        int _multiplier;
};
