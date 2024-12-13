#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<ImGuiStyle> const ImGuiStyle_metadata =
	{
		{ ImGuiStyle::Dark, "Dark" },
		{ ImGuiStyle::Light, "Light" },
		{ ImGuiStyle::Classic, "Classic" }
	};
	CHAOS_IMPLEMENT_ENUM_METHOD(ImGuiStyle, &ImGuiStyle_metadata, CHAOS_API);

	bool ImGuiManager::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		JSONTools::GetAttribute(config, "Style", style);
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
		
		switch (style)
		{
		case ImGuiStyle::Dark: ImGui::StyleColorsDark(); break;
		case ImGuiStyle::Light: ImGui::StyleColorsLight(); break;
		case ImGuiStyle::Classic: ImGui::StyleColorsClassic(); break;
		}
	





	}

}; // namespace chaos