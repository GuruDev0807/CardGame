#ifndef __CARDGAME_NET_CLIENT_H__
#define __CARDGAME_NET_CLIENT_H__

#include "../Common/Common.h"
#include "network/SocketIO.h"

class NetClient : public SocketIO::SIODelegate
{
public:
    NetClient();
    virtual ~NetClient();
    
    virtual void onConnect(SIOClient* client)override;
    virtual void onClose(SIOClient* client)override;
    virtual void onError(SIOClient* client, const std::string& data)override;
    
    void SIOClientConnect();
    void sendPacket(std::string packet_name, std::string packet_body);
    
    void ResponseUserInfo(SIOClient *client, const std::string& data);
    void ResponseMatch(SIOClient *client, const std::string& data);
    void ResponseJoinRoom(SIOClient *client, const std::string& data);
    void ResponseNotifyJoinPlayer(SIOClient *client, const std::string& data);
    void ResponseNotifyStartGame(SIOClient *client, const std::string& data);
    void ResponseNotifyLeavePlayer(SIOClient *client, const std::string& data);
    void ResponseNotifyPutCard(SIOClient *client, const std::string& data);
    void ResponseNotifyGameResult(SIOClient *client, const std::string& data);
    void ResponseUpdateProfile(SIOClient *client, const std::string& data);
    void ResponseUserList(SIOClient *client, const std::string& data);
    void ResponseAddFriend(SIOClient *client, const std::string& data);
    void ResponseAcceptFriend(SIOClient *client, const std::string& data);
    void ResponseRemoveFriend(SIOClient *client, const std::string& data);
    void ResponseNotificationList(SIOClient *client, const std::string& data);
    void ResponseNotifyNotification(SIOClient *client, const std::string& data);
    void ResponseNotifyUpdateProfile(SIOClient *client, const std::string& data);
    void ResponseFriendList(SIOClient *client, const std::string& data);
    void ResponseFriendMatchReq(SIOClient *client, const std::string& data);
    void ResponseNotifyFriendMatchReq(SIOClient *client, const std::string& data);
    void ResponseFriendMatchRes(SIOClient *client, const std::string& data);
    void ResponseUpdateCoinRes(SIOClient *client, const std::string& data);
    void ResponseCheckNameRes(SIOClient *client, const std::string& data);
    void ResponseKnockRes(SIOClient *client, const std::string& data);

protected:
    SIOClient *_sioClient;
};

#endif /* __CARDGAME_NET_CLIENT_H__ */

