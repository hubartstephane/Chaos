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
		if (content != in_content)
		{
			if (content != nullptr)
			{
				RemoveImGuiObject(content.get());
			}
			content = in_content;
			if (content != nullptr)
			{
				content->SetName("content");
				content->SetImGuiObjectFlags(ImGuiObjectFlags::FullViewport);
				AddImGuiObject(content.get());
			}
		}
	}

	void ImGuiWindow::DrawImGui()
	{
		Window::DrawImGui();
		// check whether the window as to be closed
		if (content != nullptr)
			if (content->IsClosingRequested())
				RequireWindowClosure();
	}

	bool ImGuiWindow::EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func) const
	{
		return false; // don't call super method
	}

}; // namespace chaos
