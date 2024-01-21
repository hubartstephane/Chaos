#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiWindow::ImGuiWindow()
	{
		window_category = WindowCategory::OPTIONAL_WINDOW;
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

	void ImGuiWindow::OnDrawWindowImGuiMenu(ImGuiInterface::DrawImGuiMenuFunc func)
	{
		// Window::OnDrawWindowImGuiMenu(func);
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
