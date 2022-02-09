#include "pch.h"
#include "StrategyMaker.h"
#include "yen_evaluator.h"
#include "CalcStar.h"

std::string cs_evaluator(const char* buf, const size_t buf_size)
{
	static CSTAR::CSFunctionEvaluator cs_evaluator;
	cs_evaluator.Set_strExpression(buf);
	int eval_code = cs_evaluator.Evaluate();

	if (eval_code == 1)
		return std::format("{}", cs_evaluator.Get_dblCurrValue());
	else
		return "syntax error";
}

void Show_StrategyMaker()
{
	static constexpr size_t buf_size = 32;
	static char buf[buf_size];	

	ImGui::Text("Text Parser");
	ImGui::InputTextMultiline("##Text parser", buf, buf_size);
	ImGui::NewLine();	

	ImGui::Text(std::format("Evaluates to: {}", cs_evaluator(buf, buf_size)).c_str());
}