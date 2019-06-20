#ifndef __CARDGAME_GAME_LAYER_H__
#define __CARDGAME_GAME_LAYER_H__

#include "BaseLayer.h"
#include "../Widget/PlayerSprite.h"
#include "../Data/Candidate.h"

class UserInfo;
class CardSprite;
class GameOverLayer;

class GameLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~GameLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    virtual void onDownloadedAvatar(std::string file_name) override;
    
    void OnClose(Ref* pSender);
    void OnPickup(Ref* pSender);
    void OnPickup5(Ref* pSender);
    void OnPickup10(Ref* pSender);
    void OnPickup2(Ref* pSender);
    void OnPickup4(Ref* pSender);
    void OnPickup6(Ref* pSender);
    void OnPickup8(Ref* pSender);
    void OnPlace(Ref* pSender);
    void OnDiamond(Ref* pSender);
    void OnHeart(Ref* pSender);
    void OnClub(Ref* pSender);
    void OnSpade(Ref* pSender);
    void OnStartMatch(Ref* pSender);
    
    void DoneDealOneCard(UserInfo* pPlayer, CardSprite* pSprite);
    void DoneDealLastCard(UserInfo* pPlayer, CardSprite* pSprite);
    void DoneFlipDeckCard(CardSprite* pSender);
    void DoneDealNewCard(UserInfo* pPlayer, CardSprite* pSprite, int nCompleteTurn, int nCurTurn);
    void DoneDealJackCard(UserInfo* pPlayer, CardSprite* pSprite, int nCompleteIndex, int nCurIndex, int nTurn);
    void DonePlaceCard(UserInfo* pPlayer, CardSprite* pSprite, int nNextTurn);
    
    void timeoutPlayer(UserInfo* pPlayer);
    bool isTimeoutPlayer(UserInfo* pPlayer);
    void chooseCard(UserInfo* pPlayer);
    void restartGame();
    void closeGame();
    void showItemMenu();
    void hideItemMenu();
    void putCardWithFlower(int flower);
    
private:
    void    createSprites();
    void    createLabels();
    void    createMenu();
    
    void    startGame();
    void    initCards();
    void    drawCards();
    void    dealCards();
    void    expandCards();
    void    flipDeckCard();
    void    repositionCards(UserInfo* pPlayer, bool bAnim = false);
    void    updateTurn(UserInfo* pPrevPlayer);
    
    void            addPlayerSprites();
    void            addPlayerSprite(UserInfo* pPlayer);
    void            removePlayerSprite(UserInfo* pPlayer);
    PlayerSprite*   findPlayerSprite(UserInfo* pPlayer);
    void            updatePlayerStates();
    void            updatePlayerSprites();
    
    bool    match1(CardSprite* pFirst, CardSprite* pSecond, bool bCheckCard1, bool bCheckCard2);
    bool    match2(CardSprite* pFirst, CardSprite* pSecond);
    
    bool    check1(Move* pMove1, Move* pMove2, bool bCheckCard1, bool bCheckCard2);
    bool    check2(Move* pMove1, Move* pMove2);
    
    void    processGameResult();
    void    showGameOver(bool bWinner);
    void    hideGameOver();
    void    showActiveFlower();
    void    permute(Vector<Move*> vecMoves, int nStartIndex, int nEndIndex);
    bool    hasKnockCandidate(UserInfo* pPlayer);
    
private:
    CardSprite*             m_pActiveCard;
    GameOverLayer*          m_pGameOverLayer;
    Vector<PlayerSprite*>   m_vecPlayerSprites;
    GameResultInfo          m_GameResultInfo;
    Vector<Candidate*>      m_vecCandidates;
    
    bool            m_bStartedGame;
    bool            m_bProcessingCards;
    int             m_nActiveFlower;
    int             m_nCard2Turn;
    std::string     m_strPickState;
    
public:
    CREATE_FUNC(GameLayer);
};

#endif // __CARDGAME_GAME_LAYER_H__
