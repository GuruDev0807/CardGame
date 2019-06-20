
var getApp = function (config) {
	
	var express         = require('express');
	var	app             = express();
	var favicon         = require('serve-favicon');
	var logger          = require('morgan');
	var cookieParser    = require('cookie-parser');
	var bodyParser      = require('body-parser');
	var methodOverride  = require('method-override');
	var session         = require('express-session');
	var errorHandler    = require('errorhandler');
    var upload          = require('express-fileupload');

	app.set('port', process.env.PORT || config.server.port);
	app.use(logger('dev'));
	app.use(bodyParser.json({limit: '5mb'}));
	app.use(methodOverride());
	app.use(cookieParser());
	app.use(session({ secret: 'secret', resave: true, saveUninitialized: true }));
	
    if (app.get('env') === 'development')
	  app.use(errorHandler({ dumpExceptions: true, showStack: true }));

	if (app.get('env') === 'production')
	  app.use(errorHandler());

    app.use(upload());

    app.post("/", function(req, res) {
        if(req.files) {
            var file = req.files.filename;
            var filename = file.name;
            file.mv('./upload/' + filename, function(err) {
                if(err)
                    res.send("failed");
                else
                    res.send(filename);
            });
        }
        else {
            console.log("there is no file data");
        }
    });
	
	return app;
};

exports.getApp = getApp;
