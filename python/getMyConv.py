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
		"userId":"client",
		"passwd":"hello",
		"role":"client",
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

########################################
msgGetMyConv = {
	"header":{
		"type":"action",
		"action_name":"get_my_conversations"
	},
	"body":{
	}
}
msgGetMyConv["header"]["sessionId"] = sessionId;

json_getMyConv = json.dumps(msgGetMyConv)

print("json_getMyConv '%s'" % json_getMyConv)

ws.send(json_getMyConv)
print("sent!")

result = ws.recv()

print("Received '%s'" % result)

########################################

ws.close()
print("close!")



