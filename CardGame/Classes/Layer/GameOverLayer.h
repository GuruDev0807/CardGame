
#ifndef __CARDGAME_GAMEOVER_LAYER_H__
#define __CARDGAME_GAMEOVER_LAYER_H__

#include "BaseLayer.h"

class GameOverLayer : public BaseLayer
{
public:
    virtual ~GameOverLayer();
    virtual bool init()override;
    bool init(bool bWinner);
    virtual void onEnter()override;
    
    bool onTouchBegan(Touch *touch, Event *event)override;
    void onTouchMoved(Touch *touch, Event *event)override;
    void onTouchEnded(Touch *touch, Event *event)override;
    void onTouchCancelled(Touch *touch, Event *event)override;
    
    void OnCancel(Ref* pSender);
    void OnTryAgain(Ref* pSender);
    
private:
    void createBackground(bool bWinner);
    void createMenu();
    
private:
    EventListenerTouchOneByOne* m_pTouchListener;
    
public:
    CREATE_FUNC(GameOverLayer);
};

#endif //__CARDGAME_GAMEOVER_LAYER_H__
