#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ConsoleWindow::ConsoleWindow()
	{
		console_content.SetLogger(Logger::GetInstance());
	}

	bool ConsoleWindow::DoTick(float delta_time)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		if (ImGui::Begin("##console", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar))
		{
			console_content.DrawImGui();
			ImGui::End();
		}
		return true;
	}

}; // namespace chaos
