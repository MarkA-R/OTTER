#pragma once
class bakeryUtils
{
public:
	enum pastryType {
        DOUGH,//uncooked < 10 seconds
        CROISSANT,
        COOKIE,
        CUPCAKE,
        CAKE,
        BURNT
	};

    enum toppingType {
        NONE,
        CREAM,
        SPRINKLE,
        STRAWBERRY
    };

    enum fillType {
        NONE,
        JAM,
        CUSTARD,
        CREAM
    };

     float returnCroissantTime()
    {
        return timeToCroissant;
    }
     float returnCookieTime()
    {
        return timeToCookie;
    }
     float returnCupcakeTime()
    {
        return timeToCupcake;
    }
     float returnCakeTime()
    {
        return timeToCake;
    }
     float returnBurnTime()
    {
        return timeToBurn;
    }

     float returnBakeTime(pastryType t)
     {
         if ((int)t == (int)DOUGH)
         {
             return 0.f;
         }
         else if ((int)t == (int)CROISSANT)
         {
             return returnCroissantTime();
         }
         else if ((int)t == (int)COOKIE)
         {
             return returnCookieTime();
         }
         else if ((int)t == (int)CUPCAKE)
         {
             return returnCupcakeTime();
         }
         else if ((int)t == (int)CAKE)
         {
             return returnCakeTime();
         }
         else if ((int)t == (int)BURNT)
         {
             return returnBurnTime();
         }
         return returnBurnTime();
     }

protected:
    const float timeToCroissant = 10;
    const float timeToCookie = 20;
    const float timeToCupcake = 30;
    const float timeToCake = 50;
    const float timeToBurn = 60;
};

