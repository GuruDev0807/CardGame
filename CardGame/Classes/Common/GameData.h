#ifndef __CARDGAME_GAMEDATA_H__
#define __CARDGAME_GAMEDATA_H__

#include "Common.h"

class NetClient;
class UserInfo;
class GameRoom;
class NotificationParser;

class GameData
{
public:
    static GameData*    getInstance();
    
    void        setNetClient(NetClient* pNetClient);
    NetClient*  getNetClient();
    void        initNetClient();
    void        sendPacket(std::string packet_name, std::string packet_body);
    
    void        setUserInfo(UserInfo* pUserInfo);
    UserInfo*   getUserInfo();
    
    void        setGameRoom(GameRoom* pGameRoom);
    GameRoom*   getGameRoom();
    
    NotificationParser* getNotificationParser();
    void        saveNotifications();
    
    bool        getBgmEnable();
    void        setBgmEnable(bool enable);
    bool        getEffectEnable();
    void        setEffectEnable(bool enable);
    bool        isConnected();
    void        setConnected(bool value);
    int         getFriendCount();
    bool        hasNewFriend();
    void        clearNewFriend();
    bool        isFreeCoin();
    void        updateDialyTime();
    
protected:
    GameData();
    ~GameData();
    
private:
    void init();
    bool loadNotificationsFromPLIST();
    
private:
    static GameData*    _instance;
    
    NetClient*          m_pNetClient;
    UserInfo*           m_pUserInfo;
    GameRoom*           m_pGameRoom;
    NotificationParser* m_pNotificationParser;
    
    bool                m_bBgmEnable;
    bool                m_bEffectEnable;
    bool                m_bConnected;
    int                 m_nFriendCount;
    int                 m_nPrevFriendCount;
    int                 m_nLastDialyTime;
};

#endif /* __CARDGAME_GAMEDATA_H__ */
