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

print("Received[login] '%s'" % result)

aa = json.loads(result)

sessionId = aa["header"]["sessionId"];

print("sessionId '%s'" % aa["header"]["sessionId"])

########################################
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

print("Received[get_symbol_list] '%s'" % result)

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

print("Received[get_my_conversations] '%s'" % result)

########################################
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

print("Received[get_price_list] '%s'" % result)

########################################
msgRfq = {
	"header":{
		"type":"action",
		"action_name":"send_rfq"
	},
	"body":{
		"conversation":{
			"status":"RFQ",
			"tradeDate":"2018-10-10",
			"settlementDate":"2018-10-12",
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
					"maturity":"2018-10-30",
					"volatility":0.23,
				},
				{
					"id":1,
					"ls":"L",
					"cp":"C",
					"quantity":100,
					"strike":101,
					"maturity":"2018-10-30",
					"volatility":0.25,
				},
			]
		}
	}
}
msgRfq["header"]["sessionId"] = sessionId;

jsonRfq = json.dumps(msgRfq)

print("jsonRfq '%s'" % jsonRfq)

ws.send(jsonRfq)
print("RFQ sent!")

result = ws.recv()
print("Received[send_rfq] '%s'" % result)

result = ws.recv()
print("Received[send_rfq] '%s'" % result)

aa = json.loads(result)

conv_id = aa["body"]["conversation"]["id"];

print("conversation.id:'%s'" % conv_id)


########################################
msgTake = {
	"header":{
		"type":"action",
		"action_name":"take_ownership"
	},
	"body":{
		"conversation":{}
	}
}
msgTake["header"]["sessionId"] = sessionId;
msgTake["body"]["conversation"]["id"] = conv_id;

jsonTake = json.dumps(msgTake)

print("jsonTake '%s'" % jsonTake)

ws.send(jsonTake)
print("sent!")

result = ws.recv()

print("Received[take_ownership] '%s'" % result)


########################################
msgCalc = {
	"header":{
		"type":"action",
		"action_name":"calc_option"
	},
	"body":{
		"conversation":{
		},
	},
}

msgCalc["header"]["sessionId"] = sessionId;
msgCalc["body"]["conversation"]["id"] = conv_id;

jsonCalc = json.dumps(msgCalc)

print("jsonCalc '%s'" % jsonCalc)

ws.send(jsonCalc)
print("sent!")

result = ws.recv()

print("Received[calc_option] '%s'" % result)

msgQuote = json.loads(result)

########################################
msgQuote["header"]["sessionId"] = sessionId;
msgQuote["header"]["type"] = "action";
msgQuote["header"]["action_name"] = "quote";

jsonQuote = json.dumps(msgQuote)

print("jsonQuote '%s'" % jsonQuote)

ws.send(jsonQuote)
print("sent!")

result = ws.recv()

print("Received[quote] '%s'" % result)

########################################

msgOrder = {
	"header":{
		"type":"action",
		"action_name":"order"
	},
	"body":{
		"conversation":{}
	}
}
msgOrder["header"]["sessionId"] = sessionId;
msgOrder["body"]["conversation"]["id"] = conv_id;

jsonOrder = json.dumps(msgOrder)

print("jsonOrder '%s'" % jsonOrder)

ws.send(jsonOrder)
print("sent!")

result = ws.recv()

print("Received[order] '%s'" % result)

########################################

msgExecution = {
	"header":{
		"type":"action",
		"action_name":"execute"
	},
	"body":{
		"conversation":{}
	}
}
msgExecution["header"]["sessionId"] = sessionId;
msgExecution["body"]["conversation"]["id"] = conv_id;

jsonExecution = json.dumps(msgExecution)

print("jsonExecution '%s'" % jsonExecution)

ws.send(jsonExecution)
print("sent!")

result = ws.recv()

print("Received[execute] '%s'" % result)

########################################

ws.close()
print("close!")



