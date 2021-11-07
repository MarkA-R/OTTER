#pragma once
class bakeryUtils
{
    
public:
 //https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-enumerations
    //this is why theyre a enum class
    enum class pastryType {
        DOUGH,//uncooked < 10 seconds
        CROISSANT,
        COOKIE,
        CUPCAKE,
        CAKE,
        BURNT
    };

    enum class toppingType {
        NONE,
        CREAM,
        SPRINKLE,
        STRAWBERRY
    };

    enum class fillType {
        NONE,
        JAM,
        CUSTARD,
        CREAM
    };

    static float bakeryUtils::returnCroissantTime()
    {
        return timeToCroissant;
    }
    static float bakeryUtils::returnCookieTime()
    {
        return timeToCookie;
    }
    static float bakeryUtils::returnCupcakeTime()
    {
        return timeToCupcake;
    }
    static float bakeryUtils::returnCakeTime()
    {
        return timeToCake;
    }
    static float bakeryUtils::returnBurnTime()
    {
        return timeToBurn;
    }

    static float bakeryUtils::returnBakeTime(bakeryUtils::pastryType t)
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
    

protected:
    inline static float timeToCroissant = 10;
    inline static float timeToCookie = 20;
    inline static float timeToCupcake = 30;
    inline static float timeToCake = 50;
    inline static float timeToBurn = 60;

     
    

};

