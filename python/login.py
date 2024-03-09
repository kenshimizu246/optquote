#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
#import websocket

msg = {
	"header":{
		"type":"action",
		"action_name":"login"
	},
	"body":{
		"userId":"trader",
		"passwd":"hello",
		"role":"trader",
	}
}

json_str = json.dumps(msg)
print(json_str)

print("create_connection...")
ws = create_connection("ws://localhost:5100", subprotocols=["quote"])
print("create_connection4")

ws.send(json_str)
print("sent!")

result = ws.recv()

print("Received '%s'" % result)

aa = json.loads(result)

sessionId = aa["header"]["sessionId"];

print("sessionId '%s'" % aa["header"]["sessionId"])

msgGetSymbol = {
	"header":{
		"type":"action",
		"action_name":"get_symbol_list"
	},
	"body":{
	}
}
msgGetSymbol["header"]["sessionId"] = sessionId;

json_getSymbol = json.dumps(msgGetSymbol)

print("json_getSymbol '%s'" % json_getSymbol)

ws.send(json_getSymbol)
print("sent!")

result = ws.recv()

print("Received '%s'" % result)


msgGetPrice = {
	"header":{
		"type":"action",
		"action_name":"get_price_list"
	},
	"body":{
	}
}
msgGetPrice["header"]["sessionId"] = sessionId;

json_getPrice = json.dumps(msgGetPrice)

print("json_getPrice '%s'" % json_getPrice)

ws.send(json_getPrice)
print("sent!")

result = ws.recv()

print("Received '%s'" % result)


ws.close()
print("close!")




