#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiDrawableInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
	}

	int ImGuiDrawableInterface::AddWindowMainMenuFlag(ImGuiDrawMenuMode menu_mode, int flags)
	{
		// only add a flags for the menu if it is not the global GLFW window
		if (menu_mode == ImGuiDrawMenuMode::ImGuiWindow)
			if (WindowApplication* window_application = Application::GetInstance())
				if (window_application->GetImGuiMenuMode()) // only add the menu bar if enabled
					flags |= ImGuiWindowFlags_MenuBar;
		return flags;
	}

}; // namespace chaos