#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumBitmaskMetaData<ImGuiDrawFlags> const ImGuiDrawFlags_bitmask_metadata =
	{
		{ImGuiDrawFlags::FULL_WINDOW, ImGuiDrawFlags::FLOATING_IMGUI_WINDOW}
	};

	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiDrawFlags, &ImGuiDrawFlags_bitmask_metadata, CHAOS_API);

	void ImGuiInterface::OnDrawImGuiContent()
	{
	}

	void ImGuiInterface::OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func)
	{
	}

}; // namespace chaos