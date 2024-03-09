#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection

argvs = sys.argv
argc = len(argvs)

if(argc != 2):
	print("Usage: # python %s filename" % argvs[0])
	print(argvs)
	sys.exit(0)

print(argvs[1])

f = open(argvs[1], 'r')
json_dict = json.load(f)
f.close

json_str = json.dumps(json_dict)

print(json_str)

ws = create_connection("ws://localhost:5100", subprotocols=["quote"])

print("create_connected")

ws.send(json_str)

print("sent!")

result = ws.recv()

print("Received '%s'" % result)

ws.close()

print("close!")




