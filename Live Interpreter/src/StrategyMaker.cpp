#include "pch.h"
#include "StrategyMaker.h"
#include "yen_evaluator.h"

std::string eval(const char* buf, const size_t buf_size)
{
	return yen_evaluator(buf, buf_size);
}

void Show_StrategyMaker()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	static constexpr size_t buf_size = 32;
	static char buf[2][2][buf_size];

	ImGui::Spacing();

    if (ImGui::BeginTable("table1", 2, flags))
    {
        // Display headers so we can inspect their interaction with borders.
        // (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)
        if (true)
        {
            ImGui::TableSetupColumn("FLAT");
            ImGui::TableSetupColumn("LONG");
            ImGui::TableHeadersRow();
        }

        for (int row = 0; row < 2; row++)
        {
			ImGui::TableNextRow();
            for (int col = 0; col < 2; col++)
            {
				ImGui::TableSetColumnIndex(col);
				ImGui::InputText(std::format("##{}{}", row, col).c_str(), buf[row][col], buf_size);	            
            }	        
        }

        ImGui::EndTable();

        for (int row = 0; row < 2; row++)
            for (int col = 0; col < 2; col++)
				ImGui::Text(std::format("Entry {}{} evals to: {}", row, col, eval(buf[row][col], buf_size)).c_str());
    }
}