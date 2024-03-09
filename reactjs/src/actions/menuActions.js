import {store} from '../index'
import {conn} from '../App'
import MessageFactory from '../MessageFactory'

export const newRfqAction = (v) => dispatch => {
	var msg = MessageFactory.createSendRfq(store.getState());
	dispatch({
		type: 'NEW_RFQ_ACTION'
	});
}

export const closePositionAction = (v) => dispatch => {
	//var msg = MessageFactory.createClosePosition(store.getState());
	var conv = store.getState().conversation;
	var legs = conv.legs;

	var ll = [];
	for(var i = 0; i < legs.length; i++){
		var leg = {
			id: legs[i].id,
			ls: legs[i].ls,
			cp: legs[i].cp,
			quantity: legs[i].quantity,
			strike: legs[i].strike,
			maturity: legs[i].maturity,
			premium: 0,
			volatility: 0,
			delta: 0,
			gamma: 0,
			vega: 0,
			theta: 0,
			rho: 0,
		};
		ll[i] = leg;
	}
	dispatch({
		type: 'CLOSE_POSITION_ACTION',
		id: 0,
		link_id: conv.id,
		status: 'INIT',
		oc: 'Close',
		symbol: conv.symbol,
		side: (conv.side === 'Buy' ? 'Sell' : 'Buy'),
		strategy: conv.strategy,
		legs: ll,
	});
}

export const sendRfqAction = (v) => dispatch => {
	var msg = MessageFactory.createSendRfq(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const takeOwnershipAction = (v) => dispatch => {
	var msg = MessageFactory.createTakeOwnership(store.getState());
	//alert("takeOwnershipAction:"+JSON.stringify(msg));
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const calcOptionAction = (v) => dispatch => {
	var msg = MessageFactory.createCalcOption(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const quoteAction = (v) => dispatch => {
	var msg = MessageFactory.createQuote(store.getState());

	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const rejectRfqAction = (v) => dispatch => {
	var msg = MessageFactory.createRejectRfq(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const rejectOrderAction = (v) => dispatch => {
	var msg = MessageFactory.createRejectOrder(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const rejectQuoteAction = (v) => dispatch => {
	var msg = MessageFactory.createRejectQuote(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const orderAction = (v) => dispatch => {
	var msg = MessageFactory.createOrder(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const counterAction = (v) => dispatch => {
	var msg = MessageFactory.createCounter(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}

export const executeAction = (v) => dispatch => {
	var msg = MessageFactory.createExecute(store.getState());
	if(conn){
		conn.send(msg);
	} else {
		alert('conn is not available.');
	}
}










