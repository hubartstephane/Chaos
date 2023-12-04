#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace BitmapAtlas
	{
		// ==========================================================================
		// BitmapAtlasFilterCondition methods
		// ==========================================================================

		bool BitmapAtlasFilterCondition::SerializeFromJSON(nlohmann::json const * json)
		{
			if (!JSONSerializableInterface::SerializeFromJSON(json))
				return false;
			return true;
		}

		bool BitmapAtlasFilterCondition::SerializeIntoJSON(nlohmann::json * json) const
		{
			if (!JSONSerializableInterface::SerializeIntoJSON(json))
				return false;
			return true;
		}

		bool BitmapAtlasFilterCondition::AcceptBitmap(BitmapInfoInput const* input) const
		{
			return true;
		}

		bool BitmapAtlasFilterCondition::AcceptFont(FontInfoInput const* input) const
		{
			return true;
		}

		// ==========================================================================
		// BitmapAtlasFilter methods
		// ==========================================================================

		bool DoLoadFromJSON(JSONReadConfiguration config, BitmapAtlasFilter & dst)
		{
			JSONTools::GetAttribute(config, "condition", dst.condition);
			JSONTools::GetAttribute(config, "processor", dst.processor);
			return true;
		}

		bool DoSaveIntoJSON(nlohmann::json * json, BitmapAtlasFilter const & src)
		{
			if (!PrepareSaveObjectIntoJSON(json))
				return false;
			JSONTools::SetAttribute(json, "condition", src.condition);
			JSONTools::SetAttribute(json, "processor", src.processor);
			return true;
		}

		// ==========================================================================
		// BitmapAtlasFilterSet methods
		// ==========================================================================

		bool DoLoadFromJSON(JSONReadConfiguration config, BitmapAtlasFilterSet & dst)
		{
			JSONTools::GetAttribute(config, "filters", dst.filters);
			return true;
		}

		bool DoSaveIntoJSON(nlohmann::json * json, BitmapAtlasFilterSet const & src)
		{
			if (!PrepareSaveObjectIntoJSON(json))
				return false;
			JSONTools::SetAttribute(json, "filters", src.filters);
			return true;
		}


	}; // namespace BitmapAtlas

}; // namespace chaos

