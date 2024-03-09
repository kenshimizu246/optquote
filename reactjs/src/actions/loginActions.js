
export const showLoginAction = (v) => dispatch => {
 dispatch({
  type: 'SHOW_LOGIN_ACTION',
	showLogin: v,
 })
}

export const userIdUpdateAction = (v) => (dispatch, getState) => {
 dispatch({
  type: 'USERID_UPDATE_ACTION',
  userId: v
 })
}

export const passwdUpdateAction = (v) => (dispatch, getState) => {
 dispatch({
  type: 'PASSWD_UPDATE_ACTION',
  passwd: v
 })
}

export const roleUpdateAction = (v) => (dispatch, getState) => {
 dispatch({
  type: 'ROLE_UPDATE_ACTION',
  role: v
 })
}

export const loginStatusAction = (msg) => dispatch => {
	var loginStatus;
	if(msg.body.sessionId && msg.body.sessionId != "ERROR"){
		loginStatus = true;
	} else {
		loginStatus = false;
	}
 dispatch({
  type: 'LOGIN_STATUS_ACTION',
  sessionId: msg.body.sessionId,
	loginStatus: loginStatus,
 })
}


