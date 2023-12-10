#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LogWindow::LogWindow()
	{
		console_content.SetLogger(Logger::GetInstance());
	}

	void LogWindow::OnDrawWindowImGuiContent()
	{
		ImGuiDrawableInterface::FullscreenWindow("##console", true, [this]()
		{
			console_content.DrawImGui(ImGuiDrawMenuMode::ImGuiWindow);
		});
	}

	void LogWindow::OnDrawWindowImGuiMenu()
	{
		// Window::OnDrawWindowImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
