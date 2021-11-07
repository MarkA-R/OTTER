#include "bakeryUtils.h"
bakeryUtils::bakeryUtils() {
 
}

 float bakeryUtils::returnCroissantTime()
{
    return timeToCroissant;
}
 float bakeryUtils::returnCookieTime()
{
    return timeToCookie;
}
 float bakeryUtils::returnCupcakeTime()
{
    return timeToCupcake;
}
 float bakeryUtils::returnCakeTime()
{
    return timeToCake;
}
 float bakeryUtils::returnBurnTime()
{
    return timeToBurn;
}

 float bakeryUtils::returnBakeTime(bakeryUtils::pastryType t)
{
    if ((int)t == (int)bakeryUtils::pastryType::DOUGH)
    {
        return 0.f;
    }
    else if ((int)t == (int)bakeryUtils::pastryType::CROISSANT)
    {
        return returnCroissantTime();
    }
    else if ((int)t == (int)bakeryUtils::pastryType::COOKIE)
    {
        return returnCookieTime();
    }
    else if ((int)t == (int)bakeryUtils::pastryType::CUPCAKE)
    {
        return returnCupcakeTime();
    }
    else if ((int)t == (int)bakeryUtils::pastryType::CAKE)
    {
        return returnCakeTime();
    }
    else if ((int)t == (int)bakeryUtils::pastryType::BURNT)
    {
        return returnBurnTime();
    }
    return returnBurnTime();
}

