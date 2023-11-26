#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ==============================================================================================
	// PixelFormatMergeParams methods
	// ==============================================================================================

	bool DoLoadFromJSON(JSONReadConfiguration config, PixelFormatMergeParams& dst)
	{
		JSONTools::GetAttribute(config, "pixel_format", dst.pixel_format);
		JSONTools::GetAttribute(config, "upgrade_pixel_format", dst.upgrade_pixel_format);
		JSONTools::GetAttribute(config, "accept_luminance", dst.accept_luminance);
		JSONTools::GetAttribute(config, "accept_float", dst.accept_float);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json * json, PixelFormatMergeParams const& src)
	{
		if (!PrepareSaveIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "pixel_format", src.pixel_format);
		JSONTools::SetAttribute(json, "upgrade_pixel_format", src.upgrade_pixel_format);
		JSONTools::SetAttribute(json, "accept_luminance", src.accept_luminance);
		JSONTools::SetAttribute(json, "accept_float", src.accept_float);
		return true;
	}

	// ==============================================================================================
	// PixelFormatMerger methods
	// ==============================================================================================

	PixelFormatMerger::PixelFormatMerger(PixelFormatMergeParams const & in_params) : params(in_params)
	{
		if (in_params.pixel_format.IsValid())
		{
			result = in_params.pixel_format;
			result_is_available = true;
		}
		else
			params.upgrade_pixel_format = true; // well, if no format is given as input, we want to take care of all incomming formats
	}

	void PixelFormatMerger::Reset(PixelFormatMergeParams const & in_params)
	{
		operator = (PixelFormatMerger(in_params)); // copy from a newly created instance
	}

	void PixelFormatMerger::Merge(PixelFormat src)
	{
		assert(src.IsValid());
		assert(!src.IsDepthStencilPixel());

		PixelFormat input_format = src;

		if (!params.accept_luminance && src.component_count == 1) // transform luminance into RGB
			src.component_count = 3;

		if (!params.accept_float && src.component_type == PixelComponentType::FLOAT) // transform float into unsigned char
			src.component_type = PixelComponentType::UNSIGNED_CHAR;

		if (!result_is_available)
		{
			result = src;
			result_is_available = true;
		}
		else if (params.upgrade_pixel_format)
		{
			result.component_type = std::max(result.component_type, src.component_type);
			result.component_count = std::max(result.component_count, src.component_count);
		}

		identical_incomming_format &= (input_format == result);
	}

}; // namespace chaos
