#ifndef __CARDGAME_FRIENDREQ_LAYER_H__
#define __CARDGAME_FRIENDREQ_LAYER_H__

#include "BaseLayer.h"

class UserInfo;

class FriendReqLayer : public BaseLayer, public cocos2d::IMEDelegate
{
public:
    static Scene* createScene();
    
    virtual ~FriendReqLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info)override;
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& info) override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    virtual void onDownloadedAvatar(std::string file_name) override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    void OnBack(Ref* pSender);
    void OnSend(Ref* pSender);
    void OnSearch(Ref* pSender);
    void OnCheck(Ref* pSender);
    void OnUnCheck(Ref* pSender);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    void onClickTrackNode(bool bClicked, const cocos2d::Vec2& touchPos);
    void updateScrollView();
    void createOneUser(Sprite* pSprBg, UserInfo* pUser, int tag);
    void createAvatarLayer(Sprite* pParentSpr, std::string fileName, std::string downloadLink);
    
private:
    cocos2d::Vec2       m_posTouch;
    float               m_fAdjustVert;
    Vector<UserInfo*>   m_vecUsers;
    
public:
    CREATE_FUNC(FriendReqLayer);
};

#endif // __CARDGAME_FRIENDREQ_LAYER_H__
