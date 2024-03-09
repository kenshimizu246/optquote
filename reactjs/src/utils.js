import moment from 'moment';

export const m2n = (v) => {
	if(typeof(v) === 'string'){
		if(v.match(/^\d+\.*\d*m+$/i)){
			var n = v.match(/\d+\.*\d*/);
			var l = v.match(/m+/i);
			return n * (l.length * 1000);
		} else if(v.match(/\d+\.*\d*/)){
			var n = v.match(/\d+\.*\d*/);
			return Number(n);
		}
	}
	return v;
}

export const n2m = (v) => {
	if(typeof(v) === 'number' && v > 1000){
		var n = v;
		var m = "";
		while(n > 1000){
			n = n / 1000;
			m = m + "m";
		}
		return n + m;
	}
	return v;
}

export const t2zero = (v) => {
	if(moment.isMoment(v)){
		var t = v;
		t = t.set('millisecond', 0);
		t = t.set('second', 0);
		t = t.set('minute',0);
		t = t.set('hour',0);
		return t;		
	}
	return v;
}

export const isBz = (v) => {
	var fmt = "MM/DD/YYYY";
	var holidays = {};
	holidays["03/21/2019"] = moment("03/21/2019", fmt);

	if(moment.isMoment(v)){
		if(v.isoWeekday() === 7 || v.isoWeekday() === 6){
			return false;
		}
		if(v.isSame(holidays[v.format(fmt)])){
			return false;
		}
	}
	return true;
}

export const t2d = (v) => {
	if(typeof(v) === 'string'){
		if(v.match(/^t\d+$/i)){
			var n = v.match(/\d+/);
			var t = moment();
			t = t2zero(t);
			while(n > 0){
				t = t.add(1, "d");
				if(isBz(t)){
					n--;
				}
			}
			return t;
		}
	}
	return v;
}

export const m2d = (v) => {
	if(typeof(v) === 'string'){
		if(v.match(/^\d+m$/i)){
			var n = v.match(/^\d+/);
			var t = moment();
			t = t.add(Number(n), "M");
			while(!isBz(t)){
				t = t.add(1, "d");
			}
			return t;
		}
	}
	return v;
}


