#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
#import websocket

print("Hello world!");

argvs = sys.argv
argc = len(argvs)


if(argc != 2):
	print("Usage: # python %s filename" % argvs[0])
	print(argvs)
	print(argc)
	sys.exit(0)

print(argvs[1])
#print '\n'.join(sys.path)

f = open('test.json', 'r')
json_dict = json.load(f)
f.close

json_str = json.dumps(json_dict)
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




