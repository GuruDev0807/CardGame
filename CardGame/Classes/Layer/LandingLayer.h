#ifndef __CARDGAME_LANDING_LAYER_H__
#define __CARDGAME_LANDING_LAYER_H__

#include "BaseLayer.h"

class LandingLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~LandingLayer();
    virtual bool init()override;
    
    void completedLanding();
    
public:
    CREATE_FUNC(LandingLayer);
};

#endif /* __CARDGAME_LANDING_LAYER_H__ */
