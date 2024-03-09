#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
from time import sleep
from threading import Thread
from multiprocessing import Pool

def do_case(uid):
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
					"maturity":"2019-09-30"
				},
				{
					"ls":"S",
					"cp":"C",
					"quantity":1000,
					"maturity":"2019-09-30"
				},
			],
		}
	}

	logout = {
		"header":{
			"type":"action",
			"action_name":"logout"
		},
		"body":{
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

	sesId = jres["body"]["sessionId"]
	print(sesId)

	print(rfq)

	rfq["header"]["sessionId"] = sesId

	print(rfq)

	json_str = json.dumps(rfq)
	ws.send(json_str)
	print("sent!")

	result = ws.recv()

	print("Received '%s'" % result)

	logout["header"]["sessionId"] = sesId

	json_str = json.dumps(logout)

	ws.send(json_str)
	print("sent!")

	result = ws.recv()

	print("Received '%s'" % result)

	sleep(1)

	ws.close()
	print("close!")

if __name__ == '__main__':
	p = Pool(processes=100, )

	xs = p.map(do_case, range(100))


