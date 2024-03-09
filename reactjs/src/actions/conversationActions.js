import moment from 'moment';

export const tradeDateUpdateAction = (v) => dispatch => {
 dispatch({
  type: 'TRADE_DATE_UPDATE_ACTION',
  tradeDate: v,
 })
}

export const settlementDateUpdateAction = (v) => dispatch => {
 dispatch({
  type: 'SETTLEMENT_DATE_UPDATE_ACTION',
  settlementDate: v,
 })
}

function numberOfLeg(v){
	if (v === 'straddle'
			|| v === 'strangle'
		){
		return 2;
	} else if (v === 'butterfly'){
		return 3;
	}
	return 1;
}

export const strategyUpdateAction = (v) => (dispatch, getState) => {
	var numOfLegs = numberOfLeg(v);
	var lg = {legs: [
			{
				id: 0,
				ls: 'Long',
				cp: 'Call',
				quantity: 0,
				strike: 0,
				maturity: moment(),
				volatility: 0,
				premium: 0,
				delta: 0,
				gamma: 0,
				vega: 0,
				theta: 0,
				rho: 0,
			},
			]};
		if(numOfLegs > 1){
			lg.legs[1] = {
					id: 1,
					ls: 'Long',
					cp: 'Call',
					quantity: 0,
					strike: 0,
					maturity: moment(),
					volatility: 0,
					premium: 0,
					delta: 0,
					gamma: 0,
					vega: 0,
					theta: 0,
					rho: 0,
				};
		}
		if(numOfLegs > 2){
			lg.legs[2] = {
					id: 2,
					ls: 'Long',
					cp: 'Call',
					quantity: 0,
					strike: 0,
					maturity: moment(),
					volatility: 0,
					premium: 0,
					delta: 0,
					gamma: 0,
					vega: 0,
					theta: 0,
					rho: 0,
				};
		}

	if (v === 'straddle'){
		lg.legs[0].ls = 'Long';
		lg.legs[0].cp = 'Call';
		lg.legs[1].ls = 'Long';
		lg.legs[1].cp = 'Put';
		lg.legs[0].quantity = 100; 
		lg.legs[1].quantity = lg.legs[0].quantity; 
		lg.legs[0].strike = getState().conversation.spotPrice; 
		lg.legs[1].strike = lg.legs[0].strike; 
	} else if (v === 'strangle'){
		lg.legs[0].ls = 'Long';
		lg.legs[0].cp = 'Call';
		lg.legs[1].ls = 'Long';
		lg.legs[1].cp = 'Put';
		lg.legs[0].quantity = 100; 
		lg.legs[1].quantity = lg.legs[0].quantity; 
		lg.legs[0].strike = getState().conversation.spotPrice; 
		lg.legs[1].strike = lg.legs[0].strike; 
	} else if (v === 'butterfly'){
		lg.legs[0].ls = 'Long';
		lg.legs[0].cp = 'Call';
		lg.legs[1].ls = 'Short';
		lg.legs[1].cp = 'Call';
		lg.legs[2].ls = 'Long';
		lg.legs[2].cp = 'Call';
		lg.legs[0].quantity = 100; 
		lg.legs[1].quantity = lg.legs[0].quantity * 2; 
		lg.legs[2].quantity = lg.legs[0].quantity; 
		lg.legs[0].strike = getState().conversation.spotPrice; 
		lg.legs[1].strike = lg.legs[0].strike; 
		lg.legs[2].strike = lg.legs[0].strike; 
	}

 dispatch({
  type: 'STRATEGY_UPDATE_ACTION',
  strategy: v,
  legs: lg.legs
 })
}

export const sideUpdateAction = (v) => dispatch => {
 dispatch({
  type: 'SIDE_UPDATE_ACTION',
  side: v
 })
}

export const symbolUpdateAction = (v) => dispatch => {
	dispatch({
		type: 'SYMBOL_UPDATE_ACTION',
		symbol: v,
	});
}

export const riskFreeRateUpdateAction = (v) => dispatch => {
 dispatch({
  type: 'RISK_FREE_RATE_UPDATE_ACTION',
  riskFreeRate: (typeof v === 'string' ? Number(v) : v),
 })
}

export const dividendYieldUpdateAction = (v) => dispatch => {
 dispatch({
  type: 'DIVIDEND_YIELD_UPDATE_ACTION',
  dividendYield: (typeof v === 'string' ? Number(v) : v),
 })
}

export const spotPriceUpdateAction = (v) => (dispatch, getState) => {
 dispatch({
  type: 'SPOT_PRICE_UPDATE_ACTION',
  spotPrice: (typeof v === 'string' ? Number(v) : v),
 })
}









