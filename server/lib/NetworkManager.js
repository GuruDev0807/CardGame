var Global        = require('./Global');
var MongoManager  = require('./MongoManager');

var socket;

function createServer(server) {
	var http = require('http');
    var srv = http.createServer(server);
	socket = require('socket.io')({
	  transports  : [ 'polling' ],
	}).listen(srv);

	MongoManager.init();

	socket.sockets.on(Global.CONNECTION, function (client) {
		
		console.log("## connected ! - " + client.id);
		
		client.on(Global.DISCONNECT, function () {
			console.log("## disconnected from client! - " + client.id);
			MongoManager.procDisconnect(client.id);
		});

		client.on(Global.REQUEST_USERINFO, function (data) {
			var req_json = JSON.parse(data);
			MongoManager.loginPlayer(req_json.device_id, client.id, function(response) {
				client.emit(Global.RESPONSE_USERINFO, response);
			});
		});
        
        client.on(Global.REQUEST_MATCH, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.requestMatch(req_json);
        });
        
        client.on(Global.REQUEST_JOINROOM, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procJoinRoom(req_json, function(response) {
                client.emit(Global.RESPONSE_JOINROOM, response);
            });
        });
        
        client.on(Global.REQUEST_CLOSE_MATCH, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procCloseMatch(req_json);
        });
        
        client.on(Global.REQUEST_LEAVE_ROOM, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procLeaveRoom(req_json);
        });
    
        client.on(Global.REQUEST_PUT_CARD, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procPutCard(req_json);
        });
        
        client.on(Global.REQUEST_START_GAME, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procStartGame(req_json);
        });
        
        client.on(Global.REQUEST_RESTART_GAME, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.procRestartGame(req_json);
        });
        
        client.on(Global.REQUEST_UPDATE_PROFILE, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.updateProfile(req_json, function(response) {
                client.emit(Global.RESPONSE_UPDATE_PROFILE, response);
            });
        });
        
        client.on(Global.REQUEST_USERLIST, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.searchPlayer(req_json, function(response) {
                client.emit(Global.RESPONSE_USERLIST, response);
            });
        });
        
        client.on(Global.REQUEST_ADD_FRIEND, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.addFriendRequest(req_json, function(response) {
                client.emit(Global.RESPONSE_ADD_FRIEND, response);
            });
        });
        
        client.on(Global.REQUEST_NOTIFICATION, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.notificationList(req_json, function(response) {
                client.emit(Global.RESPONSE_NOTIFICATION, response);
            });
        });
        
        client.on(Global.REQUEST_ACCEPT_FRIEND, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.acceptFriendRequest(req_json, function(response) {
                client.emit(Global.RESPONSE_ACCEPT_FRIEND, response);
            });
        });
        
        client.on(Global.REQUEST_FRIEND_LIST, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.getFriends(req_json, function(response) {
                client.emit(Global.RESPONSE_FRIEND_LIST, response);
            });
        });
        
        client.on(Global.REQUEST_REMOVE_FRIEND, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.removeFriend(req_json, function(response) {
                client.emit(Global.RESPONSE_REMOVE_FRIEND, response);
            });
        });
        
        client.on(Global.REQUEST_FRIEND_MATCH_REQ, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.matchReqFriend(req_json, function(response) {
                client.emit(Global.RESPONSE_FRIEND_MATCH_REQ, response);
            });
        });
        
        client.on(Global.REQUEST_FRIEND_MATCH_RES, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.matchResFriend(req_json);
        });
        
        client.on(Global.REQUEST_UPDATE_COIN, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.updateCoin(req_json, function(response) {
                client.emit(Global.RESPONSE_UPDATE_COIN, response); 
            });
        });
        
        client.on(Global.REQUEST_CHECK_NAME, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.checkName(req_json, function(response) {
                client.emit(Global.RESPONSE_CHECK_NAME, response); 
            });
        });
        
        client.on(Global.REQUEST_KNOCK, function(data) {
            var req_json = JSON.parse(data);
            MongoManager.sendKnock(req_json);
        });
	});
	return srv;
}

function sendPacket(sessionid, packet_name, data) {
    socket.sockets.to(sessionid).emit(packet_name, data);
}

exports.createServer = createServer;
exports.sendPacket = sendPacket;
