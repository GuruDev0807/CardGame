#ifndef __CARDGAME_NOTIFICATION_LAYER_H__
#define __CARDGAME_NOTIFICATION_LAYER_H__

#include "BaseLayer.h"

class Notification;

class NotificationLayer : public BaseLayer
{
public:
    static Scene* createScene();
    
    virtual ~NotificationLayer();
    virtual bool init()override;
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    void OnBack(Ref* pSender);
    void OnDecline(Ref* pSender);
    void OnAccept(Ref* pSender);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    void loadNotifications();
    void createOneNotification(Sprite* pSprBg, Notification* pNotification, int tag);
    void updateNotification(std::string strNotificationId, int nAction);
    
public:
    CREATE_FUNC(NotificationLayer);
};

#endif // __CARDGAME_NOTIFICATION_LAYER_H__
