var Global          = require('../Global');
var MongoManager    = require('../MongoManager');

var RoomCollection      = null;

function init(mongoose) {
    
    var RoomSchema = new mongoose.Schema({
        type : Number,
        coin : Number,
        started : Number,
        max_member : Number,
        players : [{
            player_id : String,
            position : Number,
            joined : Number,
            start_req : Number,
            ingame : Number,
            cards : []
        }],
        deck_cards : [],
        remain_cards : [],
        start_position : Number,
        cur_position : Number,
        direction : Number,
        pending_coin : Number,
        pickup_2 : Number,
        pickup_4 : Number,
        pickup_6 : Number,
        pickup_8 : Number,
        pickup_5 : Number,
        pickup_10 : Number,
        creater : String
    });
    
    RoomCollection = mongoose.model('room', RoomSchema);
    RoomCollection.deleteMany({}, function(err, res) {
        if(err)
            console.log(err);
    });
}

function createRoom(userId, roomType, gameCoin, callback) {
    
    var maxMember = 0;
    if(roomType == Global.ROOM_TYPE_1)
        maxMember = Global.MAX_MEMBER_ROOM_1;
    else if(roomType == Global.ROOM_TYPE_6)
        maxMember = Global.MAX_MEMBER_ROOM_6;
    else if(roomType == Global.ROOM_TYPE_FRIEND)
        maxMember = Global.MAX_MEMBER_ROOM_FRIEND;
    
    var newRoomCollection = new RoomCollection();
    newRoomCollection.type = roomType;
    newRoomCollection.coin = gameCoin;
    newRoomCollection.started = 0;
    newRoomCollection.max_member = maxMember;
    newRoomCollection.deck_cards = [];
    newRoomCollection.remain_cards = [];
    newRoomCollection.start_position = -1;
    newRoomCollection.cur_position = -1;
    newRoomCollection.direction = 1;
    newRoomCollection.pending_coin = 0;
    newRoomCollection.pickup_2 = 0;
    newRoomCollection.pickup_4 = 0;
    newRoomCollection.pickup_6 = 0;
    newRoomCollection.pickup_8 = 0;
    newRoomCollection.pickup_5 = 0;
    newRoomCollection.pickup_10 = 0;
    newRoomCollection.creater = userId;
    newRoomCollection.players.push({
        player_id : userId,
        position : 0,
        joined : 0,
        start_req : 0,
        ingame : 0,
        cards : []
    });
    
    newRoomCollection.save(function (err, room) {
        if(err) {
            console.log(err);
            return;
        }

        callback(room);
    });
}

function findRoom(userId, roomType, gameCoin, callback) {
    
    RoomCollection.findOne({type: roomType, coin: gameCoin, "players.player_id" : {$ne : userId}, $where : "this.players.length < this.max_member"}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        }
        
        if(room == null) {
            callback(null);
            return;
        }
        
        var index;
        var positions = [];
        for(index = 0; index < room.max_member; index++)
            positions.push(index);
        
        for(index = 0; index < room.players.length; index++) {
            var player_position = room.players[index].position;
            var positions_index = positions.indexOf(player_position);
            positions.splice(positions_index, 1);
        }
        
        room.players.push({
            player_id : userId,
            position : positions[0],
            joined : 0,
            start_req : 0,
            ingame : 0,
            cards : []
        });
        
        room.players.sort(compare);
        
        room.save(function(err, res) {
            if(err) {
                console.log(err);
                return;
            }
            
            callback(res);
        });
    });
}

function findRoomWithId(roomId, callback) {
    RoomCollection.findOne({_id : roomId}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        }
        
        callback(room);
    })
}

function addPlayerToRoom(userId, roomId, callback) {
    
    RoomCollection.findOne({_id : roomId}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        }
        
        if(room == null) {
            callback(null);
            return;
        }
        
        var index;
        var positions = [];
        for(index = 0; index < room.max_member; index++)
            positions.push(index);
        
        for(index = 0; index < room.players.length; index++) {
            var player_position = room.players[index].position;
            var positions_index = positions.indexOf(player_position);
            positions.splice(positions_index, 1);
        }
        
        room.players.push({
            player_id : userId,
            position : positions[0],
            joined : 0,
            start_req : 0,
            ingame : 0,
            cards : []
        });
        
        room.players.sort(compare);
        
        room.save(function(err, res) {
            if(err) {
                console.log(err);
                return;
            }
            
            callback(res);
        });
    });
}

function joinPlayer(data, callback) {
    
    RoomCollection.findOne({_id : data.room_id}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        } 
        
        if(room == null) {
            callback(null);
            return;
        }
        
        var bUpdated = false;
        for(var index = 0; index < room.players.length; index++) {
            if(data.user_id == room.players[index].player_id) {
                if(room.players[index].joined === 0) {
                    room.players[index].joined = 1;
                    room.players[index].start_req = 1;
                    bUpdated = true;
                    break;
                }
            }
        }
        
        if(!bUpdated) {
            callback(null);
            return;
        }
        
        room.save(function (err, updatedRoom) {
            if(err) {
                console.log(err);
                return;
            }

            callback(updatedRoom);
        });
    });
}

function leavePlayer(data, callback) {
    
    RoomCollection.findOne({_id : data.room_id}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        }
        
        if(room == null) {
            callback(null);
            return;
        }
        
        var index = 0;
        var player;
        
        for(index = 0; index < room.players.length; index++) {
            if(room.players[index].player_id == data.user_id) {
                player = room.players[index];
                break;
            }
        }
        
        if(player.ingame) {
            for(index = 0; index < player.cards.length; index++)
                room.deck_cards.push(player.cards[index]);
            
            room.save(function(err, updatedRoom) {
                if(err) {
                    console.log(err);
                    return;
                }

                RoomCollection.updateOne({_id : data.room_id}, {$pull : {"players" : {player_id : data.user_id}}}, function(err) {
                    if(err) {
                        console.log(err);
                        return;
                    }

                    RoomCollection.findOne({_id : data.room_id}, function(err, newRoom) {
                        if(err) {
                            console.log(err);
                            return;
                        }

                        callback(newRoom);
                        
                        if(newRoom.type == Global.ROOM_TYPE_1 || newRoom.type == Global.ROOM_TYPE_FRIEND) {
                            completeGame(false, data.user_id, newRoom);
                        }
                        else if(newRoom.type == Global.ROOM_TYPE_6) {
                            var gamers = [];
                            for(index = 0; index < newRoom.players.length; index++) {
                                if(newRoom.players[index].ingame == 1)
                                    gamers.push(newRoom.players[index].player_id);
                            }
                            
                            if(gamers.length == 1) {
                                completeGame(true, gamers[0], newRoom);
                                gamers.length = 0;
                            }
                        }
                        else {
                        }
                    });
                });
            });
        }
        else {
            RoomCollection.updateOne({_id : data.room_id}, {$pull : {"players" : {player_id : data.user_id}}}, function(err) {
                if(err) {
                    console.log(err);
                    return;
                }
                
                RoomCollection.findOne({_id : data.room_id}, function(err, newRoom) {
                    if(err) {
                        console.log(err);
                        return;
                    }
                    
                    if(newRoom.players.length === 0) {
                        RoomCollection.deleteMany({_id : data.room_id}, function(err, res) {
                            if(err) {
                                console.log(err);
                                return;
                            } 

                            callback(null);
                        });
                    }
                    else {
                        callback(newRoom);
                    }
                });
            });
        }
    });
}

function closeMatch(data, callback) {
    RoomCollection.deleteMany({"players.player_id" : data.user_id}, function(err, res) {
        if(err) {
            console.log(err);
            return;
        } 
        
        callback(true);
    });
}

function startGame(room_id, callback) {
    RoomCollection.findOne({_id : room_id}, function(err, room) {
        if(err) {
            console.log(err);
            return;
        } 
        
        if(room == null) {
            callback(null);
            return;
        }

        var index;
        var playerCount = 0;
        
        var bTurnProcessed = false;
        while(!bTurnProcessed) {
            room.start_position++;
            if(room.start_position == room.max_member)
                room.start_position = 0;
            
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].start_req == 1 && room.start_position == room.players[index].position) {
                    room.cur_position = room.start_position;
                    bTurnProcessed = true;
                    break;
                }
            }
        }
        
        room.started = 1;
        room.remain_cards.length = 0;
        room.deck_cards.length = 0;
        room.pending_coin = 0;
        room.pickup_2 = 0;
        room.pickup_4 = 0;
        room.pickup_6 = 0;
        room.pickup_8 = 0;
        room.pickup_5 = 0;
        room.pickup_10 = 0;
        for(index = 0; index < room.players.length; index++) {
            room.players[index].cards.length = 0;
            if(room.players[index].joined == 1 && room.players[index].start_req == 1) {
                room.players[index].ingame = 1;
                room.pending_coin += room.coin;
                playerCount++;
            }
        }
        
        if(room.remain_cards.length === 0) {
            for(index = 0; index < 52; index++)
                room.remain_cards.push(index);
        }
        
        for(index = 0; index < 26; index++) {
            var srcIndex = generateRandomNumber(1, 52) - 1;
            var descIndex = generateRandomNumber(1, 52) - 1;
            
            if(srcIndex == descIndex)
                continue;
            
            var tempCard = room.remain_cards[srcIndex];
            room.remain_cards[srcIndex] = room.remain_cards[descIndex];
            room.remain_cards[descIndex] = tempCard;
        }
        
        var player_cards = [];
        var player_index = 0;
        
        for(index = 0; index < playerCount; index++)
            player_cards[index] = [];
        
        for(index = 0; index < 7 * playerCount; index++) {
            player_cards[player_index % playerCount].push(room.remain_cards.splice(0, 1)[0]);
            player_index++;
        }
        
        room.deck_cards.push(room.remain_cards.splice(0, 1)[0]);
        
        player_index = 0;
        for(index = 0; index < room.players.length; index++) {
            if(room.players[index].ingame == 1) {
                room.players[index].cards = player_cards[player_index];
                player_index++;
            }
        }
        
        room.save(function(err, updatedRoom) {
            if(err) {
                console.log(err);
                return;
            }
            
            for(index = 0; index < playerCount; index++)
                player_cards[index].length = 0;
            player_cards.length = 0;
            
            callback(updatedRoom);
        });
    });
}

function completeGame(isWinner, playerId, room) {
    
    var index, index1;
    for(index = 0; index < room.players.length; index++) {
        for(index1 = 0; index1 < room.players[index].cards.length; index1++)
            room.deck_cards.push(room.players[index].cards[index1]);
        room.players[index].cards = [];
        if(room.players[index].ingame == 1) {
            room.players[index].ingame = 0;
            room.players[index].start_req = 0;
        }
    }
    
    for(index = 0; index < room.deck_cards.length; index++) {
        if(room.deck_cards[index] != Global.NUM_KNOCK)
            room.remain_cards.push(room.deck_cards[index]);
    }
    room.deck_cards = [];
    room.started = 0;
    room.direction = 1;
    
    room.save(function(err, updatedRoom) {
        if(err) {
            console.log(err);
            return;
        }
        
        var winnerId;
        if(isWinner) {
            winnerId = playerId;
        }
        else {
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].player_id != playerId) {
                    winnerId = room.players[index].player_id;
                    break;
                }
            }
        }
        MongoManager.notifyGameResult(updatedRoom, winnerId);
    });
}

function procPutCard(data) {
    RoomCollection.findOne({_id : data.room_id}, function(err, room) {
        if(err)
            return;
        
        if(room == null)
            return;
        
        var cur_player = findPlayer(room, room.cur_position);
        if(cur_player.player_id != data.user_id)
            return;

        var index;
        var deal_newcard = false;
        var newcard_count = 0;
        
        if(data.card_num.length == 1) {
            if(data.card_num[0] == Global.NUM_KNOCK) {
                
                if(room.pickup_2 === 1) 
                    newcard_count = 2;
                else if(room.pickup_4 === 1)
                    newcard_count = 4;
                else if(room.pickup_6 === 1)
                    newcard_count = 6;
                else if(room.pickup_8 === 1)
                    newcard_count = 8;
                else if(room.pickup_5 === 1)
                    newcard_count = 5;
                else if(room.pickup_10 === 1)
                    newcard_count = 10;
                
                if(newcard_count != 0) {
                    deal_newcard = true;
                    var real_newcards = 0;
                    for(index = 0; index < newcard_count; index++) {
                        var new_card = room.remain_cards.splice(room.remain_cards.length - 1, 1);
                        cur_player.cards.push(new_card[0]);
                        real_newcards++;
                        if((room.pickup_5 === 1 || room.pickup_10 === 1) && (new_card[0] == 23 || new_card[0] == 36))
                            break;
                    }
                    newcard_count = real_newcards;
                }
                else {
                    cur_player.cards.push(Global.NUM_KNOCK);
                    if(isAllKnocked(room)) {
                        deal_newcard = addNewCards(room);
                    }
                }
                
                room.pickup_2 = 0;
                room.pickup_4 = 0;
                room.pickup_6 = 0;
                room.pickup_8 = 0;
                room.pickup_5 = 0;
                room.pickup_10 = 0;
            }
            else {
                room.deck_cards.push(data.card_num[0]);
                if(room.deck_cards.length > 1 && (room.deck_cards[room.deck_cards.length - 1] == 10 || room.deck_cards[room.deck_cards.length - 1] == 49)) {
                    
                    room.pickup_2 = 0;
                    room.pickup_4 = 0;
                    room.pickup_6 = 0;
                    room.pickup_8 = 0;
                    
                    if(room.pickup_5 === 1) {
                        room.pickup_5 = 0;
                        room.pickup_10 = 1;
                    }
                    else {
                        room.pickup_5 = 1;
                        room.pickup_10 = 0;
                    }
                }
                else if(room.deck_cards.length > 1 && (room.deck_cards[room.deck_cards.length - 1] == 23 || room.deck_cards[room.deck_cards.length - 1] == 36)) {
                    room.pickup_5 = 0;
                    room.pickup_10 = 0;
                }
                else if(room.deck_cards.length > 1 && (room.deck_cards[room.deck_cards.length - 1] == 1 || room.deck_cards[room.deck_cards.length - 1] == 14 ||
                                                        room.deck_cards[room.deck_cards.length - 1] == 27 || room.deck_cards[room.deck_cards.length - 1] == 40)) {
                    
                    room.pickup_5 = 0;
                    room.pickup_10 = 0;
                    
                    if(room.pickup_2 === 1) {
                        room.pickup_2 = 0;
                        room.pickup_4 = 1;
                        room.pickup_6 = 0;
                        room.pickup_8 = 0;
                    }
                    else if(room.pickup_4 === 1) {
                        room.pickup_2 = 0;
                        room.pickup_4 = 0;
                        room.pickup_6 = 1;
                        room.pickup_8 = 0;
                    }
                    else if(room.pickup_6 === 1) {
                        room.pickup_2 = 0;
                        room.pickup_4 = 0;
                        room.pickup_6 = 0;
                        room.pickup_8 = 1;
                    }
                    else {
                        room.pickup_2 = 1;
                        room.pickup_4 = 0;
                        room.pickup_6 = 0;
                        room.pickup_8 = 0;
                    }
                }
                else {
                    room.pickup_2 = 0;
                    room.pickup_4 = 0;
                    room.pickup_6 = 0;
                    room.pickup_8 = 0;
                    room.pickup_5 = 0;
                    room.pickup_10 = 0;
                }
            
                for(index = 0; index < room.players.length; index++) {
                    if(room.players[index].cards[room.players[index].cards.length - 1] == Global.NUM_KNOCK)
                        room.players[index].cards.splice(room.players[index].cards.length - 1, 1);
                }

                for(index = 0; index < cur_player.cards.length; index++) {
                    if(cur_player.cards[index] == data.card_num[0]) {
                        cur_player.cards.splice(index, 1);
                        break;
                    }
                }
            }
        }
        else {
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].cards[room.players[index].cards.length - 1] == Global.NUM_KNOCK)
                    room.players[index].cards.splice(room.players[index].cards.length - 1, 1);
            }
            
            for(index = 0; index < data.card_num.length; index++) {
                room.deck_cards.push(data.card_num[index]);
            
                for(var index1 = 0; index1 < cur_player.cards.length; index1++) {
                    if(cur_player.cards[index1] == data.card_num[index]) {
                        cur_player.cards.splice(index1, 1);
                        break;
                    }
                }
            }
            
            room.pickup_2 = 0;
            room.pickup_4 = 0;
            room.pickup_6 = 0;
            room.pickup_8 = 0;
            room.pickup_5 = 0;
            room.pickup_10 = 0;
        }
        
        var prev_tune = room.cur_position;
        
        if(data.card_num[data.card_num.length - 1] == 7 || data.card_num[data.card_num.length - 1] == 20 ||
          data.card_num[data.card_num.length - 1] == 33 || data.card_num[data.card_num.length - 1] == 46) {
            room.cur_position = prev_tune;
        }
        else if(data.card_num[data.card_num.length - 1] == 9 || data.card_num[data.card_num.length - 1] == 22 ||
          data.card_num[data.card_num.length - 1] == 35 || data.card_num[data.card_num.length - 1] == 48) {
            var gamerCount = 0;
            for(index = 0; index < room.players.length; index++) {
                if(room.players[index].ingame == 1)
                    gamerCount ++;
            }
            
            if(gamerCount == 2)
                room.cur_position = prev_tune;
            else {
                room.direction = -room.direction;
                room.cur_position = getNextTune(room, prev_tune);
            }
        }
        else {
            room.cur_position = getNextTune(room, prev_tune);
        }
        
        room.save(function(err, updatedRoom) {
            if(err) {
                console.log(err);
                return;
            }
            
            var pick_state = "";
            if(updatedRoom.pickup_2 === 1) 
                pick_state = "pickup2";
            else if(updatedRoom.pickup_4 === 1)
                pick_state = "pickup4";
            else if(updatedRoom.pickup_6 === 1)
                pick_state = "pickup6";
            else if(updatedRoom.pickup_8 === 1)
                pick_state = "pickup8";
            else if(updatedRoom.pickup_5 === 1)
                pick_state = "pickup5";
            else if(updatedRoom.pickup_10 === 1)
                pick_state = "pickup10";
            
            MongoManager.notifyPutCard(updatedRoom, prev_tune, data.card_num, data.card_flower, deal_newcard, newcard_count, pick_state);
            if(cur_player.cards.length == 0)
                completeGame(true, data.user_id, updatedRoom);
        });
    });
}

function procRestartGame(data, callback) {
    RoomCollection.findOne({_id : data.room_id}, function(err, room) {
        if(err)
            return;
        
        if(room == null)
            return;
        
        if(room.started == 1)
            return;
        
        for(var index = 0; index < room.players.length; index++) {
            if(room.players[index].player_id == data.user_id && room.players[index].joined == 1) {
                room.players[index].start_req = 1;
                break;
            }
        }
        
        room.save(function(err, updatedRoom) {
            if(err) {
                console.log(err);
                return;
            }
            
            callback(updatedRoom);
        });
    });
}

function generateRandomNumber(minValue, maxValue) {
    var random_number = Math.random() * maxValue + minValue;
    return Math.floor(random_number);
}

function compare(a, b) {
    if(a.position < b.position)
        return -1;
    if(a.position > b.position)
        return 1;
    return 0;
}

function findPlayer(room, tune) {
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].position == tune)
            return room.players[index];
    }
    
    return null;
}

function getNextTune(room, base_tune) {
    var turn = base_tune;
    var bTurnProcessed = false;
    while(!bTurnProcessed) {
        turn = turn + room.direction;
        if(turn == room.max_member)
            turn = 0;
        else if(turn == -1)
            turn = room.max_member;

        for(var index = 0; index < room.players.length; index++) {
            if(turn == room.players[index].position && room.players[index].ingame == 1) {
                bTurnProcessed = true;
                break;
            }
        }
    }
    return turn;
}

function isAllKnocked(room) {
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].ingame == 1 && room.players[index].cards[room.players[index].cards.length - 1] != Global.NUM_KNOCK)
            return false;
    }
    
    return true;
}

function addNewCards(room) {
    
    for(var index = 0; index < room.players.length; index++) {
        if(room.players[index].ingame == 1 && room.players[index].cards[room.players[index].cards.length - 1] == Global.NUM_KNOCK)
            room.players[index].cards.splice(room.players[index].cards.length - 1, 1);
    }
    
    var tune = getNextTune(room, room.cur_position);
    while(tune != room.cur_position) {
        var player = findPlayer(room, tune);
        var new_card = room.remain_cards.splice(room.remain_cards.length - 1, 1);
        player.cards.push(new_card[0]);
        tune = getNextTune(room, tune);
    }
    
    var player1 = findPlayer(room, tune);
    var new_card1 = room.remain_cards.splice(room.remain_cards.length - 1, 1);
    player1.cards.push(new_card1[0]);
    return true;
}

function findPlayerRoom(data, callback) {
    
    var response_json   = {
        'success': Global.RES_FAILED,
        'room' : null
    };
    
    RoomCollection.findOne({$or : [{"players.player_id" : data.friend_id}, {"players.player_id" : data.user_id}]}, function(err, room) {
        if(err) {
            console.log(err);
            callback(response_json);
            return;
        } 
        
        if(room == null) {
            response_json.success = Global.RES_SUCCESS;
            callback(response_json);
            return;
        }
        
        response_json.room = room;
        callback(response_json);
    });
}

function deletePlayerRoom(user_id, callback) {
    var response_json   = {
        'success': Global.RES_FAILED,
    };
    
    RoomCollection.deleteMany({"players.player_id" : user_id, type : Global.MAX_MEMBER_ROOM_FRIEND}, function(err, res) {
        if(err) {
            console.log(err);
            callback(response_json);
            return;
        } 
        
        response_json.success = Global.RES_SUCCESS;
        callback(response_json);
    });
}

exports.init        = init;
exports.createRoom  = createRoom;
exports.findRoom    = findRoom;
exports.addPlayerToRoom = addPlayerToRoom;
exports.joinPlayer  = joinPlayer;
exports.leavePlayer = leavePlayer;
exports.closeMatch  = closeMatch;
exports.startGame   = startGame;
exports.procPutCard = procPutCard;
exports.procRestartGame = procRestartGame;
exports.findPlayerRoom = findPlayerRoom;
exports.deletePlayerRoom = deletePlayerRoom;
exports.findRoomWithId = findRoomWithId;