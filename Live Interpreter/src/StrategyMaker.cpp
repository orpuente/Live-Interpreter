#include "pch.h"
#include "StrategyMaker.h"

struct NameValuePair
{
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
	std::string operator()(const char* buf, const size_t buf_size)
	{
		const std::string new_buf(buf);
		if (expression_string != new_buf)
		{
			expression_string = new_buf;
			reset_free_variables();
			last_compilation_successful_p = parser.compile(expression_string, expression);
		}

		if (last_compilation_successful_p)
			return std::format("{}", expression.value());
		return default_value;
	}

	static void add_variable(const std::string& name, double& val)
	{
		symbol_table.add_variable(name, val, false);
	}

	static void update_variable(const std::string& name, const double val)
	{
		if (symbol_table.symbol_exists(name))
		{
			symbol_table.variable_ref(name) = val;
			std::cout << "updating " << name << "\n";
		}
	}

	static void reset_free_variables()
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
	using T = double;
	using symbol_table_t = exprtk::symbol_table<T>;
	using expression_t = exprtk::expression<T>;
	using parser_t = exprtk::parser<T>;
	static symbol_table_t unknown_var_symbol_table;
	static symbol_table_t symbol_table;
	std::string expression_string;
	expression_t expression;
	parser_t parser;
	const std::string default_value = std::format("{}", std::numeric_limits<double>::quiet_NaN());
	bool last_compilation_successful_p = false;
};

exprtk::symbol_table<double> EvaluatorWrapper::unknown_var_symbol_table = exprtk::symbol_table<double>();
exprtk::symbol_table<double> EvaluatorWrapper::symbol_table = exprtk::symbol_table<double>();


void Show_StrategyMaker()
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_NoHostExtendX;

	static char headersCol[2][16] = {"Flat", "Long"};

	// Goal: protect `p` from changes inside the evaluator
	// TODO: improve the horrendous solution below
	static double p = 41.52;
	static double p_ = p;    // horrendous solution ^(O_O)^ But it works

	if (static bool init = false; !init)
	{
		EvaluatorWrapper::add_variable("p", p_);
		init = true;
	}

	ImGui::Spacing();

	ImGui::Text("Enter the value of p: ");
	if (ImGui::InputDouble("##p input", &p, .01, 1, "%.2f"))
	{
		
	}

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
	if (ImGui::BeginTable("table1", 3, flags))
	{
		static constexpr size_t buf_size = 64;
		static char buf[2][2][buf_size];

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
				ImGui::InputTextMultiline(std::format("##{}{}", row, col).c_str(), buf[row][col], buf_size,
				                          ImVec2(0, 100));
			}
		}

		ImGui::EndTable();

		for (int row = 0; row < 2; row++) {
			for (int col = 0; col < 2; col++) {
				p_ = p;
				static EvaluatorWrapper cell_evaluator[2][2];
				ImGui::Text(std::format("Entry {}{} evaluates to: {}", row, col,
					cell_evaluator[row][col](buf[row][col], buf_size)).c_str());
			}
		}
	}
}
