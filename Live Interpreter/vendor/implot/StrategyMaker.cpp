#include "pch.h"
#include "StrategyMaker.h"
#include "yen_evaluator.h"


std::string eval(const char* buf, const size_t buf_size)
{
	return yen_evaluator(buf, buf_size);
}

void Show_StrategyMaker()
{
	static constexpr size_t buf_size = 32;
	static char buf[buf_size];

	ImGui::Text("Text Parser");
	ImGui::InputTextMultiline("##Text parser", buf, buf_size);
	ImGui::NewLine();
	ImGui::Text(std::format("Evaluates to: {}", eval(buf, buf_size)).c_str());
}

