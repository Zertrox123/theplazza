#pragma once
#include <string>
#include <functional>

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
