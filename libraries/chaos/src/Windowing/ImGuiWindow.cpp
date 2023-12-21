#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	WindowCategory ImGuiWindow::GetWindowCategory() const
	{
		return WindowCategory::OPTIONAL_WINDOW;
	};

	void ImGuiWindow::OnDrawWindowImGuiContent()
	{
		if (content != nullptr)
		{
			ImGuiInterface::FullscreenWindow("##console", true, [this]()
			{
				content->DrawImGui(ImGuiDrawMenuMode::ImGuiWindow);
			});
		}
	}

	void ImGuiWindow::OnDrawWindowImGuiMenu()
	{
		// Window::OnDrawWindowImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
