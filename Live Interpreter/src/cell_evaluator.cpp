#include "pch.h"
#include "cell_evaluator.h"

exprtk::symbol_table<double> CellEvaluator::symbol_table;

void CellEvaluator::compile_and_update() {
	expression.register_symbol_table(symbol_table);
	if (!parser.compile(expression_string, expression))
	{
		// compilation error
		last_value = default_value;
	}
	else
	{
		last_value = expression.value();
	}
}


CellEvaluator::CellEvaluator(double p)
	: expression_string(""), last_value(default_value)
{
	if (symbol_table.variable_count() == 0) {
		symbol_table.add_variable("p", p);
		symbol_table.add_constants();
	}
	expression.register_symbol_table(symbol_table);
}

CellEvaluator::CellEvaluator(const char* buf, size_t buf_size, double p)
	: expression_string(buf), last_value(default_value)
{
	if (symbol_table.variable_count() == 0) {
		symbol_table.add_variable("p", p);
		symbol_table.add_constants();
	}
	expression.register_symbol_table(symbol_table);
}

void CellEvaluator::update_value(nameValuePair& var) {
	if (symbol_table.is_variable(var.n)) {
		symbol_table.variable_ref(var.n) = var.v;
	}
	else {
		symbol_table.add_variable(var.n, var.v);
	}
}

void CellEvaluator::update_value(const char* name, double value) {
	if (symbol_table.is_variable(name)) {
		symbol_table.variable_ref(name) = value;
		//return std::format("Updating variable {} to ${.2f}\n", name, value);
	}
	else {
		symbol_table.add_variable(name, value);
		//return std::format("Creating variable {} to ${.2f}\n", name, value);
	}
}

std::string CellEvaluator::evaluate(const char* buf, size_t buf_size){
	const std::string new_exp(buf);

	if (new_exp != expression_string)
	{
		expression_string = new_exp;
		compile_and_update();
	}
	else {
		expression.register_symbol_table(symbol_table);
		last_value = expression.value();
	}

	return std::format("{}", last_value);
}