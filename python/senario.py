#!/usr/bin/python

import sys
import json
import logging
from websocket import create_connection
#import websocket

print("\n\n########################################")

msgLogin = {
	"header":{
		"type":"action",
		"action_name":"login"
	},
	"body":{
	}
}

msgLogin["body"]["userId"] = "client";
msgLogin["body"]["passwd"] = "hello";
msgLogin["body"]["role"] = "client";

json_login = json.dumps(msgLogin)
print(json_login)

print("create client connection...")
wsc = create_connection("ws://localhost:5100", subprotocols=["quote"])

wsc.send(json_login)
print("sent client login!")

result = wsc.recv()

print("Received[client login] '%s'" % result)

aa = json.loads(result)

sidClient = aa["header"]["sessionId"];

print("sessionId[client] '%s'" % aa["header"]["sessionId"])

print("\n\n########################################")

msgLogin["body"]["userId"] = "trader";
msgLogin["body"]["passwd"] = "hello";
msgLogin["body"]["role"] = "trader";

json_login = json.dumps(msgLogin)
print(json_login)

print("create trader connection...")
wst = create_connection("ws://localhost:5100", subprotocols=["quote"])

wst.send(json_login)
print("sent trader!")

result = wst.recv()

print("Received[trader login] '%s'" % result)

aa = json.loads(result)

sidTrader = aa["header"]["sessionId"];

print("sessionId[trader] '%s'" % aa["header"]["sessionId"])

print("\n\n########################################")

msgGetSymbol = {
	"header":{
		"type":"action",
		"action_name":"get_symbol_list"
	},
	"body":{
	}
}
msgGetSymbol["header"]["sessionId"] = sidClient;

json_getSymbol = json.dumps(msgGetSymbol)

print("json_getSymbol '%s'" % json_getSymbol)

wsc.send(json_getSymbol)
print("sent!")

result = wsc.recv()

print("Received[get_symbol_list] '%s'" % result)

print("\n\n########################################")

msgGetMyConv = {
	"header":{
		"type":"action",
		"action_name":"get_my_conversations"
	},
	"body":{
	}
}
msgGetMyConv["header"]["sessionId"] = sidClient;

json_getMyConv = json.dumps(msgGetMyConv)

print("json_getMyConv '%s'" % json_getMyConv)

wsc.send(json_getMyConv)
print("sent!")

result = wsc.recv()

print("Received[get_my_conversations] '%s'" % result)

print("\n\n########################################")

msgGetPrice = {
	"header":{
		"type":"action",
		"action_name":"get_price_list"
	},
	"body":{
	}
}
msgGetPrice["header"]["sessionId"] = sidClient;

json_getPrice = json.dumps(msgGetPrice)

print("json_getPrice '%s'" % json_getPrice)

wsc.send(json_getPrice)
print("sent!")

result = wsc.recv()

print("Received[get_price_list] '%s'" % result)

print("\n\n########################################")

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
msgRfq["header"]["sessionId"] = sidClient;

jsonRfq = json.dumps(msgRfq)

print("jsonRfq '%s'" % jsonRfq)

wsc.send(jsonRfq)
print("RFQ sent!")

result = wsc.recv()
print("Received-1[send_rfq.client] '%s'" % result)

result = wsc.recv()
print("Received-2[send_rfq.client] '%s'" % result)

aa = json.loads(result)

conv_id = aa["body"]["conversation"]["id"];

print("conversation.id:'%s'" % conv_id)


print("\n\n########################################")

result = wst.recv()
print("Received[send_rfq.trader] '%s'" % result)

print("\n\n########################################")

msgTake = {
	"header":{
		"type":"action",
		"action_name":"take_ownership"
	},
	"body":{
		"conversation":{}
	}
}
msgTake["header"]["sessionId"] = sidTrader;
msgTake["body"]["conversation"]["id"] = conv_id;

jsonTake = json.dumps(msgTake)

print("jsonTake '%s'" % jsonTake)

wst.send(jsonTake)
print("sent!")

result = wst.recv()

print("Received[take_ownership] '%s'" % result)

print("\n\n########################################")

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

msgCalc["header"]["sessionId"] = sidTrader;
msgCalc["body"]["conversation"]["id"] = conv_id;

jsonCalc = json.dumps(msgCalc)

print("jsonCalc '%s'" % jsonCalc)

wst.send(jsonCalc)
print("sent!")

result = wst.recv()

print("Received[calc_option] '%s'" % result)

msgQuote = json.loads(result)

print("\n\n########################################")

msgQuote["header"]["sessionId"] = sidTrader;
msgQuote["header"]["type"] = "action";
msgQuote["header"]["action_name"] = "quote";

jsonQuote = json.dumps(msgQuote)

print("jsonQuote '%s'" % jsonQuote)

wst.send(jsonQuote)
print("sent!")

result = wst.recv()

print("Received[quote] '%s'" % result)

print("\n\n########################################")

msgOrder = {
	"header":{
		"type":"action",
		"action_name":"order"
	},
	"body":{
		"conversation":{}
	}
}
msgOrder["header"]["sessionId"] = sidClient;
msgOrder["body"]["conversation"]["id"] = conv_id;

jsonOrder = json.dumps(msgOrder)

print("jsonOrder '%s'" % jsonOrder)

wsc.send(jsonOrder)
print("sent!")

result = wsc.recv()

print("Received[order] '%s'" % result)

print("\n\n########################################")

msgExecution = {
	"header":{
		"type":"action",
		"action_name":"execute"
	},
	"body":{
		"conversation":{}
	}
}
msgExecution["header"]["sessionId"] = sidTrader;
msgExecution["body"]["conversation"]["id"] = conv_id;

jsonExecution = json.dumps(msgExecution)

print("jsonExecution '%s'" % jsonExecution)

wst.send(jsonExecution)
print("sent!")

result = wst.recv()

print("Received[execute] '%s'" % result)

print("\n\n########################################")

wsc.close()
wst.close()
print("close!")



