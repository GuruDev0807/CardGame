#ifndef __CARDGAME_HELP_LAYER_H__
#define __CARDGAME_HELP_LAYER_H__

#include "BaseLayer.h"

class HelpLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~HelpLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    
    void OnBack(Ref* pSender);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    
public:
    CREATE_FUNC(HelpLayer);
};

#endif // __CARDGAME_HELP_LAYER_H__
