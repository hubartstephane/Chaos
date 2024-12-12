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

}; // namespace chaos