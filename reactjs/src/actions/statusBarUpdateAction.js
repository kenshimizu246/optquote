export const statusBarUpdateAction = (v) => (dispatch, getState) => {
 dispatch({
  type: 'STATUS_BAR_UPDATE_ACTION',
  message: v
 })
}

