#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LogWindow::LogWindow()
	{
		content = new ImGuiLogObject;
		if (content != nullptr)
			content->SetLogger(Logger::GetInstance());
	}

	void LogWindow::OnDrawWindowImGuiContent()
	{
		if (content != nullptr)
		{
			ImGuiInterface::FullscreenWindow("##console", true, [this]()
			{
				content->DrawImGui(ImGuiDrawMenuMode::ImGuiWindow);
			});
		}
	}

	void LogWindow::OnDrawWindowImGuiMenu()
	{
		// Window::OnDrawWindowImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
