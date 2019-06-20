#ifndef __CARDGAME_PACKET_PARSER_H__
#define __CARDGAME_PACKET_PARSER_H__

#include "../Common/Common.h"

class UserInfo;
class GameRoom;

class PacketParser
{
public:
    static PacketParser*    getInstance();
    
    std::string makeUserInfo(std::string deviceId);
    std::string makeRequestMatch(std::string userId, int roomType, int nCoin);
    std::string makeRequestJoinRoom(std::string userId, std::string roomId);
    std::string makeRequestCloseMatch(std::string userId);
    std::string makeRequestLeaveGame(std::string userId, std::string roomId);
    std::string makeRequestPutCard(std::string userId, std::string roomId, std::vector<int> vecCards, int cardFlower);
    std::string makeRequestStart(std::string userId, std::string roomId);
    std::string makeRequestRestart(std::string userId, std::string roomId);
    std::string makeRequestUpdateProfile(std::string userId, std::string new_name, std::string new_avatar, std::string new_avatar_url);
    std::string makeRequestUserList(std::string search_key);
    std::string makeRequestAddFriend(std::string userId, std::vector<std::string> new_friends);
    std::string makeRequestAcceptFriend(std::string userId, std::string notification_id, int action);
    std::string makeRequestRemoveFriend(std::string userId, std::string friend_id);
    std::string makeRequestNotificationList(std::string userId);
    std::string makeRequestFriendList(std::string userId, std::vector<std::string> friends);
    std::string makeRequestFriendMatchReq(std::string userId, std::string userName, std::vector<std::string> friends, int action, int coin);
    std::string makeRequestFriendMatchRes(std::string userId, std::string userName, std::string strFriendId, int action, int coin);
    std::string makeRequestUpdateCoinReq(std::string userId, int coin);
    std::string makeRequestCheckNameReq(std::string userId, std::string name);
    std::string makeRequestKnock(std::string userId, std::string room_id);
    
    UserInfo* parseUserInfo(std::string data);
    std::string parseMatchResponse(std::string data);
    GameRoom* parseGameRoom(std::string data);
    UserInfo* parseJoinedPlayer(std::string data);
    void parseUpdatedGameRoom(std::string data);
    LeaveInfo parseLeavePlayer(std::string data);
    GameResultInfo parseGameResult(std::string data);
    PutCardInfo parsePutCard(std::string data);
    Vector<UserInfo*> parseUserList(std::string data);
    void parseNotificationList(std::string data);
    void parseNotifyNotification(std::string data);
    MailActionInfo parseMailAction(std::string data);
    RemoveFriendResult parseRemoveFriendResult(std::string data);
    FriendMatchReqResult parseFriendMatchReq(std::string data);
    NotifyFriendMatch parseNotifyFriendMatch(std::string data);
    bool parseCheckNameResponse(std::string data);
    std::string parseKnockResponse(std::string data);
    
protected:
    PacketParser();
    
private:
    void init();
    
private:
    static PacketParser*    _instance;
};

#endif /* __CARDGAME_PACKET_PARSER_H__ */
