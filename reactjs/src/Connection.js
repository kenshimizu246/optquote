import EventEmitter from "events";
import MessageFactory from './MessageFactory.js';

export default class Connection {
	constructor(url, protocols) {
		this.url = url;
		this.protocols = protocols;
		this.emitter = new EventEmitter();
		this.errorCount = 0;
		this.maxErrorCount = 0;
		setInterval(this._heartbeat.bind(this),1000*10);
	}

	send(msg){
		if(this.connection && this.connection.readyState == WebSocket.OPEN){
			this.connection.send(msg);
			console.log("send:"+msg);
		}else{
			alert("connection is not open:" + this.connection.readyState);
		}
	}

	_heartbeat() {
		if(this.connection && this.connection.readyState == WebSocket.OPEN){
			var msg = MessageFactory.createHeatbeat();
			this.send(msg);
		}
	}

	on(eventName, listener){
		this.emitter.on(eventName, listener);
	}

	removeEventListener(eventName, listener){
		this.emitter.removeListener(eventName, listener);
	}

	removeAllEventListeners(eventName){
		var lls = this.emitter.listeners(eventName);
		for(var i = 0; i < lls.length; i++){
			this.emitter.removeListener(eventName, lls[i]);
		}
	}

	_onopen() {
		this.errorCount = 0;
		this.emitter.emit('SYS', 'ONOPEN');
	}

	_onerror(error) {
		var ss = JSON.stringify(error);
		console.log("error:"+ss);
		this.errorCount++;
		this.emitter.emit('SYS', 'ERRROR:' + ss);
		if(this.connection.readyState === WebSocket.CLOSED && this.errorCount < this.maxErrorCount){
			this.connect();
		}
	}

	_onmessage(e) {
console.log("connector._onmessage:"+e.data);
		var obj = JSON.parse(e.data);
		this.emitter.emit('MSG', obj);
	}

	_onclose(e) {
		var ss = JSON.stringify(e);
		console.log("onclose:"+ss);
		this.emitter.emit('SYS', 'ONCLOSE:' + ss);
		//setTimeout(this.connect.bind(this), 1000);
		this.connect();
	}

	connect() {
	  //this.connection = new WebSocket('ws://'+ window.location.hostname + '/ws2', ['quote']);
	  this.connection = new WebSocket(this.url, this.protocols);

		console.log("connect():"+this.url);

	  this.connection.onopen = this._onopen.bind(this);
	  this.connection.onerror = this._onerror.bind(this);
	  this.connection.onmessage = this._onmessage.bind(this);
	  this.connection.onclose = this._onclose.bind(this);
	}

	close(){
		this.connection.close();
	}

	// 0 CONNECTING
	// 1 OPEN
	// 2 CLOSING
	// 3 CLOSED
	readyState(){
		return this.connection.readyState;
	}
}

