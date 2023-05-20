#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiDemoWindow::OnDrawWindowImGuiContent()
	{
		int window_flags = ImGuiDrawableInterface::AddWindowMainMenuFlag(ImGuiDrawMenuMode::ImGuiWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		if (ImGui::Begin("##imgui_demos", nullptr, window_flags))
		{
			if (show_demo)
				ImGui::ShowDemoWindow(&show_demo);
			if (show_metrics)
				ImGui::ShowMetricsWindow(&show_metrics);
			if (show_debug_log)
				ImGui::ShowDebugLogWindow(&show_debug_log);
			if (show_stack_tool)
				ImGui::ShowStackToolWindow(&show_stack_tool);
			if (show_about)
				ImGui::ShowAboutWindow(&show_about);
			if (show_style_editor)
				ImGui::ShowStyleEditor();
			if (show_user_guide)
				ImGui::ShowUserGuide();
			ImGui::End();
		}
	}

	void ImGuiDemoWindow::OnDrawWindowImGuiMenu()
	{
		// Window::OnDrawImGuiMenu();
		// do not call super so that the application items are not inserted here

		chaos::ImGuiDrawableInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this]()
		{
			if (ImGui::BeginMenu("ImGui Windows"))
			{
	#define CHAOS_IMGUI_MENUITEM(X) ImGui::MenuItem(#X, nullptr, &X, true);
				CHAOS_IMGUI_MENUITEM(show_demo);
				CHAOS_IMGUI_MENUITEM(show_metrics);
				CHAOS_IMGUI_MENUITEM(show_debug_log);
				CHAOS_IMGUI_MENUITEM(show_stack_tool);
				CHAOS_IMGUI_MENUITEM(show_about);
				CHAOS_IMGUI_MENUITEM(show_style_editor);
				CHAOS_IMGUI_MENUITEM(show_user_guide);
	#undef CHAOS_IMGUI_MENUITEM
				ImGui::EndMenu();
			}
		});
	}

}; // namespace chaos
