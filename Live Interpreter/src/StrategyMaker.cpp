#include "pch.h"
#include "StrategyMaker.h"

struct NameValuePair {
	double v;
	std::string n;
};

class EvaluatorWrapper
{
public:
	EvaluatorWrapper()
	{
		reset_free_variables();
		unknown_var_symbol_table.add_constants();
		expression.register_symbol_table(unknown_var_symbol_table);
		expression.register_symbol_table(symbol_table);
		parser.enable_unknown_symbol_resolver();
	}

	// eval
	std::string operator() (const char* buf, const size_t buf_size)
	{
		const std::string new_buf(buf);
		if (expression_string != new_buf)
		{
			expression_string = new_buf;
			reset_free_variables();
			last_compilation_successful_p = parser.compile(expression_string, expression);
		}

		if (last_compilation_successful_p)
			return std::format("{}", static_cast<bool>(expression.value()));
		return default_value;
	}

	void add_variable(std::string name, double& val) {
		if (!symbol_table.is_variable(name)) {
			//reset_free_variables();
			symbol_table.add_variable(name, val);
		}
	}

	void reset_free_variables()
	{
		std::vector<std::string> variable_list;
		unknown_var_symbol_table.get_variable_list(variable_list);
		for (auto& var_name : variable_list)
		{
			T& v = unknown_var_symbol_table.variable_ref(var_name);
			v = T{};
		}
	}

private:
	typedef double T;
	typedef exprtk::symbol_table<T> symbol_table_t;
	typedef exprtk::expression<T>   expression_t;
	typedef exprtk::parser<T>       parser_t;
	symbol_table_t unknown_var_symbol_table;
	symbol_table_t symbol_table;
	std::string expression_string;
	expression_t expression;
	parser_t parser;
	const std::string default_value = std::format("{}", std::numeric_limits<double>::quiet_NaN());
	bool last_compilation_successful_p = false;
};

void Show_StrategyMaker()
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders		|
		ImGuiTableFlags_RowBg		|
		ImGuiTableFlags_NoHostExtendX;

	static constexpr size_t buf_size = 32;
	static char buf[2][2][buf_size];
	static char headersCol[2][16] = {"Flat", "Long"};
	static EvaluatorWrapper cell_evaluator[2][2];
	static bool init = false;
	static double p = 41.52;
	
	if (!init) {
		for (int row = 0; row < 2; row++)
			for (int col = 0; col < 2; col++)
				cell_evaluator[row][col].add_variable("p", p);
	}

	ImGui::Spacing();

	ImGui::Text("Enter the value of p: ");
	ImGui::InputDouble("##p input", &p, .5, 10, "%.2f");

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
			for (int col = 0; col < 2; col++)
			{
				ImGui::TableSetColumnIndex(col + 1); // col(0) are the headers
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::InputTextMultiline(std::format("##{}{}", row, col).c_str(), buf[row][col], buf_size, ImVec2(0, 100));
			}
		}

		ImGui::EndTable();

		for (int row = 0; row < 2; row++)
			for (int col = 0; col < 2; col++)
				ImGui::Text(std::format("Entry {}{} evals to: {}", row, col, cell_evaluator[row][col](buf[row][col], buf_size)).c_str());
	}
}