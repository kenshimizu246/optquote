import moment from 'moment';

export default class MessageFactory {
	static createLogin(userId, password, role) {
		var hdr = {
				type: 'action',
				action_name: 'login'
			};
		var bdy = {
			userId: userId,
			passwd: password,
			role: role
		};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createReconnect(sessionId, userId, role) {
		var hdr = {
				type: 'action',
				action_name: 'reconnect',
				sessionId: sessionId,
			};
		var bdy = {
			userId: userId,
			role: role
		};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createGetSymbolList(state) {
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'get_symbol_list',
				sessionId: sessionId
			};
		var bdy = {};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createSendRfq(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'send_rfq',
				sessionId: sessionId
			};
		var conversation = {
			status: 'RFQ',
			tradeDate: moment(),
			settlementDate: conv.settlementDate,
			side: conv.side,
			symbol: conv.symbol,
			spotPrice: conv.spotPrice,
			riskFreeRate: conv.riskFreeRate,
			dividendYield: conv.dividendYield,
			strategy: conv.strategy,
			oc: conv.oc,
			link_id: conv.link_id,
			legs: conv.legs,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		//alert("createSendRfq:"+ss);
		return ss;
	}

	static createClosePosition(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var legs = [];
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'send_rfq',
				sessionId: sessionId
			};
		for(var i = 0; i < conv.legs.length; i++){
			var l = {
				id: conv.legs[i].id,
				ls: conv.legs[i].ls,
				cp: conv.legs[i].cp,
				quantity: conv.legs[i].quantity,
				strike: conv.legs[i].strike,
				maturity: conv.legs[i].maturity,
			}
			legs[i] = l;
		}
		var conversation = {
			status: 'RFQ',
			oc: 'Close',
			tradeDate: moment(),
			settlementDate: conv.settlementDate,
			side: (conv.side === "Sell" ? "Buy" : "Sell"),
			symbol: conv.symbol,
			spotPrice: conv.spotPrice, // should be current price
			riskFreeRate: conv.riskFreeRate, // current
			dividendYield: conv.dividendYield, // current
			strategy: conv.strategy,
			legs: legs,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		alert("createSendRfq:"+ss);
		return ss;
	}

	static createTakeOwnership(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'take_ownership',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		//alert("createSendRfq:"+ss);
		return ss;
	}

	static createRejectRfq(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'reject_rfq',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		return ss;
	}

//	static createCounterQuote(state) {
//		var controls = state.controls;
//		var conv = state.conversation;
//		var sessionId = state.controls.sessionId;
//		var hdr = {
//				type: 'action',
//				action_name: 'counter_quote',
//				sessionId: sessionId
//			};
//		var conversation = {
//			id: conv.id,
//		};
//		var msg = {header: hdr, body: {conversation: conversation}};
//		var ss = JSON.stringify(msg);
//		return ss;
//	}

	static createRejectQuote(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'reject_quote',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		return ss;
	}

	static createRejectOrder(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'reject_order',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		return ss;
	}

	static createCalcOption(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'calc_option',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
			legs: conv.legs,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		return ss;
	}

	static createQuote(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'quote',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		//alert("createSendRfq:"+ss);
		return ss;
	}

	static createOrder(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'order',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		//alert("createSendRfq:"+ss);
		return ss;
	}

	static createCounter(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'counter_quote',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
			legs: conv.legs,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		return ss;
	}

	static createExecute(state) {
		var controls = state.controls;
		var conv = state.conversation;
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'execute',
				sessionId: sessionId
			};
		var conversation = {
			id: conv.id,
		};
		var msg = {header: hdr, body: {conversation: conversation}};
		var ss = JSON.stringify(msg);
		//alert("createSendRfq:"+ss);
		return ss;
	}

	static createGetPriceList(state) {
		var sessionId = state.controls.sessionId;
		var hdr = {
				type: 'action',
				action_name: 'get_price_list',
				sessionId: sessionId
			};
		var bdy = {};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createGetStockInfo(ticker) {
		var hdr = {
				type: 'action',
				action_name: 'get_stock_info'
			};
		var bdy = {symbol: ticker};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createGetOptionPrice(state) {
		var hdr = {
				type: 'action',
				action_name: 'get_option_price'
			};
		var bdy = {
				price: state.price,
				strike: state.strike,
				timeToMaturity: state.timeToMaturity,
				spot: state.riskFree,
				riskFree: state.riskFree,
				dividendYield: state.dividendYield,
				sigma: state.sigma,
		};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createQuoteRequest(ticker) {
		var hdr = {type: 'quote_request'};
		var bdy = {symbol: ticker};
		var msg = {header: hdr, body: bdy};
		return JSON.stringify(msg);
	}

	static createEquityOptionQuoteRequest(state) {
		var hdr = {type: 'calc_equity_option'};
		var bdy = {
			type: state.call_put,
//			price: new Number(state.price),
//			strike: new Number(state.strike),
//			dividend: new Number(state.dividend),
//			rate: new Number(state.rate),
//			volatility: new Number(state.volatility),
			today: state.today.format('YYYY-MM-DD'),
			settlement_date: state.settlement_date.format('YYYY-MM-DD'),
			maturity: state.maturity.format('YYYY-MM-DD'),
			date_counter: 'default',
		};
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createBondQuoteRequest(state) {
		var hdr = {type: 'calc_bond'};
		var bdy = {
		};
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createGetMarketList(state) {
		var hdr = {type: 'action', action_name: 'get_market_list'};
		var msg = {header: hdr};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createGetLatestPrices(sessionId) {
		var hdr = {type: 'action', action_name: 'get_latest_price', sessionId: sessionId};
		var msg = {header: hdr};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createGetMarketData(state) {
		var hdr = {type: 'action', action_name: 'get_market_data'};
		var bdy = {
			market_name: 'USD',
			trade_date: 'today',
//			today: state.today.format('YYYY-MM-DD'),
		};
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createSetMarketData(state) {
		var mkt = {};
		if(state.new_market_name){
			// insert
			mkt.market_name = state.new_market_name;
		} else {
			// update
			mkt.market_name = state.market_name;
		}
		if(state.trade_date){
			mkt.trade_date = state.trade_date;
		}
		if(state.calendar){
			mkt.calendar = state.calendar;
		}
		if(state.fixing_days){
			mkt.fixing_days = state.fixing_days;
		}
		if(state.settlement_days){
			mkt.settlement_days = state.settlement_days;
		}
		if(state.settlement_date){
			mkt.settlement_date = state.settlement_date;
		}
		if(state.zc_bonds_dc){
			mkt.zc_bonds_dc = state.zc_bonds_dc;
		}
		if(state.term_structure_dc){
			mkt.term_structure_dc = state.term_structure_dc;
		}
		if(state.deposit_dc){
			mkt.deposit_dc = state.deposit_dc;
		}
		if(state.swap_dc){
			mkt.swap_dc = state.swap_dc;
		}

		var hdr = {type: 'action', action_name: 'set_market_data'};
		var bdy = {
			market: mkt,
		};
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createCloneMarketData(state) {
		var hdr = {type: 'action', action_name: 'clone_market_data'};

		if(state.new_market_name){
			hdr.new_market_name = state.new_market_name;
		}
		if(state.new_market_name){
			hdr.new_trade_date = state.new_trade_date;
		}
		if(state.market_name){
			hdr.market_name = state.market_name;
		}
		if(state.trade_date){
			hdr.trade_date = state.trade_date;
		}

		var msg = {header: hdr};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createQuoteBond(state) {
		var hdr = {type: 'action', action_name: 'quote_bond'};
		var bdy = {
			market_name: 'USD',
			trade_date: 'today',
//			today: state.today.format('YYYY-MM-DD'),
		};
		if (state.calendar) {
			bdy.calendar = state.calendar;
		}
		if (state.settlement_date) {
			bdy.settlement_date = state.settlement_date;
		}
		if (state.fixing_days) {
			bdy.fixing_days = state.fixing_days;
		}
		if (state.settlement_days) {
			bdy.settlement_days = state.settlement_days;
		}
		if (state.trade_date) {
			bdy.trade_date = state.trade_date;
		}
		if (state.zcquotes) {
			bdy.zcquotes = state.zcquotes;
		}
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createCalcTradeDate(state) {
		var hdr = {type: 'action', action_name: 'calc_date'};
		var bdy = {
			type: 'trade_date',
		};
		if (state.calendar) {
			bdy.calendar = state.calendar;
		}
		if (state.settlement_date) {
			bdy.settlement_date = state.settlement_date.format('YYYY-MM-DD');
		}
		if (state.settlement_days) {
			bdy.settlement_days = new Number(state.settlement_days);
		}
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createCalcSettlementDate(state) {
		var hdr = {type: 'action', action_name: 'calc_date'};
		var bdy = {
			type: 'settlement_date',
		};
		if (state.calendar) {
			bdy.calendar = state.calendar;
		}
		if (state.trade_date) {
			bdy.trade_date = state.trade_date.format('YYYY-MM-DD');
		}
		if (state.settlement_days) {
			bdy.settlement_days = new Number(state.settlement_days);
		}
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}

	static createHeatbeat() {
		var hdr = {type: 'action', action_name: 'heartbeat'};
		var bdy = {
			message: 'hello',
		};
		var msg = {header: hdr, body: bdy};
		var str = JSON.stringify(msg);
		console.log(str);
		return JSON.stringify(msg);
	}
}
