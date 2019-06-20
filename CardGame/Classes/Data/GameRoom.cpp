
#include "GameRoom.h"
#include "UserInfo.h"
#include "../Widget/CardSprite.h"

GameRoom::GameRoom()
{
    m_strId = "";
    m_nType = ROOM_TYPE_1;
    m_nGameCoin = 0;
    m_nStarted = 0;
    m_nMaxMember = 0;
    m_nStartTurn = -1;
    m_nCurTurn = -1;
    m_vecPlayers.clear();
    m_vecDeckCards.clear();
    m_vecRemainCards.clear();
}

GameRoom::~GameRoom()
{
/*    for(Vector<UserInfo*>::iterator it = m_vecPlayers.begin(); it != m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        delete pPlayer;
        pPlayer = NULL;
    }*/

    m_vecPlayers.clear();
    m_vecDeckCards.clear();
    m_vecRemainCards.clear();
}

UserInfo* GameRoom::findPlayer(std::string strId)
{
    for(Vector<UserInfo*>::iterator it = m_vecPlayers.begin(); it != m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_strId == strId)
            return pPlayer;
    }
    return NULL;
}

UserInfo* GameRoom::findTurnPlayer(int nTurn)
{
    for(Vector<UserInfo*>::iterator it = m_vecPlayers.begin(); it != m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_nPosition == nTurn && pPlayer->m_nInGame == 1)
            return pPlayer;
    }
    
    return NULL;
}

int GameRoom::getInGamePlayerCount()
{
    int nCount = 0;
    for(Vector<UserInfo*>::iterator it = m_vecPlayers.begin(); it != m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_nInGame == 1)
            nCount++;
    }
    return nCount;
}

int GameRoom::getDeltaTurnFromStart(int nTurn)
{
    int nStartTurn = m_nStartTurn;
    int deltaTurn = 0;
    
    while(true)
    {
        if(nStartTurn == nTurn)
            break;
        
        nStartTurn++;
        if(isPosInGame(nStartTurn))
            deltaTurn++;
        
        if(nStartTurn == m_nMaxMember)
            nStartTurn = 0;
    }
    
    return deltaTurn;
}

bool GameRoom::isPosInGame(int nTurn)
{
    for(Vector<UserInfo*>::iterator it = m_vecPlayers.begin(); it != m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_nPosition == nTurn && pPlayer->m_nInGame == 1)
            return true;
    }
    return false;
}
