#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiDrawableInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
	}

	int ImGuiDrawableInterface::GetConditionalMainMenuFlag(ImGuiDrawMenuMode menu_mode)
	{
		// only add a flags for the menu if it is not the global GLFW window
		if (menu_mode == ImGuiDrawMenuMode::ImGuiWindow)
			if (WindowApplication* window_application = Application::GetInstance())
				if (window_application->GetImGuiMenuMode()) // only add the menu bar if enabled
					return ImGuiWindowFlags_MenuBar;
		return 0;
	}

}; // namespace chaos