#include "pch.h"
#include "implot.h"
#include "live_interpreter.h"
#include "StrategyMaker.h"


namespace ImPlot {

    void ShowDemo_Configuration() {
        ImGui::ShowFontSelector("Font");
        ImGui::ShowStyleSelector("ImGui Style");
        ImPlot::ShowStyleSelector("ImPlot Style");
        ImPlot::ShowColormapSelector("ImPlot Colormap");
        float indent = ImGui::CalcItemWidth() - ImGui::GetFrameHeight();
        ImGui::Separator();
        ImGui::Checkbox("Anti-Aliased Lines", &ImPlot::GetStyle().AntiAliasedLines);
        ImGui::Separator();
        ImGui::Checkbox("Use Local Time", &ImPlot::GetStyle().UseLocalTime);
        ImGui::Checkbox("Use ISO 8601", &ImPlot::GetStyle().UseISO8601);
        ImGui::Checkbox("Use 24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);
        ImGui::Unindent(indent);
    }

    //-----------------------------------------------------------------------------
    // WINDOWS
    //-----------------------------------------------------------------------------
    
    void ShowInterpreterWindow(bool* p_open) {

        ImGui::SetNextWindowPos(ImVec2(1920/2, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1920/2, 900), ImGuiCond_FirstUseEver);
        ImGui::Begin("Example Window", (bool*)0, ImGuiWindowFlags_MenuBar);

        ImGui::Spacing();

        if (ImGui::BeginTabBar("ImPlotDemoTabs")) {

            if (ImGui::BeginTabItem("Strategy Maker")) {
                Show_StrategyMaker();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Settings")) {
                ShowDemo_Configuration();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }


        ImGui::End();
    }

}