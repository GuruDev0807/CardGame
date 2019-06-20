#include "PacketParser.h"
#include "Packet.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"
#include "../Data/Notification.h"
#include "../Widget/CardSprite.h"
#include "../Xml/NotificationParser.h"

PacketParser* PacketParser::_instance = NULL;

PacketParser* PacketParser::getInstance()
{
    if(_instance == NULL)
        _instance = new PacketParser();
    
    return _instance;
}

PacketParser::PacketParser()
{
    init();
}

void PacketParser::init()
{
}

std::string PacketParser::makeUserInfo(std::string deviceId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\"}", PT_FIELD_DEVICEID, deviceId.c_str());
    return szString;
}

std::string PacketParser::makeRequestMatch(std::string userId, int roomType, int nCoin)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":%d, \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOMTYPE, roomType, PT_FIELD_COIN, nCoin);
    return szString;
}

std::string PacketParser::makeRequestJoinRoom(std::string userId, std::string roomId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, roomId.c_str());
    return szString;
}

std::string PacketParser::makeRequestCloseMatch(std::string userId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str());
    return szString;
}

std::string PacketParser::makeRequestLeaveGame(std::string userId, std::string roomId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, roomId.c_str());
    return szString;
}

std::string PacketParser::makeRequestPutCard(std::string userId, std::string roomId, std::vector<int> vecCards, int cardFlower)
{
    std::string strCards = "[";
    for(int index = 0; index < vecCards.size(); index++)
    {
        strCards += std::to_string(vecCards[index]);
        if(index != vecCards.size() - 1)
            strCards += ", ";
    }
    strCards += "]";
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\", \"%s\":%s, \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, roomId.c_str(),
            PT_FIELD_CARDNUM, strCards.c_str(), PT_FIELD_CARD_FLOWER, cardFlower);
    return szString;
}

std::string PacketParser::makeRequestStart(std::string userId, std::string roomId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, roomId.c_str());
    return szString;
}

std::string PacketParser::makeRequestRestart(std::string userId, std::string roomId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, roomId.c_str());
    return szString;
}

std::string PacketParser::makeRequestUpdateProfile(std::string userId, std::string new_name, std::string new_avatar, std::string new_avatar_url)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\", \"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_NAME, new_name.c_str(),
            PT_FIELD_AVATAR, new_avatar.c_str(), PT_FIELD_AVATAR_URL, new_avatar_url.c_str());
    return szString;
}

std::string PacketParser::makeRequestUserList(std::string search_key)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\"}", PT_FIELD_SEARCH, search_key.c_str());
    return szString;
}

std::string PacketParser::makeRequestAddFriend(std::string userId, std::vector<std::string> new_friends)
{
    std::string strFriends = "[";
    for(int index = 0; index < new_friends.size(); index++)
    {
        strFriends += "\"";
        strFriends += new_friends[index];
        strFriends += "\"";
        if(index != new_friends.size() - 1)
            strFriends += ", ";
    }
    strFriends += "]";

    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":%s}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_FRIENDS, strFriends.c_str());
    return szString;
}

std::string PacketParser::makeRequestAcceptFriend(std::string userId, std::string notification_id, int action)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\", \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ID, notification_id.c_str(), PT_FIELD_ACTION, action);
    return szString;
}

std::string PacketParser::makeRequestRemoveFriend(std::string userId, std::string friend_id)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_FRIEND_ID, friend_id.c_str());
    return szString;
}

std::string PacketParser::makeRequestNotificationList(std::string userId)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str());
    return szString;
}

std::string PacketParser::makeRequestFriendList(std::string userId, std::vector<std::string> friends)
{
    std::string strFriends = "[";
    for(int index = 0; index < friends.size(); index++)
    {
        strFriends += "\"";
        strFriends += friends[index];
        strFriends += "\"";
        if(index != friends.size() - 1)
            strFriends += ", ";
    }
    strFriends += "]";
    
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":%s}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_FRIENDS, strFriends.c_str());
    return szString;
}

std::string PacketParser::makeRequestFriendMatchReq(std::string userId, std::string userName, std::vector<std::string> friends, int action, int coin)
{
    std::string strFriends = "[";
    for(int index = 0; index < friends.size(); index++)
    {
        strFriends += "\"";
        strFriends += friends[index];
        strFriends += "\"";
        if(index != friends.size() - 1)
            strFriends += ", ";
    }
    strFriends += "]";
    
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\", \"%s\":%s, \"%s\":%d, \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_USER_NAME, userName.c_str(), PT_FIELD_FRIENDS, strFriends.c_str(), PT_FIELD_ACTION, action, PT_FIELD_COIN, coin);
    return szString;
}

std::string PacketParser::makeRequestFriendMatchRes(std::string userId, std::string userName, std::string strFriendId, int action, int coin)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\", \"%s\":\"%s\", \"%s\":%d, \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_USER_NAME, userName.c_str(), PT_FIELD_FRIEND_ID, strFriendId.c_str(), PT_FIELD_ACTION, action, PT_FIELD_COIN, coin);
    return szString;
}

std::string PacketParser::makeRequestUpdateCoinReq(std::string userId, int coin)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":%d}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_COIN, coin);
    return szString;
}

std::string PacketParser::makeRequestCheckNameReq(std::string userId, std::string name)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_NAME, name.c_str());
    return szString;
}

std::string PacketParser::makeRequestKnock(std::string userId, std::string room_id)
{
    char szString[256];
    sprintf(szString, "{\"%s\":\"%s\", \"%s\":\"%s\"}", PT_FIELD_USERID, userId.c_str(), PT_FIELD_ROOM_ID, room_id.c_str());
    return szString;
}

UserInfo* PacketParser::parseUserInfo(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    if(!document.HasMember(PT_FIELD_SUCCESS))
        return NULL;
    
    if(document[PT_FIELD_SUCCESS].GetInt() == RES_FAILED)
        return NULL;
    
    auto pUserInfo = new UserInfo();
    pUserInfo->m_strId = document[PT_FIELD_PROFILE][PT_FIELD_ID].GetString();
    pUserInfo->m_strSession = document[PT_FIELD_PROFILE][PT_FIELD_SESSION].GetString();
    pUserInfo->m_strName = document[PT_FIELD_PROFILE][PT_FIELD_NAME].GetString();
    pUserInfo->m_strAvatar = document[PT_FIELD_PROFILE][PT_FIELD_AVATAR].GetString();
    pUserInfo->m_strAvatarUrl = document[PT_FIELD_PROFILE][PT_FIELD_AVATAR_URL].GetString();
    pUserInfo->m_strDevice = document[PT_FIELD_PROFILE][PT_FIELD_DEVICE].GetString();
    pUserInfo->m_nOnline = document[PT_FIELD_PROFILE][PT_FIELD_ONLINE].GetInt();
    pUserInfo->setLevel(document[PT_FIELD_PROFILE][PT_FIELD_LEVEL].GetInt());
    pUserInfo->m_nWinCount = document[PT_FIELD_PROFILE][PT_FIELD_WINCNT].GetInt();
    pUserInfo->m_nLostCount = document[PT_FIELD_PROFILE][PT_FIELD_LOSECNT].GetInt();
    pUserInfo->m_nCoin = document[PT_FIELD_PROFILE][PT_FIELD_COIN].GetInt();
    pUserInfo->m_strRoomId = document[PT_FIELD_PROFILE][PT_FIELD_ROOMID].GetString();
    pUserInfo->m_nState = document[PT_FIELD_PROFILE][PT_FIELD_STATE].GetInt();
    pUserInfo->m_nHistory = document[PT_FIELD_PROFILE][PT_FIELD_HISTORY].GetInt();
    pUserInfo->m_nHistoryCount = document[PT_FIELD_PROFILE][PT_FIELD_HISTORYCNT].GetInt();
    
    const rapidjson::Value& friends = document[PT_FIELD_PROFILE][PT_FIELD_FRIENDS];
    for(rapidjson::Value::ConstValueIterator it1 = friends.Begin(); it1 != friends.End(); ++it1)
    {
        const rapidjson::Value& friendId = *it1;
        std::string strFriendId = friendId[PT_FIELD_ID].GetString();
        pUserInfo->m_vecFriends.push_back(strFriendId);
    }
    
    return pUserInfo;
}

std::string PacketParser::parseMatchResponse(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    return document[PT_FIELD_ROOM_ID].GetString();
}

GameRoom* PacketParser::parseGameRoom(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    if(!document.HasMember(PT_FIELD_SUCCESS))
        return NULL;
    
    if(document[PT_FIELD_SUCCESS].GetInt() == RES_FAILED)
        return NULL;
    
    auto pGameRoom = new GameRoom();
    pGameRoom->m_strId = document[PT_FIELD_ROOM][PT_FIELD_ID].GetString();
    pGameRoom->m_nType = document[PT_FIELD_ROOM][PT_FIELD_TYPE].GetInt();
    pGameRoom->m_nGameCoin = document[PT_FIELD_ROOM][PT_FIELD_COIN].GetInt();
    pGameRoom->m_nStarted = document[PT_FIELD_ROOM][PT_FIELD_STARTED].GetInt();
    pGameRoom->m_nMaxMember = document[PT_FIELD_ROOM][PT_FIELD_MAXMEMBER].GetInt();
    pGameRoom->m_nStartTurn = document[PT_FIELD_ROOM][PT_FIELD_START_POSITION].GetInt();
    pGameRoom->m_nCurTurn = document[PT_FIELD_ROOM][PT_FIELD_CUR_POSITION].GetInt();
    pGameRoom->m_strCreaterId = document[PT_FIELD_ROOM][PT_FIELD_CREATER].GetString();
    
    const rapidjson::Value& players = document[PT_FIELD_ROOM][PT_FIELD_PLAYERS];
    for(rapidjson::Value::ConstValueIterator it = players.Begin(); it != players.End(); ++it)
    {
        const rapidjson::Value& player = *it;
        UserInfo* pPlayer = new UserInfo;
        pPlayer->m_strId = player[PT_FIELD_ID].GetString();
        pPlayer->m_strSession = player[PT_FIELD_SESSION].GetString();
        pPlayer->m_strName = player[PT_FIELD_NAME].GetString();
        pPlayer->m_strAvatar = player[PT_FIELD_AVATAR].GetString();
        pPlayer->m_strAvatarUrl = player[PT_FIELD_AVATAR_URL].GetString();
        pPlayer->m_strDevice = player[PT_FIELD_DEVICE].GetString();
        pPlayer->m_nOnline = player[PT_FIELD_ONLINE].GetInt();
        pPlayer->setLevel(player[PT_FIELD_LEVEL].GetInt());
        pPlayer->m_nWinCount = player[PT_FIELD_WINCNT].GetInt();
        pPlayer->m_nLostCount = player[PT_FIELD_LOSECNT].GetInt();
        pPlayer->m_nCoin = player[PT_FIELD_COIN].GetInt();
        pPlayer->m_strRoomId = player[PT_FIELD_ROOMID].GetString();
        pPlayer->m_nState = player[PT_FIELD_STATE].GetInt();
        pPlayer->m_nHistory = player[PT_FIELD_HISTORY].GetInt();
        pPlayer->m_nHistoryCount = player[PT_FIELD_HISTORYCNT].GetInt();
        pPlayer->m_nJoined = player[PT_FIELD_JOINED].GetInt();
        pPlayer->m_nStartReq = player[PT_FIELD_START_REQ].GetInt();
        pPlayer->m_nInGame = player[PT_FIELD_INGAME].GetInt();
        pPlayer->m_nPosition = player[PT_FIELD_POSITION].GetInt();
        
        const rapidjson::Value& cards = player[PT_FIELD_CARDS];
        for(rapidjson::Value::ConstValueIterator it1 = cards.Begin(); it1 != cards.End(); ++it1)
        {
            const rapidjson::Value& card = *it1;
            int nCardNum = card.GetInt();
            auto cardSprite = CardSprite::create(nCardNum, CARD_NUM[nCardNum], CARD_FLOWER[nCardNum], pPlayer,
                                                 getCardFileName(card.GetInt()), "img/card/card_back.png");
            pPlayer->m_vecCards.pushBack(cardSprite);
        }
        
        pGameRoom->m_vecPlayers.pushBack(pPlayer);
    }
    
    const rapidjson::Value& deckCards = document[PT_FIELD_ROOM][PT_FIELD_DECKCARD];
    for(rapidjson::Value::ConstValueIterator it = deckCards.Begin(); it != deckCards.End(); ++it)
    {
        const rapidjson::Value& deck_card = *it;
        int nCardNum = deck_card.GetInt();
        auto cardSprite = CardSprite::create(nCardNum, CARD_NUM[nCardNum], CARD_FLOWER[nCardNum], NULL,
                                             getCardFileName(deck_card.GetInt()), "img/card/card_back.png");
        pGameRoom->m_vecDeckCards.pushBack(cardSprite);
    }
    
    const rapidjson::Value& remainCards = document[PT_FIELD_ROOM][PT_FIELD_REMAINCARD];
    for(rapidjson::Value::ConstValueIterator it = remainCards.Begin(); it != remainCards.End(); ++it)
    {
        const rapidjson::Value& remain_card = *it;
        int nCardNum = remain_card.GetInt();
        auto cardSprite = CardSprite::create(nCardNum, CARD_NUM[nCardNum], CARD_FLOWER[nCardNum], NULL,
                                             getCardFileName(remain_card.GetInt()), "img/card/card_back.png");
        pGameRoom->m_vecRemainCards.pushBack(cardSprite);
    }
    
    return pGameRoom;
}

UserInfo* PacketParser::parseJoinedPlayer(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    UserInfo* pPlayer = new UserInfo;
    pPlayer->m_strId = document[PT_FIELD_ID].GetString();
    pPlayer->m_strSession = document[PT_FIELD_SESSION].GetString();
    pPlayer->m_strName = document[PT_FIELD_NAME].GetString();
    pPlayer->m_strAvatar = document[PT_FIELD_AVATAR].GetString();
    pPlayer->m_strAvatarUrl = document[PT_FIELD_AVATAR_URL].GetString();
    pPlayer->m_strDevice = document[PT_FIELD_DEVICE].GetString();
    pPlayer->m_nOnline = document[PT_FIELD_ONLINE].GetInt();
    pPlayer->setLevel(document[PT_FIELD_LEVEL].GetInt());
    pPlayer->m_nWinCount = document[PT_FIELD_WINCNT].GetInt();
    pPlayer->m_nLostCount = document[PT_FIELD_LOSECNT].GetInt();
    pPlayer->m_nCoin = document[PT_FIELD_COIN].GetInt();
    pPlayer->m_strRoomId = document[PT_FIELD_ROOMID].GetString();
    pPlayer->m_nState = document[PT_FIELD_STATE].GetInt();
    pPlayer->m_nHistory = document[PT_FIELD_HISTORY].GetInt();
    pPlayer->m_nHistoryCount = document[PT_FIELD_HISTORYCNT].GetInt();
    pPlayer->m_nJoined = document[PT_FIELD_JOINED].GetInt();
    pPlayer->m_nStartReq = document[PT_FIELD_START_REQ].GetInt();
    pPlayer->m_nInGame = document[PT_FIELD_INGAME].GetInt();
    pPlayer->m_nPosition = document[PT_FIELD_POSITION].GetInt();
    
    const rapidjson::Value& cards = document[PT_FIELD_CARDS];
    for(rapidjson::Value::ConstValueIterator it1 = cards.Begin(); it1 != cards.End(); ++it1)
    {
        const rapidjson::Value& card = *it1;
        int nCardValue = card.GetInt();
        auto cardSprite = CardSprite::create(nCardValue, CARD_NUM[nCardValue], CARD_FLOWER[nCardValue], pPlayer,
                                             getCardFileName(card.GetInt()), "img/card/card_back.png");
        pPlayer->m_vecCards.pushBack(cardSprite);
    }
    
    return pPlayer;
}

void PacketParser::parseUpdatedGameRoom(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    if(pGameRoom == NULL || pGameRoom->m_strId != document[PT_FIELD_ID].GetString())
        return;
    
    pGameRoom->m_nType = document[PT_FIELD_TYPE].GetInt();
    pGameRoom->m_nStarted = document[PT_FIELD_STARTED].GetInt();
    pGameRoom->m_nMaxMember = document[PT_FIELD_MAXMEMBER].GetInt();
    pGameRoom->m_nStartTurn = document[PT_FIELD_START_POSITION].GetInt();
    pGameRoom->m_nCurTurn = document[PT_FIELD_CUR_POSITION].GetInt();
    
    const rapidjson::Value& players = document[PT_FIELD_PLAYERS];
    for(rapidjson::Value::ConstValueIterator it = players.Begin(); it != players.End(); ++it)
    {
        const rapidjson::Value& player = *it;
        UserInfo* pPlayer = pGameRoom->findPlayer(player[PT_FIELD_ID].GetString());
        if(pPlayer == NULL)
            continue;
        
        pPlayer->m_strSession = player[PT_FIELD_SESSION].GetString();
        pPlayer->m_strName = player[PT_FIELD_NAME].GetString();
        pPlayer->m_strAvatar = player[PT_FIELD_AVATAR].GetString();
        pPlayer->m_strAvatarUrl = player[PT_FIELD_AVATAR_URL].GetString();
        pPlayer->m_strDevice = player[PT_FIELD_DEVICE].GetString();
        pPlayer->m_nOnline = player[PT_FIELD_ONLINE].GetInt();
        pPlayer->setLevel(player[PT_FIELD_LEVEL].GetInt());
        pPlayer->m_nWinCount = player[PT_FIELD_WINCNT].GetInt();
        pPlayer->m_nLostCount = player[PT_FIELD_LOSECNT].GetInt();
        pPlayer->m_nCoin = player[PT_FIELD_COIN].GetInt();
        pPlayer->m_strRoomId = player[PT_FIELD_ROOMID].GetString();
        pPlayer->m_nState = player[PT_FIELD_STATE].GetInt();
        pPlayer->m_nHistory = player[PT_FIELD_HISTORY].GetInt();
        pPlayer->m_nHistoryCount = player[PT_FIELD_HISTORYCNT].GetInt();
        pPlayer->m_nJoined = player[PT_FIELD_JOINED].GetInt();
        pPlayer->m_nStartReq = player[PT_FIELD_START_REQ].GetInt();
        pPlayer->m_nInGame = player[PT_FIELD_INGAME].GetInt();
        pPlayer->m_nPosition = player[PT_FIELD_POSITION].GetInt();
        pPlayer->m_vecCards.clear();
        
        const rapidjson::Value& cards = player[PT_FIELD_CARDS];
        for(rapidjson::Value::ConstValueIterator it1 = cards.Begin(); it1 != cards.End(); ++it1)
        {
            const rapidjson::Value& card = *it1;
            int nCardValue = card.GetInt();
            auto cardSprite = CardSprite::create(nCardValue, CARD_NUM[nCardValue], CARD_FLOWER[nCardValue], pPlayer,
                                                 getCardFileName(card.GetInt()), "img/card/card_back.png");
            pPlayer->m_vecCards.pushBack(cardSprite);
        }
    }
    
    pGameRoom->m_vecDeckCards.clear();
    pGameRoom->m_vecRemainCards.clear();
    
    const rapidjson::Value& deckCards = document[PT_FIELD_DECKCARD];
    for(rapidjson::Value::ConstValueIterator it = deckCards.Begin(); it != deckCards.End(); ++it)
    {
        const rapidjson::Value& deck_card = *it;
        int nCardValue = deck_card.GetInt();
        auto cardSprite = CardSprite::create(nCardValue, CARD_NUM[nCardValue], CARD_FLOWER[nCardValue], NULL,
                                             getCardFileName(deck_card.GetInt()), "img/card/card_back.png");
        pGameRoom->m_vecDeckCards.pushBack(cardSprite);
    }
    
    const rapidjson::Value& remainCards = document[PT_FIELD_REMAINCARD];
    for(rapidjson::Value::ConstValueIterator it = remainCards.Begin(); it != remainCards.End(); ++it)
    {
        const rapidjson::Value& remain_card = *it;
        int nCardValue = remain_card.GetInt();
        auto cardSprite = CardSprite::create(nCardValue, CARD_NUM[nCardValue], CARD_FLOWER[nCardValue], NULL,
                                             getCardFileName(remain_card.GetInt()), "img/card/card_back.png");
        pGameRoom->m_vecRemainCards.pushBack(cardSprite);
    }
}

LeaveInfo PacketParser::parseLeavePlayer(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    LeaveInfo leaveInfo;
    leaveInfo.strRoomId = document[PT_FIELD_ROOM_ID].GetString();
    leaveInfo.strUserId = document[PT_FIELD_USERID].GetString();
    return leaveInfo;
}

GameResultInfo PacketParser::parseGameResult(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    GameResultInfo gameResultInfo;
    gameResultInfo.strRoomId = document[PT_FIELD_ROOM_ID].GetString();
    gameResultInfo.strWinnerId = document[PT_FIELD_WINNER_ID].GetString();
    gameResultInfo.nWinnerCoin = document[PT_FIELD_WINNER_COIN].GetInt();
    return gameResultInfo;
}

PutCardInfo PacketParser::parsePutCard(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    PutCardInfo putCardInfo;
    putCardInfo.strRoomId = document[PT_FIELD_ROOM_ID].GetString();
    putCardInfo.nPrevTune = document[PT_FIELD_PREVTUNE].GetInt();
    putCardInfo.nCurTune = document[PT_FIELD_CURTUNE].GetInt();
    putCardInfo.nCardFlower = document[PT_FIELD_CARD_FLOWER].GetInt();
    putCardInfo.bDealNewCard = document[PT_FIELD_DEAL_NEWCARD].GetBool();
    putCardInfo.nNewDealCount = document[PT_FIELD_DEAL_CARDCOUNT].GetInt();
    putCardInfo.strPickState = document[PT_FIELD_PICK_STATE].GetString();
    
    const rapidjson::Value& cards = document[PT_FIELD_CARDNUM];
    for(rapidjson::Value::ConstValueIterator it = cards.Begin(); it != cards.End(); ++it)
    {
        const rapidjson::Value& card = *it;
        putCardInfo.vecCardValues.push_back(card.GetInt());
    }
    return putCardInfo;
}

Vector<UserInfo*> PacketParser::parseUserList(std::string data)
{
    Vector<UserInfo*> vecResult;
    
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    const rapidjson::Value& players = document[PT_FIELD_PLAYERS];
    for(rapidjson::Value::ConstValueIterator it = players.Begin(); it != players.End(); ++it)
    {
        const rapidjson::Value& player = *it;
        
        UserInfo* pPlayer = new UserInfo;
        pPlayer->m_strId = player[PT_FIELD_ID].GetString();
        pPlayer->m_strSession = player[PT_FIELD_SESSION].GetString();
        pPlayer->m_strName = player[PT_FIELD_NAME].GetString();
        pPlayer->m_strAvatar = player[PT_FIELD_AVATAR].GetString();
        pPlayer->m_strAvatarUrl = player[PT_FIELD_AVATAR_URL].GetString();
        pPlayer->m_strDevice = player[PT_FIELD_DEVICE].GetString();
        pPlayer->m_nOnline = player[PT_FIELD_ONLINE].GetInt();
        pPlayer->setLevel(player[PT_FIELD_LEVEL].GetInt());
        pPlayer->m_nWinCount = player[PT_FIELD_WINCNT].GetInt();
        pPlayer->m_nLostCount = player[PT_FIELD_LOSECNT].GetInt();
        pPlayer->m_nCoin = player[PT_FIELD_COIN].GetInt();
        pPlayer->m_strRoomId = player[PT_FIELD_ROOMID].GetString();
        pPlayer->m_nState = player[PT_FIELD_STATE].GetInt();
        pPlayer->m_nHistory = player[PT_FIELD_HISTORY].GetInt();
        pPlayer->m_nHistoryCount = player[PT_FIELD_HISTORYCNT].GetInt();
        
        const rapidjson::Value& friends = player[PT_FIELD_FRIENDS];
        for(rapidjson::Value::ConstValueIterator it1 = friends.Begin(); it1 != friends.End(); ++it1)
        {
            const rapidjson::Value& friendId = *it1;
            std::string strFriendId = friendId[PT_FIELD_ID].GetString();
            pPlayer->m_vecFriends.push_back(strFriendId);
        }
        
        vecResult.pushBack(pPlayer);
    }
    
    return vecResult;
}

void PacketParser::parseNotificationList(std::string data)
{
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    const rapidjson::Value& notifications = document[PT_FIELD_NOTIFICATIONS];
    
    for(rapidjson::Value::ConstValueIterator it = notifications.Begin(); it != notifications.End(); ++it)
    {
        const rapidjson::Value& notification = *it;
        
        std::string strId = notification[PT_FIELD_ID].GetString();
        int nType = notification[PT_FIELD_TYPE].GetInt();
        std::string strSenderId = notification[PT_FIELD_SENDER_ID].GetString();
        std::string strSenderName = notification[PT_FIELD_SENDER_NAME].GetString();
        int nSenderLevel = notification[PT_FIELD_SENDER_LEVEL].GetInt();
        int nSenderOnline = notification[PT_FIELD_SENDER_ONLINE].GetInt();
        std::string strReceiverId = notification[PT_FIELD_RECEIVER_ID].GetString();
        int nReceiverAction = notification[PT_FIELD_RECEIVER_ACTION].GetInt();
        
        Notification* pNotification = pNotificationParser->getNotification(strId);
        if(pNotification == nullptr)
        {
            pNotification = new Notification();
            pNotification->m_strId = strId;
            pNotification->m_nType = nType;
            pNotification->m_strSenderId = strSenderId;
            pNotification->m_strSenderName = strSenderName;
            pNotification->m_nSenderLevel = nSenderLevel;
            pNotification->m_nSenderOnline = nSenderOnline;
            pNotification->m_strReceiverId = strReceiverId;
            pNotification->m_nReceiverAction = nReceiverAction;
            pNotificationParser->m_vecNotifications.pushBack(pNotification);
        }
        else
        {
            bool bNew = false;
            
            if(pNotification->m_nType != nType)
            {
                bNew = true;
                pNotification->m_nType = nType;
            }
            
            if(pNotification->m_nReceiverAction != nReceiverAction)
            {
                bNew = true;
                pNotification->m_nReceiverAction = nReceiverAction;
            }
            
            pNotification->m_strSenderId = strSenderId;
            pNotification->m_strSenderName = strSenderName;
            pNotification->m_nSenderLevel = nSenderLevel;
            pNotification->m_nSenderOnline = nSenderOnline;
            pNotification->m_strReceiverId = strReceiverId;
            if(bNew)
                pNotification->m_bNew = bNew;
        }
    }
    
    GameData::getInstance()->saveNotifications();
}

void PacketParser::parseNotifyNotification(std::string data)
{
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    const rapidjson::Value& notification = document[PT_FIELD_NOTIFICATION];
    
    std::string strId = notification[PT_FIELD_ID].GetString();
    int nType = notification[PT_FIELD_TYPE].GetInt();
    std::string strSenderId = notification[PT_FIELD_SENDER_ID].GetString();
    std::string strSenderName = notification[PT_FIELD_SENDER_NAME].GetString();
    int nSenderLevel = notification[PT_FIELD_SENDER_LEVEL].GetInt();
    int nSenderOnline = notification[PT_FIELD_SENDER_ONLINE].GetInt();
    std::string strReceiverId = notification[PT_FIELD_RECEIVER_ID].GetString();
    int nReceiverAction = notification[PT_FIELD_RECEIVER_ACTION].GetInt();
    
    Notification* pNotification = pNotificationParser->getNotification(strId);
    if(pNotification == nullptr)
    {
        pNotification = new Notification();
        pNotification->m_strId = strId;
        pNotification->m_nType = nType;
        pNotification->m_strSenderId = strSenderId;
        pNotification->m_strSenderName = strSenderName;
        pNotification->m_nSenderLevel = nSenderLevel;
        pNotification->m_nSenderOnline = nSenderOnline;
        pNotification->m_strReceiverId = strReceiverId;
        pNotification->m_nReceiverAction = nReceiverAction;
        pNotificationParser->m_vecNotifications.pushBack(pNotification);
    }
    else
    {
        bool bNew = false;
        
        if(pNotification->m_nType != nType)
        {
            bNew = true;
            pNotification->m_nType = nType;
        }
        
        if(pNotification->m_nReceiverAction != nReceiverAction)
        {
            bNew = true;
            pNotification->m_nReceiverAction = nReceiverAction;
        }
        
        pNotification->m_strSenderId = strSenderId;
        pNotification->m_strSenderName = strSenderName;
        pNotification->m_nSenderLevel = nSenderLevel;
        pNotification->m_nSenderOnline = nSenderOnline;
        pNotification->m_strReceiverId = strReceiverId;
        if(bNew)
            pNotification->m_bNew = bNew;
    }
    
    GameData::getInstance()->saveNotifications();
}

MailActionInfo PacketParser::parseMailAction(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    MailActionInfo mailActionInfo;
    mailActionInfo.nStatus = document[PT_FIELD_SUCCESS].GetInt();
    mailActionInfo.strMailId = document[PT_FIELD_ID].GetString();
    mailActionInfo.nAction = document[PT_FIELD_ACTION].GetInt();
    return mailActionInfo;
}

RemoveFriendResult PacketParser::parseRemoveFriendResult(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    RemoveFriendResult removeFriendResult;
    removeFriendResult.nStatus = document[PT_FIELD_SUCCESS].GetInt();
    removeFriendResult.strFriendId = document[PT_FIELD_FRIEND_ID].GetString();
    return removeFriendResult;
}

FriendMatchReqResult PacketParser::parseFriendMatchReq(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    FriendMatchReqResult result;
    result.nStatus = document[PT_FIELD_SUCCESS].GetInt();
    result.nAction = document[PT_FIELD_ACTION].GetInt();
    return result;
}

NotifyFriendMatch PacketParser::parseNotifyFriendMatch(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    NotifyFriendMatch result;
    result.nAction = document[PT_FIELD_ACTION].GetInt();
    result.strFriendId = document[PT_FIELD_FRIEND_ID].GetString();
    result.strFriendName = document[PT_FIELD_FRIEND_NAME].GetString();
    result.nGameCoin = document[PT_FIELD_COIN].GetInt();
    return result;
}

bool PacketParser::parseCheckNameResponse(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    
    if(!document.HasMember(PT_FIELD_SUCCESS))
        return false;
    
    if(document[PT_FIELD_SUCCESS].GetInt() == RES_FAILED)
        return false;
    else
        return true;
}

std::string PacketParser::parseKnockResponse(std::string data)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
    return document[PT_FIELD_ROOM_ID].GetString();
}
