#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DoSaveIntoJSON(nlohmann::json* json, WindowPlacementInfo const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "monitor_index", src.monitor_index);
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "size", src.size);
		JSONTools::SetAttribute(json, "fullscreen", src.fullscreen);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, WindowPlacementInfo& dst)
	{
		JSONTools::GetAttribute(config, "monitor_index", dst.monitor_index);
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "size", dst.size);
		JSONTools::GetAttribute(config, "fullscreen", dst.fullscreen);
		return true;
	}

}; // namespace chaos
