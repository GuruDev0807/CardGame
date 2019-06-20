var config          = require ('./config').values;
var app             = require ('./app').getApp(config);
var NetworkManager  = require ('./lib/NetworkManager');

var srv = NetworkManager.createServer(app);
srv.listen(config.server.port);

console.log("Express server listening on port %d in %s mode", config.server.port, app.settings.env);

process.on('SIGINT', function () {
	console.log();
	console.log('Shutting down server..');
	process.exit(0);
});

process.on('uncaughtException', function(err){
	console.log('Exception: ' + err.stack);
});