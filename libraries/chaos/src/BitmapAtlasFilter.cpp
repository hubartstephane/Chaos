#include <chaos/BitmapAtlasFilter.h>
#include <chaos/ImageTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/JSONTools.h>
#include <chaos/ImagePixelAccessor.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		// ==========================================================================
		// BitmapAtlasFilterCondition methods
		// ==========================================================================

		bool BitmapAtlasFilterCondition::LoadFromJSON(nlohmann::json const& entry)
		{

			return true;
		}

		bool BitmapAtlasFilterCondition::SaveIntoJSON(nlohmann::json& entry) const
		{

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

		bool LoadFromJSON(nlohmann::json const& entry, BitmapAtlasFilter & dst)
		{
			JSONTools::GetAttribute(entry, "condition", dst.condition);
			JSONTools::GetAttribute(entry, "processor", dst.processor);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json& entry, BitmapAtlasFilter const & src)
		{
			JSONTools::SetAttribute(entry, "condition", src.condition);
			JSONTools::SetAttribute(entry, "processor", src.processor);
			return true;
		}

		// ==========================================================================
		// BitmapAtlasFilterSet methods
		// ==========================================================================

		bool LoadFromJSON(nlohmann::json const& entry, BitmapAtlasFilterSet & dst)
		{
			JSONTools::GetAttribute(entry, "filters", dst.filters);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json& entry, BitmapAtlasFilterSet const & src)
		{
			JSONTools::SetAttribute(entry, "filters", src.filters);
			return true;
		}


	}; // namespace BitmapAtlas

}; // namespace chaos

