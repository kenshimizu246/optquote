#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
#import websocket

msg = {
	"header":{
		"type":"action",
		"action_name":"heartbeat"
	},
	"body":{
		"message":"hello"
	}
}
print(msg)

json_str = json.dumps(msg)
print(json_str)

print("create_connection...")
ws = create_connection("ws://localhost:5100", subprotocols=["quote"])
print("create_connection4")

ws.send(json_str)
print("sent!")

result = ws.recv()

print("Received '%s'" % result)

ws.close()
print("close!")




