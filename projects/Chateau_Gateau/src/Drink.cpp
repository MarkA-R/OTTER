#include "Drink.h"

void Drink::setDrinkType(bakeryUtils::drinkType x)
{
    type = x;
}

bakeryUtils::drinkType Drink::getDrinkType()
{
    return type;
}
