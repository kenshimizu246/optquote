import moment from 'moment';
var MockDate = require('mockdate');

import {m2n, n2m, t2d, t2zero, isBz, m2d} from './utils';


test('m2n.2m', () => {
	expect(m2n("2m")).toBe(2000);
});

test('m2n.21m', () => {
	expect(m2n("21m")).toBe(21000);
});

test('m2n.2.1m', () => {
	expect(m2n("2.1m")).toBe(2100);
});

test('m2n.2.1m', () => {
	expect(m2n("7")).toBe(7);
});

test('n2m.2m', () => {
	expect(n2m(2000)).toBe("2m");
});

test('n2m.2mm', () => {
	expect(n2m(2000000)).toBe("2mm");
});

test('n2m.2.1mm', () => {
	expect(n2m(2100000)).toBe("2.1mm");
});

test('n2m.2.111m', () => {
	expect(n2m(2111000)).toBe("2.111mm");
});

test('isBz.bizdate', () => {
	expect(isBz(moment("03/22/2019", "MM/DD/YYYY"))).toBe(true);
});

test('isBz.holiday', () => {
	expect(isBz(moment("03/21/2019", "MM/DD/YYYY"))).toBe(false);
});

test('isBz.sat', () => {
	expect(isBz(moment("03/23/2019", "MM/DD/YYYY"))).toBe(false);
});

test('isBz.sun', () => {
	expect(isBz(moment("03/24/2019", "MM/DD/YYYY"))).toBe(false);
});

test('t2d.t2', () => {
	MockDate.set('03/22/2019');
	var result = t2d("t2");
	var expected = moment("03/26/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('t2d.holiday', () => {
	MockDate.set('03/20/2019');
	var result = t2d("t2");
	var expected = moment("03/25/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('t2d.onholiday', () => {
	MockDate.set('03/21/2019');
	var result = t2d("t2");
	var expected = moment("03/25/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('t2d.onsat', () => {
	MockDate.set('03/23/2019');
	var result = t2d("t2");
	var expected = moment("03/26/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('t2d.t10', () => {
	MockDate.set('03/20/2019');
	var result = t2d("t10");
	var expected = moment("04/04/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('m2d.m1', () => {
	MockDate.set('02/20/2019');
	var result = m2d("1m");
	var expected = moment("03/20/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('m2d.m1.holiday', () => {
	MockDate.set('02/21/2019');
	var result = m2d("1m");
	var expected = moment("03/22/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('m2d.m1.sat', () => {
	MockDate.set('02/23/2019');
	var result = m2d("1m");
	var expected = moment("03/25/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('m2d.m1.sun', () => {
	MockDate.set('02/24/2019');
	var result = m2d("1m");
	var expected = moment("03/25/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});

test('m2d.m1.10m', () => {
	MockDate.set('02/24/2019');
	var result = m2d("10m");
	var expected = moment("12/24/2019", "MM/DD/YYYY");
	expected = t2zero(expected);
	expect(result.format()).toBe(expected.format());
});





