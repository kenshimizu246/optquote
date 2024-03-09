import {store} from '../index'
import {conn} from '../App'
import MessageFactory from '../MessageFactory'


export const priceListAction = (msg) => dispatch => {
 dispatch({
  type: 'PRICE_LIST_ACTION',
  prices: msg.body.prices,
 })
}

export const symbolListAction = (msg) => dispatch => {
 dispatch({
  type: 'SYMBOL_LIST_ACTION',
  symbols: msg.body.symbols,
 })
}

export const conversationUpdateAction = (v) => dispatch => {
	dispatch({
		type: 'CONVERSATION_UPDATE_ACTION',
		conversation: v,
	});
}


export const conversationCacheUpdateAction = (v) => dispatch => {
	dispatch({
		type: 'CONVERSATION_CACHE_UPDATE_ACTION',
		conversation: v,
	});
}

export const priceCacheUpdateAction = (v) => dispatch => {
	dispatch({
		type: 'PRICE_CACHE_UPDATE_ACTION',
		price: v,
	});
}

export const connectionStatusAction = (v) => dispatch => {
	if(v && v.length > 6 && 'ONCLOSE' === v.substr(0,7)){
		dispatch({
			type: 'CONNECTION_STATUS_ACTION',
			connectionStatus: v
		})
	} else {
		dispatch({
			type: 'CONNECTION_STATUS_ACTION',
			connectionStatus: v
		})
	}
}

