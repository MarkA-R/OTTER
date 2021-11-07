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

    static float returnCroissantTime();

    static float returnCookieTime();

    static float returnCupcakeTime();

    static float returnCakeTime();

    static float returnBurnTime();

    static float returnBakeTime(bakeryUtils::pastryType t);

    bakeryUtils();

protected:
    inline static float timeToCroissant = 10;
    inline static float timeToCookie = 20;
    inline static float timeToCupcake = 30;
    inline static float timeToCake = 50;
    inline static float timeToBurn = 60;

     
    

};

