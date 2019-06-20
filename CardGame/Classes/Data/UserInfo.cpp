
#include "UserInfo.h"
#include "../Widget/CardSprite.h"

UserInfo::UserInfo()
{
    m_strId = "";
    m_strSession = "";
    m_strName = "";
    m_strAvatar = "";
    m_strDevice = "";
    m_nOnline = PLAYER_OFFLINE;
    m_nLevel = DEFAULT_LEVEL;
    m_nWinCount = 0;
    m_nLostCount = 0;
    m_nCoin = 0;
    m_strRoomId = "";
    m_nState = STATE_LOBBY;
    m_nHistory = 0;
    m_nHistoryCount = 0;
    m_nJoined = 0;
    m_nStartReq = 0;
    m_nInGame = 0;
    m_nPosition = -1;
    m_bChecked = false;
    m_vecCards.clear();
    m_vecActiveCards.clear();
    m_vecFriends.clear();
}

UserInfo::~UserInfo()
{
    m_vecCards.clear();
    m_vecActiveCards.clear();
    m_vecFriends.clear();
}

CardSprite* UserInfo::findCard(int nCardValue)
{
    for(Vector<CardSprite*>::iterator it = m_vecCards.begin(); it != m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->getCardValue() == nCardValue)
            return pCardSprite;
    }
    
    return NULL;
}

bool UserInfo::isAllAce()
{
    for(int index = 0; index < m_vecActiveCards.size(); index++)
    {
        CardSprite* pCardSprite = m_vecActiveCards.at(index);
        if(pCardSprite->getCardNum() != CARD_1)
            return false;
    }
    
    return true;
}

bool UserInfo::isAll2()
{
    for(int index = 0; index < m_vecActiveCards.size(); index++)
    {
        CardSprite* pCardSprite = m_vecActiveCards.at(index);
        if(pCardSprite->getCardNum() != CARD_2)
            return false;
    }
    
    return true;
}

bool UserInfo::isFriend(std::string strFriendId)
{
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        std::string strFriend = m_vecFriends.at(index);
        if(strFriend == strFriendId)
            return true;
    }
    return false;
}

void UserInfo::setLevel(int nLevel)
{
    m_nLevel = nLevel;
}

int  UserInfo::getDisplayLevel()
{
    return m_nLevel / 100 + 1;
}

void UserInfo::levelup()
{
    m_nLevel += LEVELUP_PER_MATCH;
}
