#ifndef __CARDGAME_COMMON_H__
#define __CARDGAME_COMMON_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace  cocos2d::network;
using namespace CocosDenshion;

static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);

enum TYPE_CARD_FLOWER
{
    CARD_NONE = 0,
    CARD_CLUB = 1,
    CARD_DIAMOND = 2,
    CARD_HEART = 3,
    CARD_SPADE = 4,
};

enum TYPE_CARD_NUMBER
{
    CARD_1 = 1,
    CARD_2 = 2,
    CARD_3 = 3,
    CARD_4 = 4,
    CARD_5 = 5,
    CARD_6 = 6,
    CARD_7 = 7,
    CARD_8 = 8,
    CARD_9 = 9,
    CARD_10 = 10,
    CARD_11 = 11,
    CARD_12 = 12,
    CARD_13 = 13,
};

class BaseLayer;
extern BaseLayer*   g_pCurrentLayer;
extern int CARD_NUM[];
extern int CARD_FLOWER[];

#define PLAYER_OFFLINE      0
#define PLAYER_ONLINE       1

#define STATE_LOBBY         0
#define STATE_IDLE          1
#define STATE_FIND_1        2
#define STATE_FIND_5        3
#define STATE_FIND_FRIEND   4
#define STATE_GAME          5

#define ROOM_TYPE_1         0
#define ROOM_TYPE_5         1
#define ROOM_TYPE_FRIEND    2

#define RES_FAILED          0
#define RES_SUCCESS         1

#define DEFAULT_LEVEL       0

#define EMAIL_TYPE_NONE          0
#define EMAIL_TYPE_ADD_FRIEND    1
#define EMAIL_TYPE_REMOVE_FRIEND 2
#define EMAIL_TYPE_ACCEPT_FRIEND 3
#define EMAIL_TYPE_DECLINE_FRIEND 4

#define EMAIL_ACTION_NONE       0
#define EMAIL_ACTION_ACCEPT     1
#define EMAIL_ACTION_DECLINE    2

#define FRIEND_MATCH_REQ_SEND       0
#define FRIEND_MATCH_REQ_CLOSE      1
#define FRIEND_MATCH_REQ_ACCEPT     2
#define FRIEND_MATCH_REQ_DECLINE    3
#define FRIEND_MATCH_REQ_COIN       4

#define COIN_PER_MATCH              50
#define LEVELUP_PER_MATCH           10

int         getIntFromString(std::string string);
float       getFloatFromString(std::string string);
bool        getBoolFromString(std::string string);
std::string getStringFromInt(int integer);
std::string getStringFromFloat(float value);
std::string getStringFromBool(bool value);
std::string getCardFileName(int value);

struct LeaveInfo
{
    std::string         strRoomId;
    std::string         strUserId;
};

struct GameResultInfo
{
    std::string         strRoomId;
    std::string         strWinnerId;
    int                 nWinnerCoin;
};

struct PutCardInfo
{
    std::string         strRoomId;
    int                 nPrevTune;
    int                 nCurTune;
    std::vector<int>    vecCardValues;
    int                 nCardFlower;
    bool                bDealNewCard;
    int                 nNewDealCount;
    std::string         strPickState;
};

struct MailActionInfo
{
    int             nStatus;
    std::string     strMailId;
    int             nAction;
};

struct RemoveFriendResult
{
    int             nStatus;
    std::string     strFriendId;
};

struct NotifyFriendMatch
{
    int             nAction;
    std::string     strFriendId;
    std::string     strFriendName;
    int             nGameCoin;
};

struct FriendMatchReqResult
{
    int             nStatus;
    int             nAction;
};

#endif /* __CARDGAME_COMMON_H__ */
