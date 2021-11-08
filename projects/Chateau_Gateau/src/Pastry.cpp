#include "Pastry.h"

int Pastry::getCookedSeconds()
{
    return secondsCooked;

}

 void Pastry::addCookedSeconds(int i)
{
    secondsCooked += i;
}

 void Pastry::setType(bakeryUtils::pastryType t)
{
    type = t;
}
 void Pastry::setFilling(bakeryUtils::fillType t)
{
    filling = t;
}
 void Pastry::setTopping(bakeryUtils::toppingType t)
{
    topping = t;
}

 bakeryUtils::pastryType Pastry::getPastryType()
{
    return type;
}

 bakeryUtils::fillType Pastry::getPastryFilling()
{
    return filling;
}

 bakeryUtils::toppingType Pastry::getPastryTopping()
{
    return topping;
}

 float Pastry::getNextCookTime()
{
    if (type == bakeryUtils::pastryType::CROISSANT)
    {
        return bakeryUtils::returnCookieTime();
    }
    else if (type == bakeryUtils::pastryType::COOKIE)
    {
        return bakeryUtils::returnCupcakeTime();
    }
    else if (type == bakeryUtils::pastryType::CUPCAKE)
    {
        return bakeryUtils::returnCakeTime();
    }
    else if (type == bakeryUtils::pastryType::CAKE)
    {
        return bakeryUtils::returnBakeTime(type);
    }
    else if (type == bakeryUtils::pastryType::BURNT)
    {
        return 0;
    }
    else if (type == bakeryUtils::pastryType::DOUGH)
    {
        return bakeryUtils::returnCroissantTime();
    }
    return 0.f;
}


 float Pastry::getCurrentCookedTime()
{
    if (type == bakeryUtils::pastryType::CROISSANT)
    {
        return bakeryUtils::returnCroissantTime();
    }
    else if (type == bakeryUtils::pastryType::COOKIE)
    {
        return bakeryUtils::returnCookieTime();
    }
    else if (type == bakeryUtils::pastryType::CUPCAKE)
    {
        return bakeryUtils::returnCupcakeTime();
    }
    else if (type == bakeryUtils::pastryType::CAKE)
    {
        return bakeryUtils::returnCakeTime();
    }
    else if (type == bakeryUtils::pastryType::BURNT)
    {
        return bakeryUtils::returnBurnTime();
    }
    else if (type == bakeryUtils::pastryType::DOUGH)
    {
        return 0.f;
    }
    return 0.f;
}

 void Pastry::updateType()
{
    //Debug.Log("---");
    if (secondsCooked < bakeryUtils::returnCroissantTime())
    {
        type = bakeryUtils::pastryType::DOUGH;
    }
    else if (secondsCooked == bakeryUtils::returnCroissantTime())
    {
        //Debug.Log("UPDATED");
        type = bakeryUtils::pastryType::CROISSANT;
    }
    else if (secondsCooked == bakeryUtils::returnCookieTime())
    {
        //Debug.Log("UPDATED AGAIN");
        type = bakeryUtils::pastryType::COOKIE;
    }
    else if (secondsCooked == bakeryUtils::returnCupcakeTime())
    {
        type = bakeryUtils::pastryType::CUPCAKE;
    }
    else if (secondsCooked == bakeryUtils::returnCakeTime())
    {
        type = bakeryUtils::pastryType::CAKE;
    }
    else if (secondsCooked > bakeryUtils::returnBurnTime())
    {
        type = bakeryUtils::pastryType::BURNT;
    }
}

 void Pastry::setInOven(bool in)
 {
     inOven = in;
 }

 bool Pastry::isInOven()
 {
     return inOven;
 }
