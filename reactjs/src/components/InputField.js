import React from 'react';
import ReactDOM from 'react-dom';
import PropTypes from 'prop-types';

export default class InputField extends React.Component {
	doWheel(e){
		console.log(e);
	}

	render() {
		if(this.props.readonly){
			return (
				<input
					name={this.props.name}
					value={this.props.value}
					type={this.props.type}
					step={this.props.step}
					readonly="true"
					onMouseWheel={this.doWheel()}
					onChange={this.props.handleChange}
					className={this.props.className}
					/>
				);
		}
		return (
			<input
				name={this.props.name}
				value={this.props.value}
				type={this.props.type}
				step={this.props.step}
				onMouseWheel={this.doWheel()}
				onChange={this.props.handleChange}
				className={this.props.className}
				/>
		);
	}
}

InputField.propTypes = {
	name: PropTypes.string,
	value: PropTypes.string,
	type: PropTypes.string,
	step: PropTypes.number,
	readonly: PropTypes.bool,
	disabled: PropTypes.string,
	handleChange: PropTypes.func,
};

