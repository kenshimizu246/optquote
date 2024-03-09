import moment from 'moment';

export default (state = {}, action) => {
	switch (action.type) {
		case 'CONVERSATION_CACHE_UPDATE_ACTION':
			var conv = action.conversation;
			var list = [];
			var maxlen = 100;

			if(state.conversations){
				list = state.conversations;
			}
			if(list.length === 0){
				list[0] = conv;
			} else if(list.find(e => { return e.id === conv.id })){
				var idx = list.findIndex(e => {return e.id == conv.id});
				list[idx] = conv;
			} else {
				var inserted = 0;
				for(var i = 0; i < list.length; i++){
					if(list[i].id < conv.id){
						inserted = 1;
						list.splice(i, 0, conv);
						break;
					}
				}
				if(inserted === 0){
					list[list.length] = conv;
				}
			}
			return {
				...state,
				conversations: list,
			}
		case 'PRICE_CACHE_UPDATE_ACTION':
			var price = action.price;
			var list = [];

			if(state.prices){
				list = state.prices;
			}
			if(list.find(p => { return p.symbol === price.symbol })){
				var idx = list.findIndex(e => {return e.symbol == price.symbol});
				list[idx] = price;
			} else {
				list[list.length] = price;
			}
			return {
				...state,
				prices: list,
			}
    case 'EXPAND_LEG_ACTION':
			var list = [];
			if(state.conversations){
				list = state.conversations;
			}
			console.log("ddx:"+action.id);
			console.log("list.length:"+list.length);
			for(var i = 0; i < list.length; i++){
				console.log("ccx:"+i+":"+list[i].id+":"+action.id);
				if(list[i].id === action.id){
					list[i].expandLeg = !list[i].expandLeg;
					console.log("bbx:"+list[i].expandLeg);
					break;
				}
			}
			return {
				...state,
				list: list
			}
		default:
			return state
	}
}

function reverseIDs(a, b){
	return b - a;
}

