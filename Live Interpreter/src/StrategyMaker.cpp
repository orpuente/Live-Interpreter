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
	static T default_value = std::numeric_limits<T>::quiet_NaN();
	static T last_value = default_value;

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
			last_value = default_value;
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
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders		|
		ImGuiTableFlags_RowBg		|
		ImGuiTableFlags_NoHostExtendX;

	static constexpr size_t buf_size = 32;
	static char buf[2][2][buf_size];
	static char headersCol[2][16] = {"Flat", "Long"};

	ImGui::Spacing();
	
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
	if (ImGui::BeginTable("table1", 3, flags))
	{
		// Display headers so we can inspect their interaction with borders.
		// (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)

		ImGui::TableSetupColumn("");
		ImGui::TableSetupColumn("Flat");
		ImGui::TableSetupColumn("Long");
		ImGui::TableHeadersRow();
		for (int row = 0; row < 2; row++)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader(headersCol[row]);
			for (int col = 1; col < 2+1; col++)
			{
				ImGui::TableSetColumnIndex(col);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::InputTextMultiline(std::format("##{}{}", row, col).c_str(), buf[row][col], buf_size, ImVec2(0, 100));
			}
		}

		ImGui::EndTable();

		for (int row = 0; row < 2; row++)
			for (int col = 0; col < 2; col++)
				ImGui::Text(std::format("Entry {}{} evals to: {}", row, col, exprtk_evaluator(buf[row][col], buf_size)).c_str());
	}
}