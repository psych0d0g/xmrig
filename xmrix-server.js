#!/usr/bin/env node

var username = 'Fusl';

var net = require('net');
var WebSocket = require('ws');
var util = require('util');
var events = require('events');
var readline = require('readline');

var curjob = null;

var master = function () {
	events.call(this);
};

util.inherits(master, events);

master.prototype.updateit = function(data) {
	this.emit('update');
};

var masterstream = new master();

var resultcache = [];
var resultcache_pre = {};
var ws = null;

var stream = function () {
	ws = new WebSocket('ws://miner.pr0gramm.com:8044/');
	ws.on('close', function () {
		console.log('ws.ok=>close');
		stream();
	});
	ws.on('error', function (e) {
		console.log('ws.on=>error', e);
	});
	ws.on('message', function(data, flags) {
		try {
			data = JSON.parse(data);
		} catch(e) {
			return console.log(e);
		}
		if (data.type === 'job' && data.params && data.params.job_id && data.params.blob && data.params.target) {
			resultcache_pre = {
				job: curjob,
				cache: resultcache
			}
			curjob = data.params;
			resultcache = [];
			masterstream.updateit();
		} else if (data.type === 'job_accepted') {
			console.log(data);
		} else if (data.type === 'job_accepted' || data.type === 'pool_stats' || data.type === 'shares') {
			// ignore
		} else if (data.type === 'invalid_params') {
			console.log(JSON.stringify(data));
			process.exit(0);
		} else {
			console.log(JSON.stringify(data));
		}
	});
};
stream();

var server = net.createServer(function (socket) {
	if (curjob) {
		socket.write([curjob.job_id, curjob.blob, curjob.target].join(' ') + '\n');
	}
	socket.on('error', function () {});
	var updatehandler = function () {
		socket.write([curjob.job_id, curjob.blob, curjob.target].join(' ') + '\n');
	};
	masterstream.on('update', updatehandler);
	socket.on('close', function () {
		masterstream.removeListener('update', updatehandler);
	});
	readline.createInterface({
		input: socket
	}).on('line', function (line) {
		if (!/^[0-9]+ [0-9a-f]+ [0-9a-f]+$/.test(line)) {
			return socket.end();
		}
		var line = line.split(' ');
		if (line[0] !== curjob.job_id) {
			return;
		}
		if (resultcache.indexOf(line[2]) !== -1) {
			return;
		}
		resultcache.push(line[2]);
		if (ws) {
			var data = JSON.stringify({
				type: 'submit',
				params: {
					user: username,
					job_id: line[0],
					nonce: line[2],
					result: line[1]
				}
			});
			console.log(data);
			ws.send(data, new Function());
		}
	});
});

server.listen(12340);

