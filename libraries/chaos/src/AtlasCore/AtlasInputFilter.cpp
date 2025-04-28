#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================================================
	// AtlasInputFilterCondition methods
	// ==========================================================================

	bool AtlasInputFilterCondition::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!JSONSerializableInterface::SerializeFromJSON(config))
			return false;
		return true;
	}

	bool AtlasInputFilterCondition::SerializeIntoJSON(nlohmann::json* json) const
	{
		if (!JSONSerializableInterface::SerializeIntoJSON(json))
			return false;
		return true;
	}

	bool AtlasInputFilterCondition::AcceptBitmap(AtlasBitmapInfoInput const* input) const
	{
		return true;
	}

	bool AtlasInputFilterCondition::AcceptFont(AtlasFontInfoInput const* input) const
	{
		return true;
	}

	// ==========================================================================
	// AtlasInputFilter methods
	// ==========================================================================

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasInputFilter& dst)
	{
		JSONTools::GetAttribute(config, "condition", dst.condition);
		JSONTools::GetAttribute(config, "processor", dst.processor);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasInputFilter const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "condition", src.condition);
		JSONTools::SetAttribute(json, "processor", src.processor);
		return true;
	}

	// ==========================================================================
	// AtlasInputFilterSet methods
	// ==========================================================================

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasInputFilterSet& dst)
	{
		JSONTools::GetAttribute(config, "filters", dst.filters);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasInputFilterSet const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "filters", src.filters);
		return true;
	}

}; // namespace chaos

