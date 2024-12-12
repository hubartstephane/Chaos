#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ImGuiManager::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		int result = 0;

		JSONTools::GetAttribute(config, "test", result);

		return true;
	}

	bool ImGuiManager::DoStartManager()
	{
		// super method
		if (!ResourceManager::DoStartManager())
			return false;
		// read the properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::INITIALIZATION, false))
			return false;

		return true;
	}

	void ImGuiManager::InitializeImGuiContext(Window * window) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;  // Do not capture keyboard during navigation
		ImGui::StyleColorsDark();
	}

}; // namespace chaos