export default (state = {}, action) => {
 switch (action.type) {
  case 'CONNECTION_STATUS_ACTION':
   return {
		...state,
    connectionStatus: action.connectionStatus,
   }
  case 'LOGIN_STATUS_ACTION':
   return {
		...state,
    loginStatus: action.loginStatus,
    sessionId: action.sessionId,
    showLogin: false,
   }
  case 'SHOW_LOGIN_ACTION':
   return {
		...state,
    showLogin: action.showLogin,
   }
  case 'USERID_UPDATE_ACTION':
   return {
		...state,
    userId: action.userId,
   }
  case 'PASSWD_UPDATE_ACTION':
   return {
		...state,
    passwd: action.passwd,
   }
  case 'ROLE_UPDATE_ACTION':
   return {
		...state,
    role: action.role,
   }
  case 'SYMBOL_LIST_ACTION':
   return {
		...state,
    symbols: action.symbols,
   }
  case 'PRICE_LIST_ACTION':
   return {
		...state,
    prices: action.prices,
   }
  case 'CONVERSATION_LIST_ACTION':
   return {
		...state,
    conversations: action.conversations,
   }
  case 'STATUS_BAR_UPDATE_ACTION':
   return {
		...state,
    statusBar: action.message,
   }
  default:
   return state
 }
}
