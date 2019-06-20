#ifndef __CARDGAME_CARD_SPRITE_H__
#define __CARDGAME_CARD_SPRITE_H__

#include "../Common/Common.h"

class UserInfo;

class CardSprite : public Sprite
{
public:
    static CardSprite* create(const int nValue, const int nCardNum, const int nCardFlower, UserInfo* pPlayer, const std::string& frontFileName, const std::string& backFileName);
    
    bool onTouchBegan(Touch *touch, Event *event);
    void onTouchMoved(Touch *touch, Event *event);
    void onTouchEnded(Touch *touch, Event *event);
    void onTouchCancelled(Touch *touch, Event *event);
    
    void showFrontSprite();
    void hideFrontSprite();
    void showBackSprite();
    void hideBackSprite();
    void setPlayer(UserInfo* pPlayer);
    void setActive(bool bActive);
    bool isActive();
    int getCardValue();
    int getCardNum();
    int getCardFlower();
    
private:
    CardSprite();
    void init(const int nValue, const int nCardNum, const int nCardFlower, UserInfo* pPlayer, const std::string& frontFileName, const std::string& backFileName);
    
    void updatePosition();
    
private:
    Sprite*     m_pFrontSprite;
    Sprite*     m_pBackSprite;
    Sprite*     m_pOverlaySprite;
    
    int         m_nValue;
    int         m_nCardNum;
    int         m_nCardFlower;
    bool        m_bActived;
    
    UserInfo*   m_pPlayer;
    
    EventListenerTouchOneByOne* m_pTouchListener;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(CardSprite);
};

#endif /* __CARDGAME_CARD_SPRITE_H__ */

