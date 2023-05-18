#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiDrawableInterface::OnDrawImGuiContent()
	{
	}

	void ImGuiDrawableInterface::OnDrawImGuiMenu()
	{
	}

	void ImGuiDrawableInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		// show the menu
		MenuBar(menu_mode, [this]()
		{
			OnDrawImGuiMenu();
		});
		// show the content
		OnDrawImGuiContent();
	}

}; // namespace chaos