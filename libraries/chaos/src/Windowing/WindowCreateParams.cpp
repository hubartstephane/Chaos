#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DoSaveIntoJSON(nlohmann::json * json, WindowCreateParams const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "resizable", src.resizable);
		JSONTools::SetAttribute(json, "start_visible", src.start_visible);
		JSONTools::SetAttribute(json, "decorated", src.decorated);
		JSONTools::SetAttribute(json, "toplevel", src.toplevel);
		JSONTools::SetAttribute(json, "focused", src.focused);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, WindowCreateParams& dst)
	{
		JSONTools::GetAttribute(config, "resizable", dst.resizable);
		JSONTools::GetAttribute(config, "start_visible", dst.start_visible);
		JSONTools::GetAttribute(config, "decorated", dst.decorated);
		JSONTools::GetAttribute(config, "toplevel", dst.toplevel);
		JSONTools::GetAttribute(config, "focused", dst.focused);
		return true;
	}

}; // namespace chaos
