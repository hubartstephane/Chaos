#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiWindow::ImGuiWindow()
	{
		window_category = WindowCategory::OPTIONAL_WINDOW;
		EnableApplicationImGuiMenu(false);
	}

	void ImGuiWindow::SetContent(ImGuiObject* in_content)
	{
		content = in_content;
	}

	void ImGuiWindow::OnDrawWindowImGuiContent()
	{
		Window::OnDrawWindowImGuiContent();
		if (content != nullptr)
			content->DrawImGui(GetName(), ImGuiDrawFlags::FULL_WINDOW);
	}

	void ImGuiWindow::RegisterImGuiProxies()
	{
		// Window::RegisterImGuiProxies() // don't call super method
	}

}; // namespace chaos
