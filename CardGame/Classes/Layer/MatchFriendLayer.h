#ifndef __CARDGAME_MATCH_FRIEND_LAYER_H__
#define __CARDGAME_MATCH_FRIEND_LAYER_H__

#include "BaseLayer.h"
#include "../Data/UserInfo.h"

class MatchFriendLayer : public BaseLayer
{
public:
    static Scene* createScene(Vector<UserInfo*> vecFriends, int nGameCoin);
    
    virtual ~MatchFriendLayer();
    virtual bool init()override;
    void init(Vector<UserInfo*> vecFriends, int nGameCoin);
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    void OnBack(Ref* pSender);
    
    void updateStatus(float dt);
    void close();
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    
private:
    Vector<UserInfo*> m_vecFriends;
    int m_nTimeIndex;
    int m_nGameCoin;
    
public:
    CREATE_FUNC(MatchFriendLayer);
};

#endif // __CARDGAME_MATCH_FRIEND_LAYER_H__
