#ifndef __CARDGAME_FRIEND_LAYER_H__
#define __CARDGAME_FRIEND_LAYER_H__

#include "BaseLayer.h"

enum FRIEND_SHOW_TYPE
{
    FST_PLAY = 1,
    FST_REMOVE = 2,
};

class UserInfo;

class FriendLayer : public BaseLayer
{
public:
    static Scene* createScene(int nShowType);
    
    virtual ~FriendLayer();
    virtual bool init()override;
    void init(int nShowType);
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    virtual void onDownloadedAvatar(std::string file_name) override;
    
    void OnBack(Ref* pSender);
    void OnRemove(Ref* pSender);
    void OnPlay(Ref* pSender);
    void OnCheck(Ref* pSender);
    void OnUnCheck(Ref* pSender);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    void updateScrollView();
    void createOneFriend(Sprite* pSprBg, UserInfo* pUser, int tag);
    void createAvatarLayer(Sprite* pParentSpr, std::string fileName, std::string downloadLink);
    void removeFriend(std::string strFriendId);
    void updateMainMenu();
    
private:
    int                 m_nShowType;
    Vector<UserInfo*>   m_vecFriends;
    
public:
    CREATE_FUNC(FriendLayer);
};

#endif // __CARDGAME_FRIEND_LAYER_H__
