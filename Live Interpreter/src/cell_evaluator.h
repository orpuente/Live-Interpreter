#pragma once
#include "pch.h"
#include "exprtk/exprtk.hpp"

struct nameValuePair {
	typedef double T;
	T v;
	const std::string n;
};


class CellEvaluator {
	typedef double T;
	typedef exprtk::symbol_table<T> symbol_table_t;
	typedef exprtk::expression<T>   expression_t;
	typedef exprtk::parser<T>       parser_t;

private:
	const T default_value = std::numeric_limits<T>::quiet_NaN();

	std::string expression_string;
	static symbol_table_t symbol_table;
	expression_t expression;
	parser_t parser;
	T last_value;

	void compile_and_update();
public:
	CellEvaluator(T p = 42.51);
	CellEvaluator(const char* buf, size_t buf_size, T p = 42.51);

	static void update_value(nameValuePair& var);
	static void update_value(const char* name, double val);

	std::string evaluate(const char* buf, size_t buf_size);
};

