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
		if (!PrepareSaveObjectIntoJSON(json))
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
		assert(!IsDepthStencilFormat(in_params.pixel_format));

		if (IsValidFormat(in_params.pixel_format))
		{
			pixel_format = ApplyParametersToFormat(in_params.pixel_format);
			pixel_description = GetPixelDescription(pixel_format);
		}
	}

	PixelFormat PixelFormatMerger::ApplyParametersToFormat(PixelFormat in_pixel_format) const
	{
		if (IsValidFormat(in_pixel_format))
		{
			if (!params.accept_luminance)
			{
				if (in_pixel_format == PixelFormat::Gray)
					in_pixel_format = PixelFormat::BGR;
				else if (in_pixel_format == PixelFormat::GrayFloat)
					in_pixel_format = PixelFormat::RGBFloat;
			}

			if (!params.accept_float)
			{
				if (in_pixel_format == PixelFormat::RGBFloat)
					in_pixel_format = PixelFormat::BGR;
				else if (in_pixel_format == PixelFormat::RGBAFloat)
					in_pixel_format = PixelFormat::BGRA;
				else if (in_pixel_format == PixelFormat::GrayFloat)
					in_pixel_format = PixelFormat::Gray;
			}
		}
		return in_pixel_format;
	}

	void PixelFormatMerger::Reset(PixelFormatMergeParams const & in_params)
	{
		operator = (PixelFormatMerger(in_params)); // copy from a newly created instance
	}

	PixelFormat PixelFormatMerger::GetResult() const
	{
		return GetPixelFormat(pixel_description);
	}

	void PixelFormatMerger::Merge(PixelFormat in_pixel_format)
	{
		assert(IsValidFormat(in_pixel_format));
		assert(!IsDepthStencilFormat(in_pixel_format));

		in_pixel_format = ApplyParametersToFormat(in_pixel_format);

		if (!IsValidFormat(pixel_format)) // no result yet
		{
			pixel_format = in_pixel_format;
			pixel_description = GetPixelDescription(in_pixel_format);
		}
		else
		{
			PixelDescription in_pixel_description = GetPixelDescription(in_pixel_format);
			pixel_description.component_type  = std::max(pixel_description.component_type, in_pixel_description.component_type);
			pixel_description.component_count = std::max(pixel_description.component_count, in_pixel_description.component_count);
			// at this point, pixel_format does not match pixel_description, but this doesn't care because all we bother is that it is not 'Unknown'
		}
	}

}; // namespace chaos
