#!/usr/bin/python

import sys
import json
import logging
#from websocket import create_connection
import websocket
import time

msgLogin = {
	"header":{
		"type":"action",
		"action_name":"login"
	},
	"body":{
		"userId":"client",
		"passwd":"hello",
		"role":"client",
	}
}

# {"header":{"type":"action","action_name":"send_rfq","sessionId":"98abeabc-e945-4fc9-bc30-aea5170e3702"},"body":{"conversation":{"status":"RFQ","tradeDate":"2019-02-11T21:39:33.449Z","settlementDate":"2019-02-13T21:39:24.476Z","side":"S","symbol":"AAPL","spotPrice":216.16,"riskFreeRate":0.01,"dividendYield":0.03,"strategy":"straddle","oc":"Open","link_id":0,"legs":[{"id":0,"ls":"Long","cp":"Call","quantity":100,"strike":216.16,"maturity":"2019-02-27T21:39:24.000Z","volatility":0,"premium":0,"delta":0,"gamma":0,"vega":0,"theta":0,"rho":0},{"id":1,"ls":"Long","cp":"Put","quantity":100,"strike":216.16,"maturity":"2019-02-27T21:39:24.000Z","volatility":0,"premium":0,"delta":0,"gamma":0,"vega":0,"theta":0,"rho":0}]}}}
:wq

# 



try:
	import thread
except ImportError:
	import _thread as thread


def on_message(ws, message):
	print(message);
	jmsg = json.loads(message);
	if(jmsg["header"]["type"] == "login_result"):
		sessionId = jmsg["header"]["sessionId"];
	

def on_error(ws, error):
	print(error)

def on_close(ws):
	print("### closed ###")

def on_open(ws):
	json_str = json.dumps(msgLogin);
	ws.send(json_str);

#	def run(*args):
#	thread.start_new_thread(run, ())

if __name__ == '__main__':
	print("main...");
	websocket.enableTrace(True)
	ws = websocket.WebSocketApp("ws://localhost:5100",
															subprotocols=["quote"],
															on_message = on_message,
															on_error = on_error,
															on_close = on_close)
	ws.on_open = on_open
	ws.run_forever()

