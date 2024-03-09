#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
from time import sleep

if __name__ == "__main__":
	login = {
		"header":{
			"type":"action",
			"action_name":"login"
		},
		"body":{
			"userId":"trader",
			"name":"hello",
			"passwd":"hello",
			"role":"trader",
		}
	}
	
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

	rfq = {
		"header":{
			"type":"action",
			"action_name":"send_rfq",
		},
		"body":{
			"conversation":{
				"status":"RFQ",
				"tradeDate":"2018-11-21",
				"settlementDate":"2018-11-21",
				"side":"B",
				"symbol":"AAPL",
				"spotPrice":100.00,
				"riskFreeRate":0.01,
				"dividendYield":0.01,
				"strategy":"STRADDLE",
				"legs":[
					{
						"id":0,
						"ls":"L",
						"cp":"P",
						"quantity":100,
						"strike":101,
						"maturity":"2018-11-21",
					},
					{
						"id":1,
						"ls":"L",
						"cp":"C",
						"quantity":100,
						"strike":101,
						"maturity":"2018-11-21",
					},
				],
			}
		}
	}
	print(rfq)

	rfq["header"]["sessionId"] = sesId

	print(rfq)

	json_str = json.dumps(rfq)
	ws.send(json_str)
	print("sent!")

	result = ws.recv()

	print("Received '%s'" % result)

	#sleep(1)

#	ws.close()
#	print("close!")




