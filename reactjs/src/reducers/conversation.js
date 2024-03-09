import moment from 'moment';

export default (state = {}, action) => {
	console.log("reducer:"+action.type);
 switch (action.type) {
	case 'STATUS_UPDATE_ACTION':
		return {
			...state,
			status: action.status,
		}
	case 'SIDE_UPDATE_ACTION':
		return {
			...state,
			side: action.side,
		}
	case 'SYMBOL_UPDATE_ACTION':
		return {
			...state,
			symbol: action.symbol,
			spotPrice: action.spotPrice,
			dividendYield: action.dividendYield,
		}
  case 'SPOT_PRICE_UPDATE_ACTION':
   return {
		...state,
    spotPrice: action.spotPrice,
		all: action.all
   }
  case 'RISK_FREE_RATE_UPDATE_ACTION':
   return {
		...state,
    riskFreeRate: action.riskFreeRate
   }
  case 'DIVIDEND_YIELD_UPDATE_ACTION':
   return {
		...state,
    dividendYield: action.dividendYield
   }
	case 'TRADE_DATE_UPDATE_ACTION':
		return {
		...state,
    tradeDate: action.tradeDate
		}
	case 'SETTLEMENT_DATE_UPDATE_ACTION':
		return {
		...state,
    settlementDate: action.settlementDate
		}
  case 'STRATEGY_UPDATE_ACTION':
   return {
    ...state,
    strategy: action.strategy,
    legs: action.legs
   }
  case 'ADD_LEG_ACTION':
		var legs;
		if(state.legs){
			legs = [
				...state.legs,
				action.leg,
			];
		} else {
			legs = [
				action.leg,
			];
		}
		for(let i = 0; i < legs.length; i++){
			legs[i].id = i;
		}
   return {
		...state,
    legs: legs
   }
  case 'LS_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			if(state.strategy === 'straddle'){
				state.legs[0].ls = action.ls;
				state.legs[1].ls = action.ls;
			} else {
				state.legs[action.id].ls = action.ls;
			}
		}
   return {
		...state,
   }
  case 'CP_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			if(state.strategy === 'straddle'
					|| state.strategy === 'strangle'){
				state.legs[0].cp = action.cp;
				state.legs[1].cp = (action.cp === 'Call'?'Put':'Call');
			} else if(state.strategy === 'butterfly'){
				state.legs[0].cp = action.cp;
				state.legs[1].cp = action.cp;
				state.legs[2].cp = action.cp;
			} else {
				state.legs[action.id].cp = action.cp;
			}
		}
   return {
		...state,
   }
  case 'QUANTITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			if(state.strategy === 'straddle'
					|| state.strategy === 'strangle'
					){
				state.legs[0].quantity = action.quantity;
				state.legs[1].quantity = action.quantity;
			} else if(state.strategy === 'butterfly') {
				if(action.id === 1){
					state.legs[0].quantity = action.quantity/2;
					state.legs[1].quantity = action.quantity;
					state.legs[2].quantity = action.quantity/2;
				} else {
					state.legs[0].quantity = action.quantity;
					state.legs[1].quantity = action.quantity * 2;
					state.legs[2].quantity = action.quantity;
				}
			} else {
				state.legs[action.id].quantity = action.quantity;
			}
		}
   return {
		...state,
   }
  case 'STRIKE_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			if(state.strategy === 'straddle'){
				state.legs[0].strike = action.strike;
				state.legs[1].strike = action.strike;
			} else if(state.strategy === 'strangle'){
				state.legs[action.id].strike = action.strike;
				if(action.id === 0){
					if(state.legs[1].strike < state.legs[0].strike){
						state.legs[1].strike = state.legs[0].strike;
					}
				} else if(action.id === 1){
					if(state.legs[1].strike < state.legs[0].strike){
						state.legs[0].strike = state.legs[1].strike;
					}
				}
			} else if(state.strategy === 'butterfly'){
				state.legs[action.id].strike = action.strike;
				if(action.id === 0){
					if(state.legs[0].strike > state.legs[1].strike){
						var d = state.legs[2].strike - state.legs[1].strike;
						state.legs[1].strike = state.legs[0].strike;
						state.legs[2].strike = Math.trunc((state.legs[1].strike + d)*100)/100;
					}
				} else if(action.id === 1){
					if(state.legs[0].strike > state.legs[1].strike){
						state.legs[0].strike = state.legs[1].strike;
					} else if(state.legs[2].strike < state.legs[1].strike){
						state.legs[2].strike = state.legs[1].strike;
					}
				} else if(action.id === 2){
					if(state.legs[1].strike > state.legs[2].strike){
						var d = state.legs[0].strike - state.legs[1].strike;
						state.legs[1].strike = state.legs[2].strike;
						state.legs[0].strike = Math.trunc((state.legs[1].strike + d) * 100)/100;
					}
				}
			} else {
				state.legs[action.id].strike = action.strike;
			}
		}
   return {
		...state,
   }
  case 'MATURITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			if(state.strategy === 'straddle'
					|| state.strategy === 'strangle'
					){
				state.legs[0].maturity = action.maturity;
				state.legs[1].maturity = action.maturity;
			} else if(state.strategy === 'butterfly') {
				state.legs[0].maturity = action.maturity;
				state.legs[1].maturity = action.maturity;
				state.legs[2].maturity = action.maturity;
			} else {
				state.legs[action.id].maturity = action.maturity;
			}
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'PREMIUM_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].premium = action.premium;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
  case 'VOLATILITY_UPDATE_ACTION':
		if(state.legs && action.id < state.legs.length){
			state.legs[action.id].volatility = action.volatility;
		}
   return {
		...state,
   }
	case 'CONVERSATION_UPDATE_ACTION':
		//alert("reducer.conv:"+JSON.stringify(action.conversation));
		//alert("reducer.state:"+JSON.stringify(state));
		for(let i = 0; i < action.conversation.legs.length; i++){
			action.conversation.legs[i].maturity = moment(action.conversation.legs[i].maturity);
		}
   return {
		...state,
		id: action.conversation.id,
		status: action.conversation.status,
		oc: action.conversation.oc,
		link_id: action.conversation.link_id,
		side: formatSide(action.conversation.side),
		symbol: action.conversation.symbol,
		spotPrice: action.conversation.spotPrice,
    tradeDate: moment(action.conversation.tradeDate),
		settlementDate: moment(action.conversation.settlementDate),
		riskFreeRate: action.conversation.riskFreeRate,
		dividendYield: action.conversation.dividendYield,
		strategy: action.conversation.strategy,
		userId: action.conversation.userId,
		traderId: action.conversation.traderId,
		legs: action.conversation.legs,
		quoteStart: action.conversation.quoteStart,
		quoteExpire: action.conversation.quoteExpire,
		quoteRemain: action.conversation.quoteRemain,
   }
	case 'NEW_RFQ_ACTION':
   return newRfq(state);
	case 'CLOSE_POSITION_ACTION':
		return {
			...state,
			id: action.id,
			link_id: action.link_id,
			status: action.status,
			oc: action.oc,
			symbol: action.symbol,
			side: action.side,
			strategy: action.strategy,
			legs: action.legs,
		}
  default:
   return state
 }
}

function formatSide(v){
	if(v.toLowerCase() === 'sell'	
			|| v.toLowerCase() === 's'){
		return 'Sell';
	}
	return 'Buy';
}

function dupLegs(legs){
		for(let i = 0; i < legs.length; i++){
			//ll[i].id = i;
		}
}

function newRfq(state){
	return {
		...state,
		id: 0,
		status: 'INIT',
		oc: 'Open',
		link_id: 0,
	};
}



