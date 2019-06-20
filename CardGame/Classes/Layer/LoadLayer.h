#ifndef __CARDGAME_LOAD_LAYER_H__
#define __CARDGAME_LOAD_LAYER_H__

#include "BaseLayer.h"

class LoadLayer : public BaseLayer
{
public:
    virtual ~LoadLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    
    bool onTouchBegan(Touch *touch, Event *event)override;
    void onTouchMoved(Touch *touch, Event *event)override;
    void onTouchEnded(Touch *touch, Event *event)override;
    void onTouchCancelled(Touch *touch, Event *event)override;
    
private:
    EventListenerTouchOneByOne* m_pTouchListener;
    
public:
    CREATE_FUNC(LoadLayer);
};

#endif //__CARDGAME_LOAD_LAYER_H__
