var Global    = require('../Global');
var NetworkManager  = require('./../NetworkManager');

var NicknameCollection	= null;
var PlayerCollection 	= null;
var DefaultCount 		= Global.DEFAULT_NICK_COUNT;

function init(mongoose) {

	var nicknameSchema = new mongoose.Schema( {
		index	: Number,
		count 	: Number,
	});

	var playerSchema = new mongoose.Schema( {
		session 	: String,
		name		: String,
        avatar      : String,
        avatar_url : String,
		device		: String,
		online		: Number,
		level		: Number,
		winCnt		: Number,
		loseCnt		: Number,
		coin        : Number,
		roomid		: String,
		state		: Number,
		history 	: Number,
		historyCnt 	: Number,
        friends     : [{
            player_id : String
        }]
	});

	NicknameCollection = mongoose.model('nickname', nicknameSchema);
    PlayerCollection = mongoose.model('player', playerSchema);
    
	NicknameCollection.findOne( { index : Global.DEFAULT_NICK_INDEX }, function(err, post) {
		if(err) {
			console.log(err);
		}
		else {
			if(post == null) {
				var newNicknameCollection = new NicknameCollection();
				newNicknameCollection.index = Global.DEFAULT_NICK_INDEX;
				newNicknameCollection.count = DefaultCount;
					
				newNicknameCollection.save(function (err) {
					if(err)
						console.log(err);
				});		
			}
			else {
				DefaultCount = post.count;
			}
		}
	});

    PlayerCollection.updateMany(
        { online : Global.PLAYER_ONLINE }, 
        { $set : 
            { session : "", online : Global.PLAYER_OFFLINE, roomid : "", state : Global.STATE_LOBBY }
        }, 
        function(err) {
            if(err)
			    console.log(err);
        }
    );
}

function login(deviceId, sessionId, callback) {

    var response_json   = {
        'success': Global.RES_FAILED,
        'profile': null
    };
    
	PlayerCollection.findOne( {device : deviceId }, function(err, post) {
		if(err) {
			console.log(err);
            callback(response_json);
            return;
		}
        
        if(post == null) {
            var newPlayerCollection = new PlayerCollection();
            newPlayerCollection.session = sessionId;
            newPlayerCollection.name = Global.DEFAULT_NICK_SUFFIX + DefaultCount;
            newPlayerCollection.avatar = "";
            newPlayerCollection.avatar_url = "";
            newPlayerCollection.device = deviceId;
            newPlayerCollection.online = Global.PLAYER_ONLINE;
            newPlayerCollection.level = Global.DEFAULT_LEVEL;
            newPlayerCollection.winCnt = 0;
            newPlayerCollection.loseCnt = 0;
            newPlayerCollection.coin = Global.DEFAULT_COIN;
            newPlayerCollection.roomid = "";
            newPlayerCollection.state = Global.STATE_LOBBY;
            newPlayerCollection.history = 0;
            newPlayerCollection.historyCnt = 0;
            newPlayerCollection.friends = [];
            DefaultCount ++;

            newPlayerCollection.save(function (err, res) {
                if(err) {
                    console.log(err);
                    callback(response_json);
                    return;
                }
                
                updateDefaultIndex();
                
                response_json.success = Global.RES_SUCCESS;
                response_json.profile = newPlayerCollection;
                callback(response_json);
            });
        }
        else {
            if(post.online == Global.PLAYER_ONLINE) {
                response_json.success = Global.RES_SUCCESS;
                response_json.profile = post;
                callback(response_json);
                return;
            }
            
            post.session = sessionId;
            post.online = Global.PLAYER_ONLINE;
            post.roomid = "";
            post.state = Global.STATE_LOBBY;

            post.save(function(err) {
                if(err) {
                    console.log(err);
                    callback(response_json);
                }
                else {
                    response_json.success = Global.RES_SUCCESS;
                    response_json.profile = post;
                    callback(response_json);
                }
            });
        }
	});
}

function logout(sessionId, callback) {
	PlayerCollection.findOne({session : sessionId}, function(err, post) {
		if(err) {
			console.log(err);
		}
		else {
			if(post == null)
				return;

            var playerId = post._id;
            post.online = Global.PLAYER_OFFLINE;
            post.state = Global.STATE_LOBBY;
            post.roomid = "";
            post.session = "";

            post.save(function(err) {
                if(err)
                    console.log(err);
                else
                    callback(playerId, true); 
            });
		}
	});
}

function updatePlayerRoomInfo(room, callback) {

    var newState = Global.STATE_LOBBY;
    if(room.type == Global.ROOM_TYPE_1)
        newState = Global.STATE_FIND_1;
    else if(room.type == Global.ROOM_TYPE_6)
        newState = Global.STATE_FIND_6;
    else if(room.type == Global.ROOM_TYPE_FRIEND)
        newState = Global.STATE_FIND_FRIEND;
    
    var players = [];
    for(var index = 0; index < room.players.length; index++)
        players.push(room.players[index].player_id);
    
    PlayerCollection.updateMany({_id: {$in : players}}, { $set : { roomid : room._id, state : newState } }, function(err) {
        if(err) {
            console.log(err);
            return;
        }
        callback(true);
    });
}

function reduceCoin(room, callback) {
    var players = [];
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].ingame == 1)
            players.push(room.players[index].player_id);
    }
    
    PlayerCollection.updateMany({_id: {$in : players}}, { $inc : { coin :  -room.coin} }, function(err) {
        if(err) {
            console.log(err);
            return;
        }
        callback(true);
    });
}

function levelup(playerId, coin) {
    PlayerCollection.findOne({_id: playerId}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }
        
        player.coin += coin;
        player.level += Global.LEVELUP_PER_MATCH;
        
        player.save(function(err) {
            if(err) {
                console.log(err);
                return;
            }
            
            var response_json   = {
                'success': Global.RES_SUCCESS,
                'profile': player
            };
            
            if(player.online == 1)
                NetworkManager.sendPacket(player.session, Global.NOTIFY_UPDATE_PROFIE, response_json);
        });
    });
}

function findPlayers(playerIds, callback) {
    PlayerCollection.find({_id: {$in : playerIds}}, function(err, players) {
        if(err) {
            console.log(err);
            return;
        }
        callback(players);
    });
}

function findPlayer(playerId, callback) {
    PlayerCollection.findOne({_id: playerId}, function(err, player) {
        if(err) {
            console.log(err);
            callback(null);
            return;
        }
        callback(player);
    });
}

function findPlayerWithSession(sessionId, callback) {
    PlayerCollection.findOne({session: sessionId}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }
        callback(player);
    });
}

function updateProfile(data, callback) {
    
    var response_json   = {
        'success': Global.RES_FAILED,
        'profile': "",
        'reason' : ""
    };
    
    checkName(data, function(response) {
        if(response.success == Global.RES_FAILED) {
            response_json.reason = "Name is using by another player already."
            callback(response_json);
            return;
        }
        
        PlayerCollection.findOne({_id : data.user_id}, function(err, player) {
            if(err) {
                console.log(err);
                return;
            }

            player.name = data.name;
            player.avatar = data.avatar;
            player.avatar_url = data.avatar_url;
            player.save(function(err) {
                if(err) {
                    console.log(err);
                    return;
                }

                response_json.success = Global.RES_SUCCESS;
                response_json.profile = player;
                callback(response_json); 
            });
        });
    });
}

function updatePlayState(playerId, newState) {
    PlayerCollection.findOne({_id: playerId}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }
        
        player.state = newState;
        player.roomid = "";
        
        player.save(function(err) {
            if(err)
                console.log(err);
        });
    });
}

function updatePlayerStateWithRoom(playerId, newState, roomId) {
    PlayerCollection.findOne({_id: playerId}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }
        
        player.state = newState;
        player.roomid = roomId;
        
        player.save(function(err) {
            if(err)
                console.log(err);
        });
    });
}

function searchPlayer(data, callback) {
    PlayerCollection.find({name : {'$regex': data.search_key}}, function(err, players) {
        if(err) {
            console.log(err);
            return;
        }
        
        var response_json   = {
            'players': players
        };
        
        callback(response_json);
    });
}

function addFriend(data) {
    PlayerCollection.findOne({_id: data.user}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }
        
        player.friends.push(data.friend);
        player.save(function(err) {
            if(err)
                console.log(err);
            
            var response_json   = {
                'success': Global.RES_SUCCESS,
                'profile': player
            };
            
            if(player.online == 1)
                NetworkManager.sendPacket(player.session, Global.NOTIFY_UPDATE_PROFIE, response_json);
        });
    });
}

function getFriends(data, callback) {
    PlayerCollection.find({_id: {$in : data.friends}}, function(err, players) {
        if(err) {
            console.log(err);
            return;
        }
        
        var response_json   = {
            'players': players
        };
        callback(response_json);
    });
}

function removeFriend(user, friend, callback) {
    var response_json   = {
        'success': Global.RES_FAILED,
    };
    
    PlayerCollection.findOne({_id: user}, function(err, player) {
        if(err) {
            console.log(err);
            callback(response_json);
            return;
        }
        
        for(var index = 0; index < player.friends.length; index++) {
            if(player.friends[index]._id == friend) {
                player.friends.splice(index, 1);
                break;
            }
        }
        
        player.save(function(err) {
            if(err) {
                console.log(err);
                callback(response_json);
                return;
            }
            
            response_json.success = Global.RES_SUCCESS;
            callback(response_json);
            
            var response_json1   = {
                'success': Global.RES_SUCCESS,
                'profile': player
            };
            
            if(player.online == 1)
                NetworkManager.sendPacket(player.session, Global.NOTIFY_UPDATE_PROFIE, response_json1);
        });
    });
}

function updateDefaultIndex() {
	NicknameCollection.findOne( { index : Global.DEFAULT_NICK_INDEX }, function(err, post) {
		if(err) {
			console.log(err);
		}
		else {
			if(post != null) {
				post.count = DefaultCount;
				post.save(function (err) {
					if(err)
						console.log(err);
				});		
			}
		}
	});
}

function updateCoin(data, callback) {
    PlayerCollection.findOne({_id : data.user_id}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }

        player.coin += data.coin;
        player.save(function(err) {
            if(err) {
                console.log(err);
                return;
            }
            
            var response_json   = {
                'success': Global.RES_SUCCESS,
                'profile': player
            };
            callback(response_json); 
        });
    });
}

function checkName(data, callback) {
    PlayerCollection.findOne({name : data.name}, function(err, player) {
        if(err) {
            console.log(err);
            return;
        }

        var response_json   = {
            'success': Global.RES_SUCCESS
        };
        
        if(player === null) {
            if(data.name === "") {
                response_json.success = Global.RES_FAILED;
                callback(response_json);
            }
            else {
                callback(response_json); 
            }
            return;
        }
        
        if(player._id == data.user_id) {
            callback(response_json);
            return;
        }
        
        response_json.success = Global.RES_FAILED;
        callback(response_json);
    });
}

exports.init                    = init;
exports.login                   = login;
exports.logout                  = logout;
exports.updatePlayerRoomInfo    = updatePlayerRoomInfo;
exports.findPlayers             = findPlayers;
exports.findPlayer              = findPlayer;
exports.updatePlayState         = updatePlayState;
exports.updatePlayerStateWithRoom   = updatePlayerStateWithRoom;
exports.reduceCoin              = reduceCoin;
exports.levelup                 = levelup;
exports.findPlayerWithSession   = findPlayerWithSession;
exports.updateProfile           = updateProfile;
exports.searchPlayer            = searchPlayer;
exports.addFriend               = addFriend;
exports.getFriends              = getFriends;
exports.removeFriend            = removeFriend;
exports.updateCoin              = updateCoin;
exports.checkName               = checkName;