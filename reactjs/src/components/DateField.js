import React from 'react';
import ReactDOM from 'react-dom';
import PropTypes from 'prop-types';

import DatePicker from 'react-datepicker';
import moment from 'moment';
import MomentPropTypes from 'react-moment-proptypes';
import 'react-datepicker/dist/react-datepicker.css';
import Moment from 'react-moment';


export default class DateField extends React.Component {
	doWheel(e){
		console.log(e);
	}

	render() {
		if(this.props.readonly){
			var d = this.props.selected;
			if(this.props.selected){
				d = this.props.selected.format("MM/DD/YYYY");
			}
			return (
				<div
					className={this.props.className}
					>
					{d}
				</div>
				);
		}
		return (
				<DatePicker
					selected={this.props.selected}
					onChange={this.props.onChange}
					className={this.props.className}
					/>
		);
	}
}

DateField.propTypes = {
	name: PropTypes.string,
	selected: PropTypes.string,
	readonly: PropTypes.bool,
	onChange: PropTypes.func,
	className: PropTypes.string,
};

