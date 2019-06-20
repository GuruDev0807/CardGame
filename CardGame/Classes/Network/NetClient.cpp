#include "NetClient.h"
#include "Packet.h"
#include "../Layer/BaseLayer.h"

NetClient::NetClient()
{
    _sioClient = NULL;
}

NetClient::~NetClient()
{
}

void NetClient::SIOClientConnect()
{
    _sioClient = SocketIO::connect("193.34.145.83:7799", *this);
    _sioClient->on(PT_RES_USERINFO, CC_CALLBACK_2(NetClient::ResponseUserInfo, this));
    _sioClient->on(PT_RES_MATCH, CC_CALLBACK_2(NetClient::ResponseMatch, this));
    _sioClient->on(PT_RES_JOINROOM, CC_CALLBACK_2(NetClient::ResponseJoinRoom, this));
    _sioClient->on(PT_NOTIFY_JOIN_PLAYER, CC_CALLBACK_2(NetClient::ResponseNotifyJoinPlayer, this));
    _sioClient->on(PT_NOTIFY_STARTGAME, CC_CALLBACK_2(NetClient::ResponseNotifyStartGame, this));
    _sioClient->on(PT_NOTIFY_LEAVE_PLAYER, CC_CALLBACK_2(NetClient::ResponseNotifyLeavePlayer, this));
    _sioClient->on(PT_NOTIFY_PUT_CARD, CC_CALLBACK_2(NetClient::ResponseNotifyPutCard, this));
    _sioClient->on(PT_NOTIFY_GAME_RESULT, CC_CALLBACK_2(NetClient::ResponseNotifyGameResult, this));
    _sioClient->on(PT_RES_UPDATE_PROFILE, CC_CALLBACK_2(NetClient::ResponseUpdateProfile, this));
    _sioClient->on(RESPONSE_USERLIST, CC_CALLBACK_2(NetClient::ResponseUserList, this));
    _sioClient->on(RESPONSE_ADD_FRIEND, CC_CALLBACK_2(NetClient::ResponseAddFriend, this));
    _sioClient->on(RESPONSE_REMOVE_FRIEND, CC_CALLBACK_2(NetClient::ResponseRemoveFriend, this));
    _sioClient->on(RESPONSE_ACCEPT_FRIEND, CC_CALLBACK_2(NetClient::ResponseAcceptFriend, this));
    _sioClient->on(RESPONSE_NOTIFICATION, CC_CALLBACK_2(NetClient::ResponseNotificationList, this));
    _sioClient->on(NOTIFY_NOTIFICATION, CC_CALLBACK_2(NetClient::ResponseNotifyNotification, this));
    _sioClient->on(NOTIFY_UPDATE_PROFIE, CC_CALLBACK_2(NetClient::ResponseNotifyUpdateProfile, this));
    _sioClient->on(RESPONSE_FRIEND_LIST, CC_CALLBACK_2(NetClient::ResponseFriendList, this));
    _sioClient->on(RESPONSE_FRIEND_MATCH_REQ, CC_CALLBACK_2(NetClient::ResponseFriendMatchReq, this));
    _sioClient->on(NOTIFY_FRIEND_MATCH_REQ, CC_CALLBACK_2(NetClient::ResponseNotifyFriendMatchReq, this));
    _sioClient->on(RESPONSE_FRIEND_MATCH_RES, CC_CALLBACK_2(NetClient::ResponseFriendMatchRes, this));
    _sioClient->on(RESPONSE_UPDATE_COIN, CC_CALLBACK_2(NetClient::ResponseUpdateCoinRes, this));
    _sioClient->on(RESPONSE_CHECK_NAME, CC_CALLBACK_2(NetClient::ResponseCheckNameRes, this));
    _sioClient->on(RESPONSE_KNOCK, CC_CALLBACK_2(NetClient::ResponseKnockRes, this));
}

void NetClient::sendPacket(std::string packet_name, std::string packet_body)
{
    if (_sioClient != NULL)
        _sioClient->emit(packet_name, packet_body);
}

void NetClient::onConnect(SIOClient* client)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onNetClientConnected();
}

void NetClient::onClose(SIOClient* client)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onNetClientClose();
}

void NetClient::onError(SIOClient* client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onNetClientError(data);
}

void NetClient::ResponseUserInfo(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_RES_USERINFO, data);
}

void NetClient::ResponseMatch(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_RES_MATCH, data);
}

void NetClient::ResponseJoinRoom(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_RES_JOINROOM, data);
}

void NetClient::ResponseNotifyJoinPlayer(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_NOTIFY_JOIN_PLAYER, data);
}

void NetClient::ResponseNotifyStartGame(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_NOTIFY_STARTGAME, data);
}

void NetClient::ResponseNotifyLeavePlayer(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_NOTIFY_LEAVE_PLAYER, data);
}

void NetClient::ResponseNotifyPutCard(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_NOTIFY_PUT_CARD, data);
}

void NetClient::ResponseNotifyGameResult(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_NOTIFY_GAME_RESULT, data);
}

void NetClient::ResponseUpdateProfile(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(PT_RES_UPDATE_PROFILE, data);
}

void NetClient::ResponseUserList(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_USERLIST, data);
}

void NetClient::ResponseAddFriend(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_ADD_FRIEND, data);
}

void NetClient::ResponseAcceptFriend(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_ACCEPT_FRIEND, data);
}

void NetClient::ResponseRemoveFriend(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_REMOVE_FRIEND, data);
}

void NetClient::ResponseNotificationList(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_NOTIFICATION, data);
}

void NetClient::ResponseNotifyNotification(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(NOTIFY_NOTIFICATION, data);
}

void NetClient::ResponseNotifyUpdateProfile(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(NOTIFY_UPDATE_PROFIE, data);
}

void NetClient::ResponseFriendList(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_FRIEND_LIST, data);
}

void NetClient::ResponseFriendMatchReq(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_FRIEND_MATCH_REQ, data);
}

void NetClient::ResponseNotifyFriendMatchReq(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(NOTIFY_FRIEND_MATCH_REQ, data);
}

void NetClient::ResponseFriendMatchRes(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_FRIEND_MATCH_RES, data);
}

void NetClient::ResponseUpdateCoinRes(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_UPDATE_COIN, data);
}

void NetClient::ResponseCheckNameRes(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_CHECK_NAME, data);
}

void NetClient::ResponseKnockRes(SIOClient *client, const std::string& data)
{
    if(g_pCurrentLayer == NULL)
        return;
    
    g_pCurrentLayer->onReceiveMessage(RESPONSE_KNOCK, data);
}
