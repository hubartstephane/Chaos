#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiWindow::ImGuiWindow()
	{
		window_category = WindowCategory::OPTIONAL_WINDOW;
		SetApplicationImGuiMenuPluggedIn(false);
	}

	void ImGuiWindow::SetContent(ImGuiObject* in_content)
	{
		content = in_content;
	}

	void ImGuiWindow::OnDrawImGuiContent()
	{
		Window::OnDrawImGuiContent();
		if (content != nullptr)
			content->DrawImGui(GetName(), ImGuiDrawFlags::FULL_WINDOW);
	}

	bool ImGuiWindow::EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func) const
	{
		return false; // don't call super method
	}

}; // namespace chaos
