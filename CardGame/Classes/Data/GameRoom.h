#ifndef __CARDGAME_GAMEROOM_H__
#define __CARDGAME_GAMEROOM_H__

#include "../Common/Common.h"

class UserInfo;
class CardSprite;

class GameRoom : public Ref
{
public:
    GameRoom();
    ~GameRoom();
    
    UserInfo* findPlayer(std::string strId);
    UserInfo* findTurnPlayer(int nTurn);
    int getInGamePlayerCount();
    int getDeltaTurnFromStart(int nTurn);
    bool isPosInGame(int nTurn);
    
public:
    std::string             m_strId;
    std::string             m_strCreaterId;
    int                     m_nType;
    int                     m_nGameCoin;
    int                     m_nStarted;
    int                     m_nMaxMember;
    int                     m_nStartTurn;
    int                     m_nCurTurn;
    Vector<UserInfo*>       m_vecPlayers;
    Vector<CardSprite*>     m_vecDeckCards;
    Vector<CardSprite*>     m_vecRemainCards;
};

#endif /* __CARDGAME_GAMEROOM_H__ */
