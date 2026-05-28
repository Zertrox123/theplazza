#pragma once
#include <vector>
#include "Ingredient.hpp"

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
