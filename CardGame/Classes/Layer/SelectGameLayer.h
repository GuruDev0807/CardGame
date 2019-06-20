#ifndef __CARDGAME_SELECTGAME_LAYER_H__
#define __CARDGAME_SELECTGAME_LAYER_H__

#include "BaseLayer.h"
#include "../Data/UserInfo.h"

class SelectGameLayer : public BaseLayer
{
public:
    static Scene* createScene(int nRoomType, Vector<UserInfo*> vecFriends);
    
    virtual ~SelectGameLayer();
    virtual bool init()override;
    void init(int nRoomType, Vector<UserInfo*> vecFriends);
    virtual void onEnter()override;
    
    void OnBack(Ref* pSender);
    void OnGame50(Ref* pSender);
    void OnGame200(Ref* pSender);
    void OnGame500(Ref* pSender);
    void OnGame1000(Ref* pSender);
    
    void showPurchase();
    
private:
    void createSprites();
    void createMenu();
    
private:
    int m_nRoomType;
    Vector<UserInfo*> m_vecFriends;
    
public:
    CREATE_FUNC(SelectGameLayer);
};

#endif // __CARDGAME_SELECTGAME_LAYER_H__
