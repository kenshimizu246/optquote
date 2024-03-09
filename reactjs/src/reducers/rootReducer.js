import { combineReducers } from 'redux';
import conversation from './conversation';
import controls from './controls';
import cache from './cache';

export default combineReducers({
	conversation,
	cache,
	controls,
});
