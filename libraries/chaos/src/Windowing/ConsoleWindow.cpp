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
		int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;
		if (imgui_menu_mode)
			window_flags |= ImGuiWindowFlags_MenuBar;

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		if (ImGui::Begin("##console", nullptr, window_flags))
		{
			if (imgui_menu_mode)
				console_content.DrawImGuiMenu();
			console_content.DrawImGui();
			ImGui::End();
		}
		return true;
	}

	void ConsoleWindow::DrawImGuiMenu(WindowDrawParams const& draw_params)
	{
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
