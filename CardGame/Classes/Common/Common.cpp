#include <time.h>
#include "Common.h"
#include "../Layer/BaseLayer.h"

BaseLayer*  g_pCurrentLayer = NULL;

int CARD_NUM[] = {
    CARD_1,
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_10,
    CARD_11,
    CARD_12,
    CARD_13,
    CARD_1,
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_10,
    CARD_11,
    CARD_12,
    CARD_13,
    CARD_1,
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_10,
    CARD_11,
    CARD_12,
    CARD_13,
    CARD_1,
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_10,
    CARD_11,
    CARD_12,
    CARD_13,
};

int CARD_FLOWER[] = {
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_CLUB,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_DIAMOND,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_HEART,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
    CARD_SPADE,
};

int getIntFromString(std::string string)
{
    return (int) atoi(string.c_str());
}

float getFloatFromString(std::string string)
{
    return (float) atof(string.c_str());
}

bool getBoolFromString(std::string string)
{
    if(string == "true")
    return true;
    else if(string == "1")
    return true;
    else
    return false;
}

std::string getStringFromInt(int integer)
{
    char szString[32];
    sprintf(szString, "%d", integer);
    return szString;
}

std::string getStringFromFloat(float value)
{
    char szString[32];
    sprintf(szString, "%.2f", value);
    return szString;
}

std::string getStringFromBool(bool value)
{
    if(value)
    return "true";
    else
    return "false";
}

std::string getCardFileName(int value)
{
    char szString[64];
    sprintf(szString, "img/card/card_%02d.png", value);
    return szString;
}
