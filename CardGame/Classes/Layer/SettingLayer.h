#ifndef __CARDGAME_SETTING_LAYER_H__
#define __CARDGAME_SETTING_LAYER_H__

#include "BaseLayer.h"

class SettingLayer : public BaseLayer, public cocos2d::IMEDelegate
{
public:
    static Scene* createScene();
    
    virtual ~SettingLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info)override;
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& info) override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    void OnBack(Ref* pSender);
    void OnSoundOn(Ref* pSender);
    void OnSoundOff(Ref* pSender);
    void OnFriendRequest(Ref* pSender);
    void OnProfileUpdate(Ref* pSender);
    void OnProfileEdit(Ref* pSender);
    void OnNameCheck(Ref* pSender);
    void OnHelp(Ref* pSender);
    
    void updateProfile(std::string file_name);
    void uploadUserInfo(std::string avatarName);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    Layer* createAvatarLayer(std::string fileName);
    void updateMenu();
    void onClickTrackNode(bool bClicked, const cocos2d::Vec2& touchPos);
    
private:
    cocos2d::Vec2   m_posTouch;
    float           m_fAdjustVert;
    bool            m_bChangedAvatar;
    
public:
    CREATE_FUNC(SettingLayer);
};

#endif // __CARDGAME_SETTING_LAYER_H__
