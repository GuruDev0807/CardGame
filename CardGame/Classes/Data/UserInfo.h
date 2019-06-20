#ifndef __CARDGAME_USERINFO_H__
#define __CARDGAME_USERINFO_H__

#include "../Common/Common.h"

class CardSprite;

class UserInfo : public Ref
{
public:
    UserInfo();
    ~UserInfo();
    
    CardSprite* findCard(int nCardValue);
    
    bool isAllAce();
    bool isAll2();
    bool isFriend(std::string strFriendId);
    
    void setLevel(int nLevel);
    int  getDisplayLevel();
    void levelup();
    
public:
    std::string             m_strId;
    std::string             m_strSession;
    std::string             m_strName;
    std::string             m_strAvatar;
    std::string             m_strAvatarUrl;
    std::string             m_strDevice;
    int                     m_nOnline;
    int                     m_nWinCount;
    int                     m_nLostCount;
    int                     m_nCoin;
    std::string             m_strRoomId;
    int                     m_nState;
    int                     m_nHistory;
    int                     m_nHistoryCount;
    int                     m_nJoined;
    int                     m_nStartReq;
    int                     m_nInGame;
    int                     m_nPosition;
    bool                    m_bChecked;
    Vector<CardSprite*>     m_vecCards;
    Vector<CardSprite*>     m_vecActiveCards;
    std::vector<std::string> m_vecFriends;
    
private:
    int                     m_nLevel;
};

#endif /* __CARDGAME_USERINFO_H__ */
