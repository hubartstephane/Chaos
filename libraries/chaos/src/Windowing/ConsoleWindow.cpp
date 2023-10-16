#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ConsoleWindow::ConsoleWindow()
	{
		console_content.SetLogger(Logger::GetInstance());
	}

	void ConsoleWindow::OnDrawWindowImGuiContent()
	{
		ImGuiDrawableInterface::FullscreenWindow("##console", true, [this]()
		{
			console_content.DrawImGui(ImGuiDrawMenuMode::ImGuiWindow);
		});
	}

	void ConsoleWindow::OnDrawWindowImGuiMenu()
	{
		// Window::OnDrawImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

}; // namespace chaos
