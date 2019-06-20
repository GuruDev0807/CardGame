#ifndef __CARDGAME_LOBBY_LAYER_H__
#define __CARDGAME_LOBBY_LAYER_H__

#include "BaseLayer.h"

class UserInfo;

class LobbyLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~LobbyLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void onNetClientConnected()override;
    virtual void onNetClientError(std::string error) override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    void    OnPlay1(Ref* pSender);
    void    OnPlay6(Ref* pSender);
    void    OnPlayFriend(Ref* pSender);
    void    OnSetting(Ref* pSender);
    void    OnFriendList(Ref* pSender);
    void    OnNotificationList(Ref* pSender);
    void    OnAddCoin(Ref* pSender);
    void    OnFreeCoins(Ref* pSender);
    
    void    updateStatus(float dt);
    void    startConnecting();
    void    loadUserInfo();
    
private:
    void    createBackground();
    Layer*  createAvatarLayer(std::string fileName);
    void    createLabel();
    void    createMenu();
    void    showMainMenu();
    void    updateUserInfo();
    
private:
    int     m_nStep;
    int     m_nTimeIndex;
    
public:
    CREATE_FUNC(LobbyLayer);
};

#endif // __CARDGAME_LOBBY_LAYER_H__


