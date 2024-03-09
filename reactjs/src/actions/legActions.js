
export const addLegAction = (v) => dispatch => {
 dispatch({
  type: 'ADD_LEG_ACTION',
  leg: {
		ls: "L",
		cp: "C",
		quantity: 100,
		strike: 100,
		maturity: "2018-10-06",
		volatility: 0.25,
		delta: 0.21,
		gamma: 0.22,
		vega: 0.23,
		theta: 0.24,
		rho: 0.25,
	},
 })
}

export const lsUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'LS_UPDATE_ACTION',
  id: id,
  ls: v
 })
}

export const cpUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'CP_UPDATE_ACTION',
  id: id,
  cp: v
 })
}


export const quantityUpdateAction = (id,v) => dispatch => {
	var re = new RegExp('(\\d+)(m+)','i');
	if(typeof(v) === 'string' && v.match(re)){
		var n = v.match(/\d+/g);
		var l = v.match(/m+/i);
		v = n * (l.length * 1000);
	}
 dispatch({
  type: 'QUANTITY_UPDATE_ACTION',
  id: id,
  quantity: Number(v),
 })
}

export const strikeUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'STRIKE_UPDATE_ACTION',
  id: id,
  strike: Number(v)
 })
}


export const maturityUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'MATURITY_UPDATE_ACTION',
  id: id,
  maturity: v
 })
}


export const volatilityUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'VOLATILITY_UPDATE_ACTION',
  id: id,
  volatility: Number(v)
 })
}

export const premiumUpdateAction = (id,v) => dispatch => {
 dispatch({
  type: 'PREMIUM_UPDATE_ACTION',
  id: id,
  premium: Number(v)
 })
}

export const expandLegAction = (id,v) => dispatch => {
 dispatch({
  type: 'EXPAND_LEG_ACTION',
  id: id,
  expandLeg: v
 })
}


