import React, { Component } from 'react';
import { connect } from 'react-redux';
import PropTypes from 'prop-types';
import { Router, Route, Switch} from 'react-router';
import { withRouter } from 'react-router-dom';

import DatePicker from 'react-datepicker';
import moment from 'moment';
import MomentPropTypes from 'react-moment-proptypes';
import 'react-datepicker/dist/react-datepicker.css';
import NumericInput from 'react-numeric-input';
import Moment from 'react-moment';

import _ from 'lodash';


import { statusBarUpdateAction } from './actions/statusBarUpdateAction'

import { priceListAction, symbolListAction, conversationUpdateAction, conversationCacheUpdateAction, priceCacheUpdateAction, connectionStatusAction } from './actions/messageActions'
import { showLoginAction, userIdUpdateAction, passwdUpdateAction, roleUpdateAction, loginStatusAction } from './actions/loginActions'
import { newRfqAction, closePositionAction, sendRfqAction, takeOwnershipAction, calcOptionAction, quoteAction, rejectRfqAction, rejectQuoteAction, orderAction, counterAction, rejectOrderAction, executeAction } from './actions/menuActions'
import { tradeDateUpdateAction, settlementDateUpdateAction, strategyUpdateAction, sideUpdateAction, symbolUpdateAction, riskFreeRateUpdateAction, dividendYieldUpdateAction, spotPriceUpdateAction } from './actions/conversationActions'
import { addLegAction, lsUpdateAction, cpUpdateAction, quantityUpdateAction, strikeUpdateAction, maturityUpdateAction, volatilityUpdateAction, premiumUpdateAction, expandLegAction } from './actions/legActions'

import Connection from './Connection'
import MessageFactory from './MessageFactory'

import InputField from './components/InputField'
import SelectField from './components/SelectField'
import DateField from './components/DateField'

import logo from './logo.svg';
import './App.css';

const mapStateToProps = state => ({
 ...state
})

const mapDispatchToProps = dispatch => ({
	spotPriceUpdateAction: (v) => dispatch(spotPriceUpdateAction(v)),
	riskFreeRateUpdateAction: (v) => dispatch(riskFreeRateUpdateAction(v)),
	dividendYieldUpdateAction: (v) => dispatch(dividendYieldUpdateAction(v)),
	connectionStatusAction: (v) => dispatch(connectionStatusAction(v)),
	sideUpdateAction: (v) => dispatch(sideUpdateAction(v)),
	symbolUpdateAction: (v) => dispatch(symbolUpdateAction(v)),
	newRfqAction: (v) => dispatch(newRfqAction(v)),
	closePositionAction: (v) => dispatch(closePositionAction(v)),
	sendRfqAction: (v) => dispatch(sendRfqAction(v)),
	takeOwnershipAction: (v) => dispatch(takeOwnershipAction(v)),
	calcOptionAction: (v) => dispatch(calcOptionAction(v)),
	quoteAction: (v) => dispatch(quoteAction(v)),
	rejectRfqAction: (v) => dispatch(rejectRfqAction(v)),
	rejectQuoteAction: (v) => dispatch(rejectQuoteAction(v)),
	orderAction: (v) => dispatch(orderAction(v)),
	counterAction: (v) => dispatch(counterAction(v)),
	executeAction: (v) => dispatch(executeAction(v)),
	rejectOrderAction: (v) => dispatch(rejectOrderAction(v)),
	conversationUpdateAction: (v) => dispatch(conversationUpdateAction(v)),
	conversationCacheUpdateAction: (v) => dispatch(conversationCacheUpdateAction(v)),
	priceCacheUpdateAction: (v) => dispatch(priceCacheUpdateAction(v)),
	addLegAction: (v) => dispatch(addLegAction(v)),
	lsUpdateAction: (id,v) => dispatch(lsUpdateAction(id,v)),
	cpUpdateAction: (id,v) => dispatch(cpUpdateAction(id,v)),
	quantityUpdateAction: (id,v) => dispatch(quantityUpdateAction(id,v)),
	strikeUpdateAction: (id,v) => dispatch(strikeUpdateAction(id,v)),
	maturityUpdateAction: (id,v) => dispatch(maturityUpdateAction(id,v)),
	volatilityUpdateAction: (id,v) => dispatch(volatilityUpdateAction(id,v)),
	premiumUpdateAction: (id,v) => dispatch(premiumUpdateAction(id,v)),
	loginStatusAction: (v) => dispatch(loginStatusAction(v)),
	showLoginAction: (v) => dispatch(showLoginAction(v)),
	userIdUpdateAction: (v) => dispatch(userIdUpdateAction(v)),
	passwdUpdateAction: (v) => dispatch(passwdUpdateAction(v)),
	roleUpdateAction: (v) => dispatch(roleUpdateAction(v)),
	tradeDateUpdateAction: (v) => dispatch(tradeDateUpdateAction(v)),
	settlementDateUpdateAction: (v) => dispatch(settlementDateUpdateAction(v)),
	strategyUpdateAction: (v) => dispatch(strategyUpdateAction(v)),
	symbolListAction: (v) => dispatch(symbolListAction(v)),
	priceListAction: (v) => dispatch(priceListAction(v)),
	statusBarUpdateAction: (v) => dispatch(statusBarUpdateAction(v)),
	expandLegAction: (id,v) => dispatch(expandLegAction(id,v)),
})

var params = window.location.search;
var env = "prod";
var debug = false;
var role = "";
var wspass = "/ws-oq";

if(params.indexOf("env=dev") > -1){
	env = "dev";
	wspass = "/ws3";
}

if(params.indexOf("debug=true") > -1){
	debug=true;
}

if(params.indexOf("role=trader") > -1){
	role = "trader";
}else if(params.indexOf("role=client") > -1){
	role = "client";
}

var conn = new Connection('ws://'+ window.location.hostname + wspass, ['quote']);
var seen = []; 

class App extends Component {
	constructor(props) {
		super(props);
		this.state = {
		};
		this.props.userIdUpdateAction("client");
		this.props.roleUpdateAction("client");
		this.props.sideUpdateAction("S");
		this.props.riskFreeRateUpdateAction(0.01);
		this.props.tradeDateUpdateAction(moment());
		this.props.settlementDateUpdateAction(moment().add(2, 'days'));
		this.props.strategyUpdateAction("straddle");
		this.openConnection();
	}

	onSystemEvent(msg){
		var ss = JSON.stringify(msg);
		console.log("system event:"+ss);
		
		var sessionId =	this.props.controls.sessionId;
		if(sessionId){
			this.doReconnect(sessionId);
		}
		
		this.connectionStatusAction(msg);
	}

	onMessage(msg){
		if(msg.header && msg.header.type){
			if(msg.header.type === "login_result"){
				this.loginStatusAction(msg);
				this.props.statusBarUpdateAction("login_result:"+msg);
			} else if(msg.header.type === "calc_quote_result"){ // quote calc result
				this.conversationUpdate(msg.body.conversation);
				this.conversationCacheUpdate(msg.body.conversation);
			} else if(msg.header.type === "symbols"){
				this.symbolListAction(msg);
			} else if(msg.header.type === "prices"){
				this.priceListAction(msg);
			} else if(msg.header.type === "heartbeat_response"){
				// need to do something.
			} else if(msg.header.type === "conversation"){
				this.conversationUpdate(msg.body.conversation);
				this.conversationCacheUpdate(msg.body.conversation);
				this.props.statusBarUpdateAction("conversation:"+msg.body.conversation.id);
				console.log("conversation123:"+msg.body.conversation.id);
			} else if(msg.header.type === "price"){
				this.priceCacheUpdate(msg.body.price);
				this.props.statusBarUpdateAction("price:"+msg.body.price.symbol);
				console.log("price123:"+msg.body.price.symbol);
			} else {
				var ss = JSON.stringify(msg);
				console.log("unknown msg:"+ss);
			}
		}
	}

	sideUpdateAction = (event) => {
		this.props.sideUpdateAction(event.target.value);
	}

	symbolUpdateAction = (event) => {
		var spotPrice = 0;
		var dividendYield = 0;
		var quantity = 0;

		var prices = this.props.cache.prices;
		for(let i = 0; i < prices.length; i++){
			if(prices[i].symbol === event.target.value){
				spotPrice = prices[i].price;
				dividendYield = prices[i].dividendYield;
				quantity = 100;
			}
		}
		this.props.symbolUpdateAction(event.target.value);
		if(quantity > 0){
			this.props.spotPriceUpdateAction(spotPrice);
			this.props.dividendYieldUpdateAction(dividendYield);
			var legs = this.props.conversation.legs;
			for(let i = 0; i < legs.length; i++){
				this.props.strikeUpdateAction(i, spotPrice);
				this.props.quantityUpdateAction(i, quantity);
			}
		}
	}

	spotPriceUpdateAction = (event) => {
		this.props.spotPriceUpdateAction(event.target.value);
	}

	tradeDateUpdateAction = (value) => {
		this.props.tradeDateUpdateAction(value);
	}

	settlementDateUpdateAction = (value) => {
		this.props.settlementDateUpdateAction(value);
	}

	strategyUpdateAction = (event) => {
		this.props.strategyUpdateAction(event.target.value);
	}

	symbolListAction = (msg) => {
		this.props.symbolListAction(msg);
	}

	priceListAction = (msg) => {
		this.props.priceListAction(msg);
	}

	userIdUpdateAction = (event) => {
		this.props.userIdUpdateAction(event.target.value);
	}

	passwdUpdateAction = (event) => {
		this.props.passwdUpdateAction(event.target.value);
	}

	roleUpdateAction = (event) => {
		this.props.roleUpdateAction(event.target.value);
	}

	riskFreeRateUpdateAction = (event) => {
		this.props.riskFreeRateUpdateAction(event.target.value);
	}

	dividendYieldUpdateAction = (event) => {
		this.props.dividendYieldUpdateAction(event.target.value);
	}

	statusBarUpdateAction = (event) => {
		this.props.statusBarUpdateAction(event.target.value);
	}

	lsUpdateAction = (id, event) => {
		this.props.lsUpdateAction(id, event.target.value);
	}

	cpUpdateAction = (id, event) => {
		this.props.cpUpdateAction(id, event.target.value);
	}

	quantityUpdateAction = (id, event) => {
		this.props.quantityUpdateAction(id, event.target.value);
	}

	strikeUpdateAction = (id, event) => {
		this.props.strikeUpdateAction(id, event.target.value);
	}

	volatilityUpdateAction = (id, event) => {
		this.props.volatilityUpdateAction(id, event.target.value);
	}

	premiumUpdateAction = (id, event) => {
		this.props.premiumUpdateAction(id, event.target.value);
	}

	maturityUpdateAction = (id, value) => {
		this.props.maturityUpdateAction(id, value);
	}

	connectionStatusAction = (msg) => {
		this.props.connectionStatusAction(msg);
	}

	loginStatusAction = (v) => {
		this.props.loginStatusAction(v);
	}

	openConnection = (event) => {
		conn.removeAllEventListeners('SYS');
		conn.removeAllEventListeners('MSG');
		conn.on('SYS', this.onSystemEvent.bind(this));
		conn.on('MSG', this.onMessage.bind(this));
		conn.connect();
	}

	showLoginAction = (event) => {
		this.props.showLoginAction(true);
	}

	doLoginAction = (event) => {
		// set cursor progress
		var shield = document.getElementById('shield');
		if(shield){
			shield.style = "cursor: progress";
		}
		conn.send(
			MessageFactory.createLogin(
				this.props.controls.userId,
				this.props.controls.passwd,
				this.props.controls.role));
	}

	cancelLoginAction = (event) => {
		this.props.showLoginAction(false);
	}

	doReconnect = (sessionId) => {
		conn.send(
			MessageFactory.createReconnect(
				sessionId,
				this.props.controls.userId,
				this.props.controls.role));
	}

	addLeg = (event) => {
		this.props.addLegAction();
	}

	closeConnection = (event) => {
		conn.close();
		conn.removeAllEventListeners('SYS');
		conn.removeAllEventListeners('MSG');
	}

	calcOption = (v) => {
		this.props.calcOptionAction(v);
	}

	newRfq = (v) => {
		this.props.newRfqAction(v);
		this.props.userIdUpdateAction("client");
		this.props.roleUpdateAction("client");
		this.props.sideUpdateAction("S");
		this.props.riskFreeRateUpdateAction(0.01);
		this.props.tradeDateUpdateAction(moment());
		this.props.settlementDateUpdateAction(moment().add(2, 'days'));
		this.props.strategyUpdateAction("straddle");
	}

	closePosition = (v) => {
		this.props.userIdUpdateAction("client");
		this.props.roleUpdateAction("client");
		this.props.riskFreeRateUpdateAction(0.01);
		this.props.tradeDateUpdateAction(moment());
		this.props.settlementDateUpdateAction(moment().add(2, 'days'));
		this.props.closePositionAction(v);
	}

	sendRfq = (v) => {
		this.props.sendRfqAction(v);
	}

	takeOwnership = (v) => {
		this.props.takeOwnershipAction(v);
	}

	quote = (v) => {
		this.props.quoteAction(v);
	}

	rejectRfq = (v) => {
		this.props.rejectRfqAction(v);
	}

	rejectQuote = (v) => {
		this.props.rejectQuoteAction(v);
	}

	order = (v) => {
		this.props.orderAction(v);
	}

	counter = (v) => {
		this.props.counterAction(v);
	}

	execute = (v) => {
		this.props.executeAction(v);
	}

	rejectOrder = (v) => {
		this.props.rejectOrderAction(v);
	}

	conversationUpdate = (v) => {
		this.props.conversationUpdateAction(v);
	}

	conversationCacheUpdate = (v) => {
		this.props.conversationCacheUpdateAction(v);
	}

	priceCacheUpdate = (v) => {
		this.props.priceCacheUpdateAction(v);
	}

	expandLegAction = (id, event) => {
		this.props.expandLegAction(id, event.target.value);
	}

	setConversation = (id) => {
		if(this.props.conversation.id === id){
			return;
		}
	}

	renderLogin(){
		console.log(window.location);
		let params = window.location.search;
		if(role === "trader"){
			this.props.controls.userId = "trader";
			this.props.controls.passwd = "trader";
			this.props.controls.role = "trader";
		}else if(role === "client"){
			this.props.controls.userId = "client";
			this.props.controls.passwd = "client";
			this.props.controls.role = "client";
		}
		return (
				<div id="shield" className="shield">
					<div id="login-box" className="login-box">
						<div id="login-title" className="login-title">Login</div>
						<div id="login-uid-box" className="login-field-box">
							<div id="login-uid-label" className="login-field-label">UserID:</div>
							<InputField
								name="userId"
								value={this.props.controls.userId}
								type="text"
								handleChange={this.userIdUpdateAction}
								id="login-uid-input"
								className="login-field-input"
								/>
						</div>
						<div id="login-pwd-box" className="login-field-box">
							<div id="login-pwd-label" className="login-field-label">Password:</div>
							<InputField
								name="passwd"
								value={this.props.controls.passwd}
								type="password"
								handleChange={this.passwdUpdateAction}
								id="login-pwd-input"
								className="login-field-input"
								/>
						</div>
						<div id="login-role-box" className="login-field-box">
							<div id="login-role-label" className="login-field-label">Role:</div>
							<select
								name="role"
								value={this.props.controls.role}
								onInput={this.roleUpdateAction}
								type="text"
								id="login-role-select"
								className="login-field-select"
								>
								<option value="client">client</option>
								<option value="trader">trader</option>
							</select>
						</div>
						<div id="login-control-box" className="login-control-box">
							<button
								onClick={this.doLoginAction}
								id="login-login-btn"
								className="btn"
								>Login</button>
							<button
								onClick={this.cancelLoginAction}
								id="login-cancel-btn"
								className="btn"
								>Cancel</button>
						</div>
						<div id="login-status-box" className="login-status-box">
						<pre>
						Connection Status:{
							this.props.controls.connectionStatus
						}
						</pre>
						<pre>
						Status:{
							this.props.controls.statusBar
						}
						</pre>
						</div>
					</div>
				</div>);
	}

	renderLegLabel(){
		return (
			<div className="leg-header">
				<div className="leg-field-ls-label">
					L/S
				</div>
				<div className="leg-field-cp-label">
					C/P
				</div>
				<div className="leg-field-label">
					Qty
				</div>
				<div className="leg-field-label">
					Strike
				</div>
				<div className="leg-date-field-label">
						Maturity
				</div>
				<div className="leg-field-label">
						Vol
				</div>
				<div className="leg-field-label">
						Premium
				</div>
				<div className="leg-field-label">
						Delta
				</div>
				<div className="leg-field-label">
						Gamma
				</div>
				<div className="leg-field-label">
						Vega
				</div>
				<div className="leg-field-label">
						Theta
				</div>
			</div>
		);
	}

	renderLeg(leg){
		var status = this.props.conversation.status;
		var role = this.props.controls.role;
		var strategy = this.props.conversation.strategy;

		var lsopt = ["Long","Short"].map((option) => 
			 <option value={option}>{option}</option>
		);
		var cpopt = ["Call","Put"].map((option) =>
			<option value={option}>{option}</option>
		);

		var lsro = true;
		var cpro = true;
		var qtyro = true;
		var strikero = true;
		var maturityro = true;
		var volro = true;

		if(status === "INIT" && role === "client"){
			if(strategy === "custom"){
				lsro = false;
				cpro = false;
			} else if(strategy === "straddle" && leg.id === 0){
				qtyro = false;
				strikero = false;
				maturityro = false;
			} else if(strategy === "strangle"){
				if(leg.id === 0){
					qtyro = false;
					maturityro = false;
				}
				strikero = false;
			} else if(strategy === "butterfly"){
				if(leg.id === 0){
					qtyro = false;
					maturityro = false;
				}
				strikero = false;
			}
		}
		if(status === "TAKE" && role === "trader"){
			volro = false;
		}
		if(status === "QUOTE" && role === "client"){
			volro = false;
		}

		var ls;
		var cp;
		var qty;
		var strike;
		var maturity;
		var vol;

		if(lsro){
			ls = (
				<div name="ls-{leg.id}"
						className="leg-field-ls-ro">
							{leg.ls}
				</div>
			);
		} else {
			ls = (
				<SelectField
					name="ls-{leg.id}"
					value={leg.ls}
					onChange={(e) => this.lsUpdateAction(leg.id, e)}
					type="text"
					options={lsopt}
					className="leg-field-ls"
					/>
			);
		}

		if(cpro){
			cp = (
				<div name="cp-{leg.id}"
						className="leg-field-cp-ro">
							{leg.cp}
				</div>
			);
		} else {
			cp = (
				<SelectField
					id="cp-{leg.id}"
					name="cp-{leg.id}"
					value={leg.cp}
					onChange={(e) => this.cpUpdateAction(leg.id, e)}
					type="text"
					options={cpopt}
					className="leg-field-cp"
					/>
			);
		}

		if(qtyro){
			qty = (
				<div name="qty-{leg.id}"
						className="leg-field-ro">
							{leg.quantity}
				</div>
			);
		} else {
			qty = (
				<InputField
					name="qty-{leg.id}"
					value={leg.quantity}
					handleChange={(e) => this.quantityUpdateAction(leg.id, e)}
					type="text"
					step={10}
					className="leg-field"
					/>
			);
		}

		if(strikero){
			strike = (
				<div name="strike-{leg.id}"
						className="leg-field-ro">
							{leg.strike}
				</div>
			);
		} else {
			strike = (
				<InputField
					name="strike-{leg.id}"
					value={leg.strike}
					type="number"
					step={0.01}
					handleChange={(e) => this.strikeUpdateAction(leg.id, e)}
					className="leg-field"
					/>
			);
		}

		if(volro){
			vol = (
				<div name="volatility-{leg.id}"
						className="leg-field-ro">
							{leg.volatility}
				</div>
			);
		} else {
			vol = (
				<InputField
					name="volatility-{leg.id}"
					value={leg.volatility}
					type="number"
					handleChange={(e) => this.volatilityUpdateAction(leg.id, e)}
					readonly={volro}
					className="leg-field"
					step="0.01"
					/>
			);
		}

		return (
			<div className="leg">
				<div className="leg-field-ls">
					{ls}
				</div>
				<div className="leg-field-cp">
					{cp}
				</div>
				<div className="leg-field-box">
					{qty}
				</div>
				<div className="leg-field-box">
					{strike}
				</div>
				<div className="leg-date-field-box">
					<DateField
						selected={leg.maturity}
						readonly={maturityro}
						onChange={(value) => this.maturityUpdateAction(leg.id, value)}
						className="leg-input-date"
						/>
				</div>
				<div className="leg-field-box">
					{vol}
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" id="delta-{leg.id}">{leg.premium}</div>
				</div>
				<div className="leg-field-box">
						<div className="leg-field-ro" id="delta-{leg.id}">{leg.delta}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" name="gamma-{leg.id}">{leg.gamma}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" name="vega-{leg.id}">{leg.vega}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" id="theta-{leg.id}">{leg.theta}</div>
				</div>
			</div>
		);
	}

	renderLegRO(leg){
		var maturity;
		if(leg.maturity){
			maturity = leg.maturity.format("DD/MM/YYYY");
		}
		return (
			<div className="leg">
				<div className="leg-field-ls">
						<div className="leg-field-ro" id="ls-{leg.id}">{leg.ls}</div>
				</div>
				<div className="leg-field-cp">
						<div className="leg-field-ro" id="cp-{leg.id}">{leg.cp}</div>
				</div>
				<div className="leg-field-box">
						<div className="leg-field-ro" id="quantity-{leg.id}">{leg.quantity}</div>
				</div>
				<div className="leg-field-box">
						<div className="leg-field-ro" id="strike-{leg.id}">{leg.strike}</div>
				</div>
				<div className="leg-date-field-box">
						<div className="leg-field-ro" id="maturity-{leg.id}">{maturity}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" id="cp-{leg.id}">{leg.volatility}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" id="cp-{leg.id}">{leg.premium}</div>
				</div>
				<div className="leg-field-box">
						<div className="leg-field-ro" id="delta-{leg.id}">{leg.delta}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" name="gamma-{leg.id}">{leg.gamma}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" name="vega-{leg.id}">{leg.vega}</div>
				</div>
				<div className="leg-field-box">
					<div className="leg-field-ro" id="theta-{leg.id}">{leg.theta}</div>
				</div>
			</div>
		);
	}

	renderMenu(){
		var btnNewRfq =				<button id="btnNewRfq" onClick={this.newRfq} class="btn">New</button>;
		var btnClose =				<button id="btnClose" onClick={this.closePosition} class="btn">Close</button>;
		var btnClone =				<button id="btnClone" onClick={this.newRfq} class="btn">Clone</button>;
		var btnPartialClose =	<button id="btnPartialClose" onClick={this.newRfq} class="btn">Partial Close</button>;
		var btnSendRfq =			<button id="btnSendRfq" onClick={this.sendRfq} class="btn">Send RFQ</button>;
		var btnTakeOwnership =	<button id="btnTakeOwnership" onClick={this.takeOwnership} class="btn">Take</button>;
		var btnCalcOpt =			<button id="btnCalcOpt" onClick={this.calcOption} class="btn">Calc</button>;
		var btnQuote =				<button id="btnQuote" onClick={this.quote} class="btn">Quote</button>;
		var btnRejectRfq =		<button id="btnRejectRfq" onClick={this.rejectRfq} class="btn">Reject</button>;
		var btnOrder =				<button id="btnOrder" onClick={this.order} class="btn">Order</button>;
		var btnCounter =			<button id="btnCounter" onClick={this.counter} class="btn">Counter</button>;
		var btnCancel =				<button id="btnCancel" onClick={this.rejectQuote} class="btn">Cancel</button>;
		var btnExecute =			<button id="btnExecute" onClick={this.execute} class="btn">Execute</button>;
		var btnReject =				<button id="btnReject" onClick={this.rejectOrder} class="btn">Reject</button>;

		if(!this.props.conversation.status
				|| this.props.conversation.status === 'INIT'
				|| this.props.conversation.status === ''){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
						{btnSendRfq}
						{btnNewRfq}
					</div>
					);
			}
		} else if(this.props.conversation.status === 'RFQ'){
			if(this.props.controls.role === 'trader'){
				return (
					<div id="menu-box" className="menu-box">
							{btnTakeOwnership}
					</div>
						);
			}else if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'REJECT_RFQ'
					|| this.props.conversation.status === 'REJECT_QUOTE'
					|| this.props.conversation.status === 'REJECT_ORDER'
					|| this.props.conversation.status === 'TIMEOUT'
				){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'TAKE'){
			if(this.props.controls.role === 'trader'){
				// console.log("userId === traderId : " + this.props.controls.userId + " : " + this.props.conversation.traderId);
				if(this.props.controls.userId === this.props.conversation.traderId){
					return (
					<div id="menu-box" className="menu-box">
							{btnCalcOpt}
							{btnQuote}
							{btnRejectRfq}
					</div>
						);
				} else {
					return (
					<div id="menu-box" className="menu-box">
							{btnTakeOwnership}
					</div>
						);
				}
			}else if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'COUNTER'){
			if(this.props.controls.role === 'trader'){
				if(this.props.controls.userId === this.props.conversation.traderId){
					return (
					<div id="menu-box" className="menu-box">
							{btnExecute}
							{btnReject}
							{btnCalcOpt}
							{btnQuote}
					</div>
						);
				} else {
					return (
					<div id="menu-box" className="menu-box">
							{btnTakeOwnership}
					</div>
						);
				}
			}else if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'QUOTE'){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
						{btnOrder}
						{btnCounter}
						{btnCalcOpt}
						{btnCancel}
						{btnNewRfq}
					</div>
					);
			}
		} else if(this.props.conversation.status === 'QUOTE_REJECT'){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'ORDER'){
			if(this.props.controls.role === 'trader'){
				return (
					<div id="menu-box" className="menu-box">
							{btnExecute}
							{btnReject}
					</div>
						);
			}
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		} else if(this.props.conversation.status === 'ORDER_REJECT'){
		} else if(this.props.conversation.status === 'EXECUTE'){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
						{btnNewRfq}
						{btnClone}
						{btnClose}
						{btnPartialClose}
					</div>
					);
			}
		} else if(this.props.conversation.status === 'EXECUTE_ACK'){
			if(this.props.controls.role === 'client'){
				return (
					<div id="menu-box" className="menu-box">
							{btnNewRfq}
					</div>
						);
			}
		}
		return (<div/>);
	}

	clickConv(e){
		var conv = this.props.cache.conversations.find( i => {
			return i.id == e.currentTarget.id;
		});
		if(conv){
			this.conversationUpdate(conv);
		}
	}

	renderConv(id){
		if(this.props.cache.conversations){
			var conv = this.props.cache.conversations.find(conv => { return conv.id === id;});

			if(conv){
				var ll = this.renderLegLabel();
				var ld;
				var legs;
				if(conv.expandLeg && conv.legs){
					ld = conv.legs.map(leg => {
						return this.renderLegRO(leg);
					});
					legs = (
						<div className="conv-list-leg-list">
							{ll}
							{ld}
						</div>
					);
				}
				return (
					<div className="list-row" onClick={this.clickConv.bind(this)} id={conv.id}>
						<div className="list-row-detail">
							<div className="list-expand">
								<input
									id={conv.id}
									type="checkbox"
									value="true"
									checked={conv.legExpand}
									onChange={(e) => this.expandLegAction(conv.id, e)}
									className="expand-icon"
									/>
							</div>
							<div className="list-id">
								{conv.id}
							</div>
							<div className="list-tradedate">
								<Moment format="YYYY-MM-DD">{conv.tradeDate}</Moment>
							</div>
							<div className="list-status">
								{conv.status}
							</div>
							<div className="list-oc">
								{conv.oc}
							</div>
							<div className="list-side">
								{conv.side}
							</div>
							<div className="list-symbol">
								{conv.symbol}
							</div>
							<div className="list-strategy">
								{conv.strategy}
							</div>
							<div className="list-settldate">
								<Moment format="YYYY-MM-DD">{conv.settlementDate}</Moment>
							</div>
						</div>
						{legs}
					</div>
				);
			}
			return (
				<div className="conv-row" onClick={this.clickConv} id="0">
					<div className="conv-field">
					</div>
					<div className="conv-field">
					</div>
				</div>
			);
		}
	}

	renderDebug(){
		return (<div>
				<pre>
				Connection Status:{
					this.props.controls.connectionStatus
				}
				</pre>
				<pre>
				Remain:{
					this.props.conversation.quoteRemain
				}
				</pre>
				<pre>
				Login Status:{
					(this.props.controls.loginStatus ? "Login" : "Logout")
				}
				</pre>
				<pre>
				Session ID:{
					this.props.controls.sessionId
				}
				</pre>
				<pre>
				LS[0]:{
					(this.props.conversation.legs?this.props.conversation.legs[0].quantity:"na")
				}
				<pre>
				LS[1]:{
					(this.props.conversation.legs?this.props.conversation.legs[1].quantity:"na")
				}
				</pre>
				</pre>
				<pre>
				{
					JSON.stringify(this.props)
				}
				</pre>
				<pre>
				{
					JSON.stringify(this.state)
				}
				</pre>
				<pre>
				id:{
					this.props.conversation.id
				}
				</pre>
			</div>
		);
	}

	render() {
		var menu = this.renderMenu();
		var legLabel = this.renderLegLabel();
		var legs;

		var status = this.props.conversation.status;
		var role = this.props.controls.role;

		var stldtro = true;
		var trddtro = true;
		var sidero = true;
		var symbolro = true;
		var volro = true;
		var pricero = true;
		var dvdro = true;
		var strategyro = true;
		var riskro = true;

		if(status === "INIT" && role === "client"){
			stldtro = false;
			trddtro = false;
			sidero = false;
			symbolro = false;
			pricero = false;
			dvdro = false;
			riskro = false;
			strategyro = false;
		}
		if(status === "TAKE" && role === "trader"){
			volro = false;
		}

		var bsopt = ["Buy","Sell"].map((option) => 
			 <option value={option}>{option}</option>
		);

		var strategyopt = ["straddle","strangle","butterfly"].map((option) => 
			 <option value={option}>{option}</option>
		);

		if(this.props.conversation.legs && this.props.conversation.legs[0]){
			legs = this.props.conversation.legs.map(leg => {
				return this.renderLeg(leg);
			});
		}
		var listConv;
		if(this.props.cache.conversationIDs && this.props.cache.conversationIDs[0]){
			listConv = this.props.cache.conversations.map(c => {
				return this.renderConv(c.id);
			});
		}
		if(this.props.cache.conversations){
			listConv = this.props.cache.conversations.map(conv => {
				return this.renderConv(conv.id);
			});
		}

		var login;
		if(!this.props.controls.loginStatus){
			login = this.renderLogin();
		}

		var symbols;
		if(this.props && this.props.controls && this.props.controls.symbols){
			symbols = this.props.controls.symbols.map((symbol) => 
						<option value={symbol}>{symbol}</option>
			);
		}

		if(this.props && this.props.cache && this.props.cache.prices){
			symbols = this.props.cache.prices.map((price) => 
						<option value={price.symbol}>{price.symbol}</option>
			);
		}

		var debugNode;
		if(debug){
			debugNode = this.renderDebug();
		}
		var link;
		if(this.props.conversation.link_id > 0){
			link = (<div id="ticket-link-value" className="field-value">{this.props.conversation.link_id}</div>);
		} else {
			link = (<div id="ticket-link-value" className="field-value"></div>);
		}

		return (
			<div className="App">
				{login}
				{menu}
				<div id="ticket-box" className="ticket-box">
					<div id="ticket-box-header" className="ticket-header">
							<div id="ticket-id-box" className="ticket-field-box">
								<div id="ticket-id-label" className="field-label">ID:</div>
								<div id="ticket-id-value" className="field-value">{this.props.conversation.id}</div>
							</div>
							<div id="ticket-status-box" className="ticket-field-box">
								<div id="ticket-status-label" className="field-label">Status:</div>
								<div id="ticket-status-value" className="field-value">{this.props.conversation.status}</div>
							</div>
							<div id="ticket-oc-box" className="ticket-field-box">
								<div id="ticket-oc-label" className="field-label">OC:</div>
								<div id="ticket-oc-value" className="field-value">{this.props.conversation.oc}</div>
								{link}
							</div>
							<div id="ticket-tradedate-box" className="ticket-field-box">
								<div id="ticket-tradedate-label" className="field-label">Date:</div>
								<DateField
									selected={this.props.conversation.tradeDate}
									onChange={(value) => {alert('aa:'+value);this.tradeDateUpdateAction(value);}}
									id="ticket-tradedate-input"
									readonly={trddtro}
									className="field-input-date"
									/>
							</div>
							<div id="ticket-settl-box" className="ticket-field-box ticket-settl-box">
								<div id="ticket-settl-label" className="field-label">Settle't:</div>
								<DateField
									selected={this.props.conversation.settlementDate}
									onChange={(value) => this.settlementDateUpdateAction(value)}
									id="ticket-settl-input"
									readonly={stldtro}
									className="field-input-date"
									/>
							</div>
							<div id="ticket-remain-box" className="ticket-field-box">
								<div id="ticket-remain-label" className="field-label">Timer:</div>
								<div id="ticket-remain-value" className="field-value">{this.props.conversation.quoteRemain}</div>
							</div>
							<div id="ticket-side-box" className="ticket-field-box ticket-side-box">
								<div id="ticket-side-label" className="field-label">Side:</div>
								<SelectField
									name="side"
									value={this.props.conversation.side}
									type="text"
									onChange={(e) => this.sideUpdateAction(e)}
									id="ticket-settl-select"
									readonly={sidero}
									className="field-input-select"
									options={bsopt}
									/>
							</div>
							<div id="ticket-symbol-box" className="ticket-field-box">
								<div id="ticket-symbol-label" className="field-label">Symbol:</div>
								<SelectField
									name="symbol"
									value={this.props.conversation.symbol}
									type="text"
									onChange={(e) => this.symbolUpdateAction(e)}
									id="ticket-symbol-select"
									readonly={symbolro}
									className="field-input-select"
									options={symbols}
									/>
							</div>
							<div id="ticket-price-box" className="ticket-field-box">
								<div id="ticket-price-label" className="field-label">Price:</div>
								<InputField
									name="spotPrice"
									value={this.props.conversation.spotPrice}
									type="number"
									step={0.01}
									handleChange={this.spotPriceUpdateAction}
									id="ticket-price-input"
									readonly={pricero}
									className="field-input-decimal"
									/>
							</div>
							<div id="ticket-dvdyld-box" className="ticket-field-box">
								<div id="ticket-dvdyld-label" className="field-label">Div Yield:</div>
								<InputField
									name="dividendYield"
									value={this.props.conversation.dividendYield}
									type="number"
									step={0.01}
									handleChange={this.dividendYieldUpdateAction}
									id="ticket-dvdyld-input"
									readonly={dvdro}
									className="field-input-decimal-short"
									/>
							</div>
							<div id="ticket-strategy-box" className="ticket-field-box">
								<div id="ticket-strategy-label" className="field-label">Strategy:</div>
								<SelectField
									name="strategy"
									value={this.props.conversation.strategy}
									onChange={this.strategyUpdateAction}
									id="ticket-strategy-select"
									readonly={strategyro}
									className="field-input-select"
									options={strategyopt}
									/>
							</div>
							<div id="ticket-rfrate-box" className="ticket-field-box">
								<div id="ticket-rfrate-label" className="field-label">Risk Free:</div>
								<InputField
									name="riskFreeRate"
									value={this.props.conversation.riskFreeRate}
									type="number"
									step={0.01}
									handleChange={this.riskFreeRateUpdateAction}
									id="ticket-rfrate-input"
									readonly={riskro}
									className="field-input-decimal-short"
									/>
							</div>
					</div>
					<div className="conv-leg-list">
						{legLabel}
						{legs}
					</div>
				</div>
				<div id="list-base" className="list-base">
					<div id="list-header" className="list-header">
						<div id="list-header-expand" className="list-expand list-field"></div>
						<div id="list-header-id" className="list-id list-field">ID</div>
						<div id="list-header-tradedate" className="list-tradedate list-field">Trade Dt</div>
						<div id="list-header-status" className="list-status list-field">Status</div>
						<div id="list-header-oc" className="list-oc list-field">O/C</div>
						<div id="list-header-side" className="list-side list-field">Side</div>
						<div id="list-header-symbol" className="list-symbol list-field">Symbol</div>
						<div id="list-header-strategy" className="list-strategy list-field">Strategy</div>
						<div id="list-header-settldate" className="list-settldate list-field">Settl Dt</div>
					</div>
					<div className="list-view">
						{listConv}
					</div>
				</div>
				<div id="statusbar" className="statusbar">
					{this.props.controls.statusBar}
				</div>
				{debugNode}
			</div>
 		);
	}
}
export {conn};
export default connect(mapStateToProps, mapDispatchToProps)(App);
