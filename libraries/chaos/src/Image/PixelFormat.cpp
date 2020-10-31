#include <chaos/Chaos.h>

namespace chaos
{

	// ==============================================================================================
	// PixelFormat methods
	// ==============================================================================================

	static std::vector<std::pair<PixelComponentType, char const*>> const pixel_component_type_encoding =
	{
		{ PixelComponentType::UNKNOWN, "UNKNOWN" },
		{ PixelComponentType::UNSIGNED_CHAR, "UNSIGNED_CHAR" },
		{ PixelComponentType::FLOAT, "FLOAT" },
		{ PixelComponentType::DEPTH_STENCIL, "DEPTH_STENCIL" }
	};

	CHAOS_IMPLEMENT_ENUMJSON_METHOD(PixelComponentType, pixel_component_type_encoding);

	// ==============================================================================================
	// PixelFormat methods
	// ==============================================================================================

	PixelFormat const PixelFormat::Gray			= PixelFormat(PixelComponentType::UNSIGNED_CHAR, 1);
	PixelFormat const PixelFormat::BGR			= PixelFormat(PixelComponentType::UNSIGNED_CHAR, 3);
	PixelFormat const PixelFormat::BGRA			= PixelFormat(PixelComponentType::UNSIGNED_CHAR, 4);
	PixelFormat const PixelFormat::GrayFloat	= PixelFormat(PixelComponentType::FLOAT, 1);
	PixelFormat const PixelFormat::RGBFloat		= PixelFormat(PixelComponentType::FLOAT, 3);
	PixelFormat const PixelFormat::RGBAFloat	= PixelFormat(PixelComponentType::FLOAT, 4);
	PixelFormat const PixelFormat::DepthStencil	= PixelFormat(PixelComponentType::DEPTH_STENCIL, 1);

	PixelFormat::PixelFormat(PixelFormatType in_format)
	{
		if (in_format == PixelFormatType::GRAY)
		{
			component_type = PixelComponentType::UNSIGNED_CHAR;
			component_count = 1;
		}
		else if (in_format == PixelFormatType::RGB)
		{
			component_type = PixelComponentType::UNSIGNED_CHAR;
			component_count = 3;
		}
		else if (in_format == PixelFormatType::RGBA)
		{
			component_type = PixelComponentType::UNSIGNED_CHAR;
			component_count = 4;
		}
		else if (in_format == PixelFormatType::GRAY_FLOAT)
		{
			component_type = PixelComponentType::FLOAT;
			component_count = 1;
		}
		else if (in_format == PixelFormatType::RGB_FLOAT)
		{
			component_type = PixelComponentType::FLOAT;
			component_count = 3;
		}
		else if (in_format == PixelFormatType::RGBA_FLOAT)
		{
			component_type = PixelComponentType::FLOAT;
			component_count = 4;
		}
		else if (in_format == PixelFormatType::DEPTH_STENCIL)
		{
			component_type = PixelComponentType::DEPTH_STENCIL;
			component_count = 1;
		}
	}

	PixelFormatType PixelFormat::GetFormat() const
	{
		if (component_type == PixelComponentType::UNSIGNED_CHAR)
		{
			if (component_count == 1)
				return PixelFormatType::GRAY;
			if (component_count == 3)
				return PixelFormatType::RGB;
			if (component_count == 4)
				return PixelFormatType::RGBA;
		}
		else if (component_type == PixelComponentType::FLOAT)
		{
			if (component_count == 1)
				return PixelFormatType::GRAY_FLOAT;
			if (component_count == 3)
				return PixelFormatType::RGB_FLOAT;
			if (component_count == 4)
				return PixelFormatType::RGBA_FLOAT;
		}
		else if (component_type == PixelComponentType::DEPTH_STENCIL)
		{
			return PixelFormatType::DEPTH_STENCIL;
		}
		return PixelFormatType::UNKNOWN;
	}

	int PixelFormat::GetPixelSize() const
	{
		if (IsValid())
		{
			if (component_type == PixelComponentType::UNSIGNED_CHAR)
				return component_count * sizeof(unsigned char);
			if (component_type == PixelComponentType::FLOAT)
				return component_count * sizeof(float);
			if (component_type == PixelComponentType::DEPTH_STENCIL)
				return sizeof(PixelDepthStencil);
		}
		return 0;	
	}

	bool PixelFormat::IsValid() const
	{
		return IsColorPixel() || IsDepthStencilPixel();
	}

	bool PixelFormat::IsColorPixel() const
	{
		if (component_type != PixelComponentType::UNSIGNED_CHAR && component_type != PixelComponentType::FLOAT)
			return false;
		if (component_count != 1 && component_count != 3 && component_count != 4)
			return false;
		return true;
	}

	bool PixelFormat::IsDepthStencilPixel() const
	{
		if (component_type == PixelComponentType::DEPTH_STENCIL)
			return true;
		return false;
	}

	bool PixelFormat::operator == (PixelFormat const & other) const
	{
		return (component_type == other.component_type) && (component_count == other.component_count);	
	}

	bool PixelFormat::operator != (PixelFormat const & other) const
	{
		return !operator == (other);
	}

	bool LoadFromJSON(nlohmann::json const& json_entry, PixelFormat& dst)
	{
		if (!json_entry.is_object())
			return false;
		JSONTools::GetAttribute(json_entry, "component_type", dst.component_type);
		JSONTools::GetAttribute(json_entry, "component_count", dst.component_count);
		return true;
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, PixelFormat const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "component_type", src.component_type);
		JSONTools::SetAttribute(json_entry, "component_count", src.component_count);
		return true;
	}

	// ==============================================================================================
	// PixelFormatMergeParams methods
	// ==============================================================================================

	bool LoadFromJSON(nlohmann::json const& json_entry, PixelFormatMergeParams& dst)
	{
		if (!json_entry.is_object())
			return false;

		JSONTools::GetAttribute(json_entry, "pixel_format", dst.pixel_format);
		JSONTools::GetAttribute(json_entry, "upgrade_pixel_format", dst.upgrade_pixel_format);
		JSONTools::GetAttribute(json_entry, "accept_luminance", dst.accept_luminance);
		JSONTools::GetAttribute(json_entry, "accept_float", dst.accept_float);
		return true;
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, PixelFormatMergeParams const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "pixel_format", src.pixel_format);
		JSONTools::SetAttribute(json_entry, "upgrade_pixel_format", src.upgrade_pixel_format);
		JSONTools::SetAttribute(json_entry, "accept_luminance", src.accept_luminance);
		JSONTools::SetAttribute(json_entry, "accept_float", src.accept_float);
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

	PixelFormat PixelFormatMerger::GetResult() const
	{
		return result;
	}

}; // namespace chaos
