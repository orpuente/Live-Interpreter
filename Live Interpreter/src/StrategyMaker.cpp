#include "pch.h"
#include "StrategyMaker.h"
#include "yen_evaluator.h"
#include "exprtk/exprtk.hpp"

std::string exprtk_evaluator(const char* buf, const size_t buf_size)
{
	typedef double T;
	typedef exprtk::symbol_table<T> symbol_table_t;
	typedef exprtk::expression<T>   expression_t;
	typedef exprtk::parser<T>       parser_t;

	static bool init = false;
	static std::string expression_string;
	static T p = 42.51;
	static symbol_table_t symbol_table;
	static expression_t expression;
	static parser_t parser;
	static T last_value = std::numeric_limits<T>::quiet_NaN();

	static T x = 0, y = 0, z = 0;

	if (!init)
	{
		symbol_table.add_variable("p", p);
		symbol_table.add_variable("x", x);
		symbol_table.add_variable("y", y);
		symbol_table.add_variable("z", z);
		symbol_table.add_constants();
		init = true;
		expression.register_symbol_table(symbol_table);
	}

	const std::string new_exp(buf);

	if (new_exp != expression_string)
	{
		x = 0;
		y = 0;
		z = 0;
		expression_string = new_exp;
		if(!parser.compile(expression_string, expression))
		{
			// compilation error
			last_value = std::numeric_limits<T>::quiet_NaN();
		}
		else
		{
			last_value = expression.value();
		}
	}

	return std::format("{}", last_value);
}

void Show_StrategyMaker()
{
	static constexpr size_t buf_size = 64;
	static char buf[buf_size];	

	ImGui::Text("Text Parser");
	ImGui::InputTextMultiline("##Text parser", buf, buf_size);
	ImGui::NewLine();	

	ImGui::Text(std::format("Evaluates to: {}", exprtk_evaluator(buf, buf_size)).c_str());
}