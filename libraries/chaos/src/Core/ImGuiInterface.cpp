#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
	}

	int ImGuiInterface::GetConditionalMainMenuFlag(ImGuiDrawMenuMode menu_mode)
	{
		// only add a flags for the menu if it is not the global GLFW window
		if (menu_mode == ImGuiDrawMenuMode::ImGuiWindow)
			if (WindowApplication* window_application = Application::GetInstance())
				if (window_application->GetImGuiMenuMode()) // only add the menu bar if enabled
					return ImGuiWindowFlags_MenuBar;
		return 0;
	}

	void ImGuiInterface::FullscreenWindow(char const* name, bool menu_bar, LightweightFunction<void()> content_func)
	{
		int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;
		if (menu_bar)
			window_flags |= GetConditionalMainMenuFlag(ImGuiDrawMenuMode::ImGuiWindow);

		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		if (ImGui::Begin(name, nullptr, window_flags))
		{
			content_func();
			ImGui::End();
		}
	}

	bool ImGuiInterface::MenuBar(ImGuiDrawMenuMode menu_mode, LightweightFunction<void()> func)
	{
		bool show_menu = true;
		if (WindowApplication* window_application = Application::GetInstance())
			show_menu = window_application->GetImGuiMenuMode();

		if (show_menu)
		{
			switch (menu_mode)
			{
			case ImGuiDrawMenuMode::FullWindow:
				if (ImGui::BeginMainMenuBar())
				{
					func();
					ImGui::EndMainMenuBar();
					return true;
				}
				break;
			case ImGuiDrawMenuMode::ImGuiWindow:
				if (ImGui::BeginMenuBar())
				{
					func();
					ImGui::EndMenuBar();
					return true;
				}
				break;
			default:
				assert(0);
				break;
			}
		}
		return false;
	}


}; // namespace chaos