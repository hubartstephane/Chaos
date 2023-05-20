#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ConsoleWindow::ConsoleWindow()
	{
		console_content.SetLogger(Logger::GetInstance());
	}

	void ConsoleWindow::OnDrawImGuiContent(ImGuiDrawMenuMode menu_mode)
	{
		int window_flags = ImGuiDrawableInterface::AddWindowMainMenuFlag(ImGuiDrawMenuMode::ImGuiWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		if (ImGui::Begin("##console", nullptr, window_flags))
		{
			console_content.DrawImGui(ImGuiDrawMenuMode::ImGuiWindow);
			ImGui::End();
		}
	}

	void ConsoleWindow::OnDrawImGuiMenu(ImGuiDrawMenuMode menu_mode)
	{
		// Window::OnDrawImGuiMenu(menu_mode);
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
