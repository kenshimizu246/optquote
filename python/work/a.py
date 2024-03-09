#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
from time import sleep


def do_case(uid):
	print(uid)

	login = {
		"header":{
			"type":"action",
			"action_name":"login"
		},
		"body":{
			"userId":100,
			"name":"hello",
			"passwd":"hello",
			"role":"trader",
		}
	}

	login["body"]["userId"] = uid

	json_str = json.dumps(login)
	print(json_str)

	print("create_connection...")
	ws = create_connection("ws://localhost:5100", subprotocols=["quote"])
	print("create_connection4")

	ws.send(json_str)
	print("sent!")

	result = ws.recv()

	print("Received '%s'" % result)

	jres = json.loads(result)

	rfq = {
		"header":{
			"type":"action",
			"action_name":"send_rfq",
		},
		"body":{
			"symbol":"AAPL",
			"settlement":"T+1",
			"side":"B",
			"legs":[
				{
					"ls":"L",
					"cp":"P",
					"quantity":1000,
				},
				{
					"ls":"S",
					"cp":"C",
					"quantity":1000,
				},
			],
		}
	}

	sleep(1)

	ws.close()
	print("close!")

if __name__ == '__main__':
	for i in [1,2,3]:
		do_case(i)


