#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ImGuiStandardMenuBuilder::WithMenu(LightweightFunction<void()> func) const
	{
		if (ImGui::BeginMenuBar())
		{
			func();
			ImGui::EndMenuBar();
			return true;
		}
		return false;
	}

	bool ImGuiMainMenuBuilder::WithMenu(LightweightFunction<void()> func) const
	{
		if (ImGui::BeginMainMenuBar())
		{
			func();
			ImGui::EndMainMenuBar();
			return true;
		}
		return false;
	}

}; // namespace chaos