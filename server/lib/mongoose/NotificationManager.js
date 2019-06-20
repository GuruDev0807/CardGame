var Global          = require('../Global');
var MongoManager    = require('../MongoManager');

var NotificationCollection 	= null;

function init(mongoose) {
    
	var notificationSchema = new mongoose.Schema( {
        type            : Number,
        sender          : String,
        receiver        : String,
        receive_action  : Number
	});

    NotificationCollection = mongoose.model('notification', notificationSchema);
}

function addFriendRequest(data, callback) {
    NotificationCollection.findOne({type : Global.EMAIL_TYPE_ADD_FRIEND, receive_action : Global.EMAIL_ACTION_NONE, $or : [{sender : data.user_id, receiver : {$in : data.friends}}, {receiver : data.user_id, sender : {$in : data.friends}}]}, function(err, post) {
		if(err) {
			console.log(err);
            return;
		}
        
        if(post != null) {
            var response_json = {
                "status" : Global.RES_FAILED    
            };
            callback(response_json);
            return;
        }
        
        saveNewNotification(0, Global.EMAIL_TYPE_ADD_FRIEND, data.user_id, data.friends, Global.EMAIL_ACTION_NONE, callback);
	});    
}

function notificationList(data, callback) {
    NotificationCollection.find({receiver : data.user_id}, function(err, docs) {
        if(err) {
            console.log(err);
            return;
        }
        
        if(docs == null)
            return;
        
        callback(docs);
    });
}

function acceptFriendRequest(data, callback) {
    NotificationCollection.findOne({_id : data._id}, function(err, post) {
        if(err) {
            console.log(err);
            return;
        }
        
        if(post == null)
            return;
        
        if(post.receiver != data.user_id)
            return;
        
        var response_json = {
            "success" : Global.RES_FAILED,
            "_id" : "",
            "action" : Global.EMAIL_ACTION_NONE
        };
        
        post.receive_action = data.action;
        post.save(function(err) {
            if(err) {
                console.log(err);
            }
            else {
                response_json.success = Global.RES_SUCCESS;
                response_json._id = post._id;
                response_json.action = data.action;
                callback(response_json);
                
                if(data.action == Global.EMAIL_ACTION_ACCEPT) {
                    var friendData = [
                      {"user" : post.sender, "friend" : post.receiver},
                      {"user" : post.receiver, "friend" : post.sender}
                    ];
                    MongoManager.addFriend(friendData);
                }

                var newNotificationCollection = new NotificationCollection();
                newNotificationCollection.type = (data.action == Global.EMAIL_ACTION_ACCEPT) ? Global.EMAIL_TYPE_ACCEPT_FRIEND : Global.EMAIL_TYPE_DECLINE_FRIEND;
                newNotificationCollection.sender = post.receiver;
                newNotificationCollection.receiver = post.sender;
                newNotificationCollection.receive_action = data.action;

                newNotificationCollection.save(function (err, res) {
                    if(err) {
                        console.log(err);
                        return;
                    }

                    MongoManager.notifyNotification(res._id, res.type, res.sender, res.receiver, res.receive_action);
                });
            }
        });
    });
}
    
function saveNewNotification(index, type, sender, receivers, action, callback) {
    var response_json = {
        "status" : Global.RES_FAILED    
    };
    
    if(index >= receivers.length) {
        response_json.status = Global.RES_SUCCESS;
        callback(response_json);
        return;
    }
    
    var newNotificationCollection = new NotificationCollection();
    newNotificationCollection.type = type;
    newNotificationCollection.sender = sender;
    newNotificationCollection.receiver = receivers[index];
    newNotificationCollection.receive_action = action;

    newNotificationCollection.save(function (err, res) {
        if(err) {
            console.log(err);
            callback(response_json);
            return;
        }
        
        MongoManager.notifyNotification(res._id, type, sender, receivers[index], action);
        saveNewNotification(index+1, type, sender, receivers, action, callback);
    });
}

function removeFriend(sender, receiver, callback) {
    
    var response_json   = {
        'success': Global.RES_FAILED,
    };
    
    var newNotificationCollection = new NotificationCollection();
    newNotificationCollection.type = Global.EMAIL_TYPE_REMOVE_FRIEND;
    newNotificationCollection.sender = sender;
    newNotificationCollection.receiver = receiver;
    newNotificationCollection.receive_action = Global.EMAIL_ACTION_NONE;

    newNotificationCollection.save(function (err, res) {
        if(err) {
            console.log(err);
            callback(response_json);
            return;
        }
        
        response_json.success = Global.RES_SUCCESS;
        callback(response_json);
        
        MongoManager.notifyNotification(res._id, res.type, res.sender, res.receiver, res.receive_action);
    });
}

exports.init                    = init;
exports.addFriendRequest        = addFriendRequest;
exports.notificationList        = notificationList;
exports.acceptFriendRequest     = acceptFriendRequest;
exports.removeFriend            = removeFriend;