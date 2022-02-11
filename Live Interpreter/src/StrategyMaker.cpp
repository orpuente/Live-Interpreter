#include "pch.h"
#include "StrategyMaker.h"

struct NameValuePair
{
	double v;
	std::string n;
	std::string desc;
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

	static inline bool add_variable(const std::string& name, double& val)
	{
		return symbol_table.add_variable(name, val, false);
	}

	static inline bool add_variable(NameValuePair& var)
	{
		return symbol_table.add_variable(var.n, var.v, false);
	}

	static inline bool remove_variable(NameValuePair& var) {
		// If the symbol is contained in the expression, a re-compilation is necessary
		return symbol_table.remove_variable(var.n, true);
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
	using compositor_t = exprtk::function_compositor<T>;

	static compositor_t compositor;
	static symbol_table_t unknown_var_symbol_table;
	static symbol_table_t& symbol_table;
	std::string expression_string;
	expression_t expression;
	parser_t parser;
	const std::string default_value = std::format("{}", std::numeric_limits<double>::quiet_NaN());
	bool last_compilation_successful_p = false;
};

exprtk::function_compositor<double> EvaluatorWrapper::compositor = exprtk::function_compositor<double>();
exprtk::symbol_table<double>  EvaluatorWrapper::unknown_var_symbol_table = exprtk::symbol_table<double>();
exprtk::symbol_table<double>& EvaluatorWrapper::symbol_table = EvaluatorWrapper::compositor.symbol_table();


void Show_StrategyMaker()
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_NoHostExtendX;

	static char headersCol[2][16] = { "Flat", "Long" };

	// Goal: protect `p` from changes inside the evaluator
	// TODO: improve the horrendous solution below
	//static double p = 41.52;
	//static double p_ = p; // horrendous solution ^(O_O)^ But it works
	static std::vector<NameValuePair> variables = { {41.52, "p", "Price of the stock"} };

	static char temp_name[30];
	static char temp_desc[256];
	static double temp_value;

	if (static bool init = false; !init)
	{
		for (size_t i = 0; i < variables.size(); i++)
			EvaluatorWrapper::add_variable(variables[i]);
		init = true;
	}

	ImGui::Spacing();

	if (ImGui::Button("Add Variable##ad var btn")) {
		temp_name[0] = '\0';
		temp_desc[0] = '\0';
		temp_value = 0;
		ImGui::OpenPopup("add var popup");
	}
	
	// Popup to add new variables
	if (ImGui::BeginPopup("add var popup")) {
		ImGui::Text("Enter the variable's name: ");
		ImGui::SameLine();
		ImGui::InputText("##popup_var_name", temp_name, 30);

		ImGui::Text("Enter the variable's value (can be modified later): ");
		ImGui::SameLine();
		ImGui::InputDouble("##popup_var_value", &temp_value);

		ImGui::Text("Enter the variable's description: ");
		ImGui::SameLine();
		ImGui::InputText("##popup_var_desc", temp_desc, 256);

		ImGui::Spacing();

		if (ImGui::Button("Add")) {
			if (std::regex_match(temp_name, std::regex("^[a-zA-Z][a-zA-Z0-9_]*$"))) {
				// The variable name has the right format
				variables.push_back({ temp_value, temp_name, temp_desc });
				EvaluatorWrapper::add_variable(variables.back());
				ImGui::CloseCurrentPopup();
			}
			else {
				ImGui::OpenPopup("warning with message");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::BeginPopup("warning with message")) {
			ImGui::Text("Variable names must be made only of letters, numbers and \"_\" and cannot begin with a number");

			if (ImGui::Button("Ok"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::EndPopup();
	}


	for (size_t i = 0; i < variables.size(); i++) {
		ImGui::Text(std::format("Enter the value of {}: ", variables[i].n).c_str());
		ImGui::SameLine();
		ImGui::InputDouble(std::format("##{} input", variables[i].n).c_str(), &variables[i].v, .01, 1, "%.2f");
		ImGui::SameLine();
		if (ImGui::Button(std::format("Remove {}", variables[i].n).c_str())) {
			if (EvaluatorWrapper::remove_variable(variables[i])) {
				// If succesful at removing the variable, remove it from the vector as well
				variables.erase(variables.begin() + i);
			}
		}
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

		for (int row = 0; row < 2; row++)
		{
			for (int col = 0; col < 2; col++)
			{
				//p_ = p;
				static EvaluatorWrapper cell_evaluator[2][2];
				ImGui::Text(std::format("Entry {}{} evaluates to: {}", row, col,
				                        cell_evaluator[row][col](buf[row][col], buf_size)).c_str());
			}
		}
	}
}
