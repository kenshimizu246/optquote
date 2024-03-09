import React from 'react';
import ReactDOM from 'react-dom';
import PropTypes from 'prop-types';

export default class SelectField extends React.Component {
	render() {
		if(this.props.readonly){
			return (
          <select
						name={this.props.name}
						value={this.props.value}
						onChange={this.props.onChange}
            type="text"
						disabled="true"
						className={this.props.className}
            >
						{this.props.options}
          </select>
				);
		}
		return (
         <select
					name={this.props.name}
					value={this.props.value}
					onChange={this.props.onChange}
          type="text"
					className={this.props.className}
          >
					{this.props.options}
        </select>
			);
	}
}

SelectField.propTypes = {
	name: PropTypes.string,
	value: PropTypes.string,
	type: PropTypes.string,
	readonly: PropTypes.bool,
	onInput: PropTypes.func,
};

