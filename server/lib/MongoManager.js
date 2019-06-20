var mongoose        = require('mongoose');
var Global          = require('./Global');
var PlayerManager   = require('./mongoose/PlayerManager');
var RoomManager     = require('./mongoose/RoomManager');
var NotificationManager = require('./mongoose/NotificationManager');
var NetworkManager  = require('./NetworkManager');

function init() {
    mongoose.connect('mongodb://localhost:27017/Test', { useNewUrlParser: true });
    
    PlayerManager.init(mongoose);
    RoomManager.init(mongoose);
    NotificationManager.init(mongoose);
}

function loginPlayer(deviceId, sessionId, callback) {
    PlayerManager.login(deviceId, sessionId, function(response) {
        callback(response);
    });
}

function procDisconnect (sessionId) {
    
    PlayerManager.findPlayerWithSession(sessionId, function(player) {
        if(player == null)
            return;
        
        if(player.roomid === "") {
            PlayerManager.logout(sessionId, function(playerId, bCompleted) {
            });
        }
        else {
            var data = {
                "user_id" : player._id,
                "room_id" : player.roomid
            };
            
            RoomManager.leavePlayer(data, function(room) {
                if(room != null) {
                    var playerIds = [];
                    for(var index = 0; index < room.players.length; index++) {
                        if(room.players[index].joined == 1)
                            playerIds.push(room.players[index].player_id);
                    }

                    PlayerManager.findPlayers(playerIds, function(players) {

                        var response_json = {
                            "room_id" : room._id,
                            "user_id" : data.user_id
                        }

                        for(index = 0; index < players.length; index++) {
                            if(players[index]._id != data.user_id && players[index].online == 1)
                                NetworkManager.sendPacket(players[index].session, Global.NOTIFY_LEAVE_PLAYER, response_json);
                        }
                    });
                }

                PlayerManager.logout(sessionId, function(playerId, bCompleted) {
                });
            });
        }
    });
}

function requestMatch (data) {
    RoomManager.findRoom(data.user_id, data.room_type, data.coin, function(response) {
       if(response == null) {
           RoomManager.createRoom(data.user_id, data.room_type, data.coin, function(room) {
               PlayerManager.updatePlayerRoomInfo(room, function(response1) {
                   
               });
           });
       } 
       else {
           PlayerManager.updatePlayerRoomInfo(response, function(response2) {
               var playerIds = [];
               for(var index = 0; index < response.players.length; index++) {
                   if(response.players[index].joined === 0)
                       playerIds.push(response.players[index].player_id);
               }
               
               PlayerManager.findPlayers(playerIds, function(players) {
                   var response_data = {
                       "room_id" : response._id
                   }
                   
                   for(index = 0; index < players.length; index++)
                       NetworkManager.sendPacket(players[index].session, Global.RESPONSE_MATCH, response_data);
               });
           });
       }
    });
}

function procJoinRoom (data, callback) {
    
    var response_json   = {
        'success': Global.RES_FAILED,
        'room': null
    };
    
    RoomManager.joinPlayer(data, function(room) {
        if(room == null) {
            callback(response_json);
            return;
        }
        
        var playerIds = [];
        for(var index = 0; index < room.players.length; index++) {
            if(room.players[index].joined == 1)
                playerIds.push(room.players[index].player_id);
        }

        PlayerManager.findPlayers(playerIds, function(players) {
            
            response_json.success = Global.RES_SUCCESS;
            response_json.room = makeRoomInfo(room, players);
            
            callback(response_json);
            
            var joined_player;
            for(index = 0; index < players.length; index++) {
                if(players[index]._id == data.user_id) {
                    joined_player = players[index];
                    break;
                }
            }
            
            for(index = 0; index < players.length; index++) {
                if(players[index]._id != data.user_id && players[index].online == 1)
                    NetworkManager.sendPacket(players[index].session, Global.NOTIFY_JOIN_PLAYER, makePlayerInfo(room, joined_player));
            }
            
            var all_startreq = true;
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].start_req != 1) {
                    all_startreq = false;
                    break;
                }
            }

            if((room.type == Global.ROOM_TYPE_1 && players.length == room.max_member && all_startreq) || 
               (room.type == Global.ROOM_TYPE_6 && players.length >= 2 && all_startreq)) {
                RoomManager.startGame(room._id, function(room1) {
                    PlayerManager.reduceCoin(room1, function(res) {
                        
                        var newPlayerIds = [];
                        for(var index = 0; index < room1.players.length; index++) {
                            if(room1.players[index].joined == 1)
                                newPlayerIds.push(room1.players[index].player_id);
                        }
                        
                        PlayerManager.findPlayers(newPlayerIds, function(newPlayers) {
                            response_json.room = makeRoomInfo(room1, newPlayers);
                            for(index = 0; index < newPlayers.length; index++) {
                                var update_json = {
                                    'success' : Global.RES_SUCCESS,
                                    'profile' : newPlayers[index]
                                };
                                if(newPlayers[index].online == 1) {
                                    NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_STARTGAME, response_json.room);
                                    NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_UPDATE_PROFIE, update_json);
                                }
                            }
                        });
                    });
                });
            }
        });
    });
}

function procCloseMatch(data) {
    RoomManager.closeMatch(data, function(response) {
        if(response)
            PlayerManager.updatePlayState(data.user_id, Global.STATE_LOBBY);
    });
}

function procLeaveRoom(data) {
    
    RoomManager.leavePlayer(data, function(room) {
        if(room != null) {
            var playerIds = [];
            for(var index = 0; index < room.players.length; index++) {
                if(room.players[index].joined == 1)
                    playerIds.push(room.players[index].player_id);
            }

            PlayerManager.findPlayers(playerIds, function(players) {

                var response_json = {
                    "room_id" : room._id,
                    "user_id" : data.user_id
                }

                for(index = 0; index < players.length; index++) {
                    if(players[index]._id != data.user_id && players[index].online == 1)
                        NetworkManager.sendPacket(players[index].session, Global.NOTIFY_LEAVE_PLAYER, response_json);
                }
            });
        }
        
        PlayerManager.updatePlayState(data.user_id, Global.STATE_LOBBY);
    });
}

function notifyGameResult(room, winnerId) {
    
    PlayerManager.levelup(winnerId, room.pending_coin);
    
    var playerIds = [];
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].joined == 1)
            playerIds.push(room.players[index].player_id);
    }

    PlayerManager.findPlayers(playerIds, function(players) {

        var response_json = {
            "room_id" : room._id,
            "winner_id" : winnerId,
            "winner_coin" : room.pending_coin
        }

        for(index = 0; index < players.length; index++) {
            if(players[index].online == 1)
                NetworkManager.sendPacket(players[index].session, Global.NOTIFY_GAME_RESULT, response_json);
        }
    });
}

function procPutCard(data) {
    RoomManager.procPutCard(data);
}

function notifyPutCard(room, prev_tune, card, card_flower, deal_newcard, newcard_count, pick_state) {
    var playerIds = [];
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].joined == 1)
            playerIds.push(room.players[index].player_id);
    }

    PlayerManager.findPlayers(playerIds, function(players) {

        var response_json = {
            "room_id" : room._id,
            "prev_tune" : prev_tune,
            "cur_tune" : room.cur_position,
            "card_num" : card,
            "card_flower" : card_flower,
            "deal_newcard" : deal_newcard,
            "deal_cardcount" : newcard_count,
            "pick_state" : pick_state
        }

        for(index = 0; index < players.length; index++) {
            if(players[index].online == 1)
                NetworkManager.sendPacket(players[index].session, Global.NOTIFY_PUT_CARD, response_json);
        }
    });
}

function procStartGame(data) {
    RoomManager.startGame(data.room_id, function(room) {
        PlayerManager.reduceCoin(room, function(res) {
            var newPlayerIds = [];
            for(var index = 0; index < room.players.length; index++) {
                if(room.players[index].joined == 1)
                    newPlayerIds.push(room.players[index].player_id);
            }
            
            PlayerManager.findPlayers(newPlayerIds, function(newPlayers) {
                var room_json = makeRoomInfo(room, newPlayers);
                for(index = 0; index < newPlayers.length; index++) {
                    var update_json = {
                        'success' : Global.RES_SUCCESS,
                        'profile' : newPlayers[index]
                    };
                    if(newPlayers[index].online == 1) {
                        NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_STARTGAME, room_json);
                        NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_UPDATE_PROFIE, update_json);
                    }
                }
            });
        });
    });
}

function procRestartGame(data) {
    
    var response_json   = {
        'success': Global.RES_FAILED,
        'room': null
    };
    
    RoomManager.procRestartGame(data, function(room) {
        if(room == null)
            return;
        
        var playerIds = [];
        for(var index = 0; index < room.players.length; index++) {
            if(room.players[index].joined == 1)
                playerIds.push(room.players[index].player_id);
        }

        PlayerManager.findPlayers(playerIds, function(players) {

            var all_startreq = true;
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].start_req != 1) {
                    all_startreq = false;
                    break;
                }
            }

            if((room.type == Global.ROOM_TYPE_1 && players.length == room.max_member && all_startreq) ||
               (room.type == Global.ROOM_TYPE_FRIEND && players.length == room.max_member && all_startreq) ||
              (room.type == Global.ROOM_TYPE_6 && players.length >= 2 && all_startreq)) {
                RoomManager.startGame(room._id, function(room1) {
                    PlayerManager.reduceCoin(room1, function(res) {
            
                        var newPlayerIds = [];
                        for(var index = 0; index < room1.players.length; index++) {
                            if(room1.players[index].joined == 1)
                                newPlayerIds.push(room1.players[index].player_id);
                        }

                        PlayerManager.findPlayers(newPlayerIds, function(newPlayers) {
                            response_json.room = makeRoomInfo(room1, newPlayers);
                            for(index = 0; index < newPlayers.length; index++) {
                                var update_json = {
                                    'success' : Global.RES_SUCCESS,
                                    'profile' : newPlayers[index]
                                };
                                if(newPlayers[index].online == 1) {
                                    NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_STARTGAME, response_json.room);
                                    NetworkManager.sendPacket(newPlayers[index].session, Global.NOTIFY_UPDATE_PROFIE, update_json);
                                }
                            }
                        });
                    });
                });
            }
        });
    });
}

function updateProfile(data, callback) {
    PlayerManager.updateProfile(data, function(response) {
       callback(response); 
    });
}

function searchPlayer(data, callback) {
    PlayerManager.searchPlayer(data, function(response) {
       callback(response); 
    });
}

function addFriendRequest(data, callback) {
    NotificationManager.addFriendRequest(data, function(response) {
       callback(response); 
    });
}

function acceptFriendRequest(data, callback) {
    NotificationManager.acceptFriendRequest(data, function(response) {
       callback(response); 
    });
}

function notificationList(data, callback) {
    NotificationManager.notificationList(data, function(response) {
        var response_json = {
            "notifications" : []  
        };
        
        if(response.length == 0) {
            callback(response_json);
            return;
        }
        
        var playerIds = [];
        var index;
        var index1;
        for(index = 0; index < response.length; index++) {
            var exist = false;
            for(index1 = 0; index1 < playerIds.length; index1++) {
                if(response[index].sender == playerIds[index1]) {
                    exist = true;
                    break;
                }
            }
            
            if(!exist)
                playerIds.push(response[index].sender);
        }

        PlayerManager.findPlayers(playerIds, function(players) {

            for(index = 0; index < response.length; index++) {
                for(index1 = 0; index1 < players.length; index1++) {
                    if(response[index].sender == players[index1]._id) {
                        response_json.notifications.push({
                            "_id" : response[index]._id,
                            "type" : response[index].type,
                            "sender_id" : response[index].sender,
                            "sender_name" : players[index1].name,
                            "sender_level" : players[index1].level,
                            "sender_online" : players[index1].online,
                            "receiver_id" : response[index].receiver,
                            "receiver_action" : response[index].receive_action
                        });
                        break;
                    }
                }
            }
            
            callback(response_json);
        });
    });
}

function notifyNotification(id, type, sender, receiver, action) {
    var response_json = {
        "notification" : {}  
    };

    var playerIds = [];
    playerIds.push(sender);
    playerIds.push(receiver);

    PlayerManager.findPlayers(playerIds, function(players) {

        var index;
        for(index = 0; index < players.length; index++) {
            if(players[index]._id == sender) {
                response_json.notification = {
                    "_id" : id,
                    "type" : type,
                    "sender_id" : sender,
                    "sender_name" : players[index].name,
                    "sender_level" : players[index].level,
                    "sender_online" : players[index].online,
                    "receiver_id" : receiver,
                    "receiver_action" : action
                };
                break;
            }
        }
        
        for(index = 0; index < players.length; index++) {
            if(players[index]._id == receiver && players[index].online == 1)
                NetworkManager.sendPacket(players[index].session, Global.NOTIFY_NOTIFICATION, response_json);
        }
    });
}

function addFriend(dataList) {
    for(var index = 0; index < dataList.length; index++) {
        PlayerManager.addFriend(dataList[index]);
    }
}

function getFriends(data, callback) {
    PlayerManager.getFriends(data, function(response) {
       callback(response); 
    });
}

function removeFriend(data, callback) {
    
    var response_json = {
        "success" : Global.RES_FAILED,
        "friend_id" : data.friend_id
    }
    
    PlayerManager.removeFriend(data.user_id, data.friend_id, function(response) {
        if(response.success == Global.RES_FAILED) {
            callback(response_json);
            return;
        }
        
        PlayerManager.removeFriend(data.friend_id, data.user_id, function(response1) {
            if(response1.success == Global.RES_FAILED) {
                callback(response_json);
                return;
            }
            
            NotificationManager.removeFriend(data.user_id, data.friend_id, function(response2) {
                if(response2.success == Global.RES_FAILED) {
                    callback(response_json);
                    return;
                }
                
                response_json.success = Global.RES_SUCCESS;
                callback(response_json);
            });
        });
    });
}

function makeRoomInfo(room, players) {
    
    var room_response = {
        "_id" : room._id,
        "type" : room.type,
        "coin" : room.coin,
        "started" : room.started,
        "max_member" : room.max_member,
        "deck_cards" : room.deck_cards,
        "remain_cards" : room.remain_cards,
        "start_position" : room.start_position,
        "cur_position" : room.cur_position,
        "creater" : room.creater,
        "players" : []
    }

    var index, index1;
    for(index = 0; index < players.length; index++) {
        for(index1 = 0; index1 < room.players.length; index1++) {
            if(room.players[index1].player_id == players[index]._id) {
                room_response.players.push({
                    "_id"        : players[index]._id,
                    "session"    : players[index].session,
                    "name"       : players[index].name,
                    "avatar"     : players[index].avatar,
                    "avatar_url" : players[index].avatar_url,
                    "device"     : players[index].device,
                    "online"     : players[index].online,
                    "level"      : players[index].level,
                    "winCnt"     : players[index].winCnt,
                    "loseCnt"    : players[index].loseCnt,
                    "coin"       : players[index].coin,
                    "roomid"     : players[index].roomid,
                    "state"      : players[index].state,
                    "history"    : players[index].history,
                    "historyCnt" : players[index].historyCnt,
                    "joined"     : room.players[index1].joined,
                    "start_req"  : room.players[index1].start_req,
                    "ingame"     : room.players[index1].ingame,
                    "position"   : room.players[index1].position,
                    "cards"      : room.players[index1].cards
                });
                break;
            }
        }
    }
    
    return room_response;
}

function makePlayerInfo(room, player) {
    
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].player_id == player._id) {
            var player_response = {
                "_id"        : player._id,
                "session"    : player.session,
                "name"       : player.name,
                "avatar"     : player.avatar,
                "avatar_url" : player.avatar_url,
                "device"     : player.device,
                "online"     : player.online,
                "level"      : player.level,
                "winCnt"     : player.winCnt,
                "loseCnt"    : player.loseCnt,
                "coin"       : player.coin,
                "roomid"     : player.roomid,
                "state"      : player.state,
                "history"    : player.history,
                "historyCnt" : player.historyCnt,
                "joined"     : room.players[index].joined,
                "start_req"  : room.players[index].start_req,
                "ingame"     : room.players[index].ingame,
                "position"   : room.players[index].position,
                "cards"      : room.players[index].cards
            }
            
            return player_response;
        }    
    }
    
    return null;
}

function matchReqFriend(data, callback) {
    
    var response_json1 = {
        "success" : Global.RES_FAILED,
        "action" : data.action
    }
    
    var index;
    var send_count = 0;
    if(data.action === Global.FRIEND_MATCH_REQ_SEND) {
        PlayerManager.getFriends(data, function(friend_players) {
            
            if(friend_players == null) {
                response_json1.success = Global.RES_FAILED;
                callback(response_json1);
                return;
            }
            
            RoomManager.createRoom(data.user_id, Global.ROOM_TYPE_FRIEND, data.coin, function(room) {
                PlayerManager.updatePlayerRoomInfo(room, function(response1) {
                    for(index = 0; index < friend_players.players.length; index++) {
                        var friend = friend_players.players[index];
                        if(friend.online === 0)
                            continue;

                        if(friend.state != Global.STATE_LOBBY)
                            continue;

                        if(friend.coin < data.coin)
                            continue;

                        RoomManager.addPlayerToRoom(friend._id, room._id, function(response2) {
                            
                        });
                        
                        var response_json = {
                            "friend_id" : data.user_id,
                            "friend_name" : data.user_name,
                            "action" : Global.FRIEND_MATCH_REQ_SEND,
                            "coin" : data.coin
                        }

                        NetworkManager.sendPacket(friend.session, Global.NOTIFY_FRIEND_MATCH_REQ, response_json);
                        send_count++;
                    }
                    
                    if(send_count == 0) {
                        RoomManager.deletePlayerRoom(data.user_id, function(response) {
                            if(response.success == Global.RES_FAILED) {
                                response_json1.success = Global.RES_FAILED;
                                callback(response_json1);
                                return;
                            }
                            
                            PlayerManager.updatePlayState(data.user_id, Global.STATE_LOBBY);
                            response_json1.success = Global.RES_FAILED;
                            callback(response_json1);
                        });
                    }
                });
            });
        });
    }
    else if(data.action == Global.FRIEND_MATCH_REQ_CLOSE) {
        RoomManager.deletePlayerRoom(data.user_id, function(response) {
            if(response.success == Global.RES_FAILED) {
                response_json1.success = Global.RES_FAILED;
                callback(response_json1);
                return;
            }
            
            PlayerManager.updatePlayState(data.user_id, Global.STATE_LOBBY);
            PlayerManager.getFriends(data, function(friend_players) {
                if(friend_players == null) {
                    response_json1.success = Global.RES_FAILED;
                    callback(response_json1);
                    return;
                }

                for(index = 0; index < friend_players.players.length; index++) {
                    var friend = friend_players.players[index];
                    if(friend.online === 0)
                        continue;

                    if(friend.state != Global.STATE_LOBBY)
                        continue;

                    if(friend.coin < data.coin)
                        continue;

                    var response_json = {
                        "friend_id" : data.user_id,
                        "friend_name" : data.user_name,
                        "action" : Global.FRIEND_MATCH_REQ_CLOSE,
                        "coin" : data.coin
                    }

                    NetworkManager.sendPacket(friend.session, Global.NOTIFY_FRIEND_MATCH_REQ, response_json);
                    send_count++;
                }

                if(send_count == 0) {
                    response_json1.success = Global.RES_FAILED;
                    callback(response_json1);
                    return;
                }
                
                response_json1.success = Global.RES_SUCCESS;
                callback(response_json1);
            });
        });
    }
}

function completeFriendMatch(data, room, joiner_count) {
    var playerIds = [];
    var index;
    for(index = 0; index < room.players.length; index++) {
        if(room.players[index].joined == 1)
            playerIds.push(room.players[index].player_id);
    }
    
    PlayerManager.findPlayers(playerIds, function(players) {
        
        var room_json = {
            'success' : Global.RES_FAILED,
            'room' : null
        }
        
        room_json.success = Global.RES_SUCCESS;
        room_json.room = makeRoomInfo(room, players);
        
        if(joiner_count === 1) {
            
            var joined_player;
            for(index = 0; index < players.length; index++) {
                if(players[index]._id == data.user_id) {
                    joined_player = players[index];
                    break;
                }
            }
            
            for(index = 0; index < players.length; index++) {
                if(players[index].online === 1) {
                    if(players[index]._id == data.user_id) {
                        NetworkManager.sendPacket(players[index].session, Global.RESPONSE_JOINROOM, room_json);
                    }
                    else {
                        NetworkManager.sendPacket(players[index].session, Global.NOTIFY_JOIN_PLAYER, makePlayerInfo(room, joined_player));
                    }
                }
            }
        }
        else if(joiner_count === 2) {
            for(index = 0; index < players.length; index++) {
                if(players[index].online === 1) {
                    if(players[index]._id == data.user_id || players[index]._id == data.friend_id) {
                        NetworkManager.sendPacket(players[index].session, Global.RESPONSE_JOINROOM, room_json);
                    }
                }
            }
        }
    });
}

function matchResFriend(data) {
    if(data.action == Global.FRIEND_MATCH_REQ_ACCEPT) {
        RoomManager.findPlayerRoom(data, function(response) {
            if(response.success == Global.RES_SUCCESS || (response.success == Global.RES_FAILED && response.room == null)) {
                console.log("error happened when friend accept match.");
                return;
            }
            
            PlayerManager.updatePlayerStateWithRoom(data.user_id, Global.STATE_FIND_FRIEND, response.room._id);
            
            var index;
            var friend_player = null;
            var data1, data2;
            
            for(index = 0; index < response.room.players.length; index++) {
                if(response.room.players[index].player_id == data.friend_id) {
                    friend_player = response.room.players[index];
                    break;
                }
            }
            
            if(friend_player == null)
                return;
            
            if(friend_player.joined === 1) {
                data1 = {
                    'user_id' : data.user_id,
                    'room_id' : response.room._id
                };
                
                RoomManager.joinPlayer(data1, function(room1) {
                    if(room1 == null) {
                        console.log("error happened when friend1 join to match.");
                        return;
                    }
                    
                    completeFriendMatch(data, room1, 1);
                });
            }
            else {
                data1 = {
                    'user_id' : data.user_id,
                    'room_id' : response.room._id
                };
                
                data2 = {
                    'user_id' : data.friend_id,
                    'room_id' : response.room._id
                };
                
                RoomManager.joinPlayer(data1, function(room1) {
                    if(room1 == null) {
                        console.log("error happened when friend1 join to match.");
                        return;
                    }
                    
                    RoomManager.joinPlayer(data2, function(room2) {
                        if(room2 == null) {
                            console.log("error happened when friend2 join to match.");
                            return;
                        }
                        
                        completeFriendMatch(data, room2, 2);
                    });
                });
            }
        });
    }
    else if(data.action == Global.FRIEND_MATCH_REQ_DECLINE) {
        RoomManager.findPlayerRoom(data, function(response) {
            if(response.room == null)
                return;
            
            var data1 = {
                "room_id" : response.room._id,
                "user_id" : data.user_id
            }
            
            RoomManager.leavePlayer(data1, function(response1) {
                if(response1 != null && response1.players.length === 1) {
                    if(response1.players[0].player_id == data.friend_id) {
                         RoomManager.deletePlayerRoom(data.friend_id, function(response2) {
                            if(response2.success == Global.RES_FAILED)
                                return;

                            PlayerManager.updatePlayState(data.friend_id, Global.STATE_LOBBY);

                            PlayerManager.findPlayer(data.friend_id, function(response3) {
                                if(response3 == null)
                                    return;

                                if(response3.online == 0)
                                    return;

                                var response_json = {
                                    "friend_id" : data.user_id,
                                    "friend_name" : data.user_name,
                                    "action" : Global.FRIEND_MATCH_REQ_DECLINE,
                                    "coin" : data.coin
                                }

                                NetworkManager.sendPacket(response3.session, Global.RESPONSE_FRIEND_MATCH_RES, response_json);
                            });
                        });
                    }
                }
            });
        });
    }
}

function updateCoin(data, callback) {
    PlayerManager.updateCoin(data, function(response) {
       callback(response); 
    });
}

function checkName(data, callback) {
    PlayerManager.checkName(data, function(response) {
       callback(response); 
    });
}

function sendKnock(data) {
    RoomManager.findRoomWithId(data.room_id, function(room) {
        if(room == null)
            return;
        
        var playerIds = [];
        var index;
        for(index = 0; index < room.players.length; index++) {
            if(room.players[index].joined == 1)
                playerIds.push(room.players[index].player_id);
        }

        PlayerManager.findPlayers(playerIds, function(players) {

            for(index = 0; index < players.length; index++) {
                if(players[index].online === 1) {
                    NetworkManager.sendPacket(players[index].session, Global.RESPONSE_KNOCK, data);
                }
            }
        });
    });
}

exports.init                = init;
exports.loginPlayer         = loginPlayer;
exports.procDisconnect      = procDisconnect;
exports.requestMatch        = requestMatch;
exports.procJoinRoom        = procJoinRoom;
exports.procCloseMatch      = procCloseMatch;
exports.procLeaveRoom       = procLeaveRoom;
exports.notifyGameResult    = notifyGameResult;
exports.procPutCard         = procPutCard;
exports.notifyPutCard       = notifyPutCard;
exports.procStartGame       = procStartGame;
exports.procRestartGame     = procRestartGame;
exports.updateProfile       = updateProfile;
exports.searchPlayer        = searchPlayer;
exports.addFriendRequest    = addFriendRequest;
exports.notificationList    = notificationList;
exports.notifyNotification  = notifyNotification;
exports.acceptFriendRequest = acceptFriendRequest;
exports.addFriend           = addFriend;
exports.getFriends          = getFriends;
exports.removeFriend        = removeFriend;
exports.matchReqFriend      = matchReqFriend;
exports.matchResFriend      = matchResFriend;
exports.updateCoin          = updateCoin;
exports.checkName           = checkName;
exports.sendKnock           = sendKnock;