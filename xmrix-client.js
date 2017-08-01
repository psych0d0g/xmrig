#!/usr/bin/env node

var net = require('net');
var readline = require('readline');
var child_process = require('child_process');
var os = require('os');

var threads = [];
var gsock = null;
var curjob = {
	job_id: null,
	blob: null,
	target: null
};

var parseresult = function (line) {
	if (!/^[0-9a-f]+ [0-9a-f]+$/.test(line)) {
		return;
	}
	line = line.split(' ');
	if (gsock) {
		console.log([curjob.job_id, line[1], line[0]].join(' '));
		gsock.write([curjob.job_id, line[1], line[0]].join(' ') + '\n');
	}
};

var restart = function (job_id, blob, target) {
	if (curjob.job_id === job_id && curjob.blob === blob && curjob.target === target) {
		return;
	}
	curjob = {
		job_id: job_id,
		blob: blob,
		target: target
	};
	console.log({
		job_id: job_id,
		blob: blob,
		target: target
	});
	threads.forEach(function (thread) {
		thread.kill('SIGKILL');
	});
	threads = [];
	var cpus = Math.ceil(os.cpus().length / 2);
	for (var i = 0; i < cpus; i++) {
		threads[i] = child_process.spawn('./xmrix', [blob, target]);
		threads[i].stderr.resume();
		readline.createInterface({
			input: threads[i].stdout
		}).on('line', parseresult);
	}
};

var connect = function () {
	var socket = new net.Socket();
	socket.connect(12340, '127.0.0.1', function () {
		gsock = socket;
		console.log('Connected');
		readline.createInterface({
			input: socket
		}).on('line', function (line) {
			if (!/^[0-9]+ [0-9a-f]+ [0-9a-f]+$/.test(line)) {
				return;
			}
			line = line.split(' ');
			restart(line[0], line[1], line[2]);
		});
	});
	socket.on('error', function() {});
	socket.on('close', function() {
		gsock = null;
		connect();
	});
};
connect();