#ifndef __CARDGAME_MESSAGE_LAYER_H__
#define __CARDGAME_MESSAGE_LAYER_H__

#include "BaseLayer.h"

enum MESSAGE_TYPE
{
    MSG_CONNECT_FAILED = 0,
    MSG_USERINFO_FAILED = 1,
    MSG_REMOVE_FRIEND_FAILED = 2,
    MSG_FRIEND_MATCH_FAILED = 3,
    MSG_NOTIFY_FRIEND_MATCH = 4,
    MSG_FRIEND_MATCH_DECLINE = 5,
    MSG_NOT_ENOUGH_COIN = 6,
    MSG_NOT_ENOUGH_COIN_FRIEND = 7,
    MSG_FAILED_UPLOAD_AVATAR = 8,
    MSG_FAILED_UPDATE_PROFILE = 9,
    MSG_SUCCESS_CHECK_NAME = 10,
    MSG_FAILED_CHECK_NAME = 11,
    MSG_MAX_FRIENDS = 12,
    MSG_MAX,
};

class MessageLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~MessageLayer();
    virtual bool init()override;
    bool init(int message_type, std::string param1, std::string param2, int param3);
    virtual void onEnter()override;
    
    bool onTouchBegan(Touch *touch, Event *event)override;
    void onTouchMoved(Touch *touch, Event *event)override;
    void onTouchEnded(Touch *touch, Event *event)override;
    void onTouchCancelled(Touch *touch, Event *event)override;

    void OnAction(Ref* pSender);
    void OnCancel(Ref* pSender);
    
private:
    void createBackground();
    void createMenu();
    
private:
    int m_nMessageType;
    std::string m_strParam1;
    std::string m_strParam2;
    int m_nParam3;
    EventListenerTouchOneByOne* m_pTouchListener;
    
public:
    CREATE_FUNC(MessageLayer);
};

#endif // __CARDGAME_MESSAGE_LAYER_H__
