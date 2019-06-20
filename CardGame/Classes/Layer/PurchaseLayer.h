#ifndef __CARDGAME_PURCHASE_LAYER_H__
#define __CARDGAME_PURCHASE_LAYER_H__

#include "BaseLayer.h"

class PurchaseLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~PurchaseLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    void OnBack(Ref* pSender);
    void OnPurchase500(Ref* pSender);
    void OnPurchase1250(Ref* pSender);
    void OnPurchase2500(Ref* pSender);
    void OnPurchase5000(Ref* pSender);
    
    void updateCoin(int plus_coin);
    
private:
    void createSprites();
    void createMenu();
    
public:
    CREATE_FUNC(PurchaseLayer);
};

#endif // __CARDGAME_PURCHASE_LAYER_H__
