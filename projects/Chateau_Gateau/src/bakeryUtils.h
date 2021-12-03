#pragma once
#include <string>
#include <iostream>

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
        PECAN,
        SPRINKLE,
        STRAWBERRY
    };

    enum class fillType {
        NONE,
        CUSTARD,
        CHOCOLATE,
        JAM
        
    };

    enum class drinkType {
        NONE,
        COFFEE,
        MILKSHAKE,
        TEA//short for bubble tea
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

   static void addToGameTime(float dt) {
        gameTime += dt;
    }

    static float getTime() {
        return gameTime;
    }

    static void addToDifficulty(int d) {
        difficulty += d;
    }

    static int getDifficulty() {
        return difficulty;
    }
    
    static std::string getToppingName(bakeryUtils::toppingType type) {
        if (type == bakeryUtils::toppingType::NONE)
        {
            return "NONE";
        }
        else if (type == bakeryUtils::toppingType::PECAN)
        {
            return "PECAN";
        }
        else if (type == bakeryUtils::toppingType::STRAWBERRY)
        {
            return "STRAWBERRY";
        }
        else if (type == bakeryUtils::toppingType::SPRINKLE)
        {
            return "SPRINKLE";
        }
    }
    static std::string getFillingName(bakeryUtils::fillType filling) {
        if (filling == bakeryUtils::fillType::NONE)
        {
            return "NONE";
        }
        else if (filling == bakeryUtils::fillType::CHOCOLATE)
        {
            return "CHOCOLATE";
        }
        else if (filling == bakeryUtils::fillType::CUSTARD)
        {
            return "CUSTARD";
        }
        else if (filling == bakeryUtils::fillType::JAM)
        {
            return "JAM";
        }
    }
   static std::string getPastryName(bakeryUtils::pastryType type) {
        if (type == bakeryUtils::pastryType::CROISSANT)
        {
           return "CROISSANT";
        }
        else if (type == bakeryUtils::pastryType::COOKIE)
        {
            return "COOKIE";
        }
        else if (type == bakeryUtils::pastryType::CUPCAKE)
        {
            return "CUPCAKE";
        }
        else if (type == bakeryUtils::pastryType::CAKE)
        {
            return "CAKE";
        }
    }

   static void addToRounds(int i) {
       roundsLasted += i;
   }
   static int getRoundsLasted() {
       return roundsLasted;
   }

   static void addToFailed(int i) {
       ordersFailed += i;
   }
   static int getOrdersFailed() {
       return ordersFailed;
   }
   static void setOrdersFailed(int x) {
      ordersFailed = x;
   }
   static void setRoundsLasted(int x) {
       roundsLasted = x;
   }
   static void setDifficulty(int x) {
       difficulty = x;
   }
   static void setTime(int x) {
       gameTime = x;
   }

   static float getDrinkFillAmount() {
       return drinkFillAmount;
   }
protected:
    inline static float timeToCroissant = 10;
    inline static float timeToCookie = 20;
    inline static float timeToCupcake = 30;
    inline static float timeToCake = 50;
    inline static float timeToBurn = 60;
    inline static float drinkFillAmount = 5.f;
    inline static float gameTime = 0;
    inline static int difficulty = 1;
    inline static int roundsLasted = 0;
    inline static int ordersFailed = 0;
     
    

};

