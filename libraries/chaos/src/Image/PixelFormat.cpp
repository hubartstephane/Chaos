#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ==============================================================================================
	// PixelComponentType methods
	// ==============================================================================================

	static EnumMetaData<PixelComponentType> const PixelComponentType_metadata =
	{
		{ PixelComponentType::Unknown, "UNKNOWN" },
		{ PixelComponentType::UnsignedChar, "UNSIGNED_CHAR" },
		{ PixelComponentType::Float, "FLOAT" },
		{ PixelComponentType::DepthStencil, "DEPTH_STENCIL" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(PixelComponentType, &PixelComponentType_metadata, CHAOS_API);

	// ==============================================================================================
	// PixelFormat methods
	// ==============================================================================================

	static EnumMetaData<PixelFormat> const PixelFormat_metadata =
	{
		{ PixelFormat::Unknown, "Unknown" },
		{ PixelFormat::BGR, "BGR" },
		{ PixelFormat::BGRA, "BGRA" },
		{ PixelFormat::Gray, "Gray" },
		{ PixelFormat::RGBFloat, "RGBFloat" },
		{ PixelFormat::RGBAFloat, "RGBAFloat" },
		{ PixelFormat::GrayFloat, "GrayFloat" },
		{ PixelFormat::DepthStencil, "DepthStencil" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(PixelFormat, &PixelFormat_metadata, CHAOS_API);


	bool IsValidFormat(PixelFormat pixel_format)
	{
		return pixel_format != PixelFormat::Unknown;
	}

	bool IsColorFormat(PixelFormat pixel_format)
	{
		return 
			(pixel_format == PixelFormat::BGR) ||
			(pixel_format == PixelFormat::BGRA) ||
			(pixel_format == PixelFormat::Gray) ||
			(pixel_format == PixelFormat::RGBFloat) ||
			(pixel_format == PixelFormat::RGBAFloat) ||
			(pixel_format == PixelFormat::GrayFloat);
	}

	bool IsLuminanceFormat(PixelFormat pixel_format)
	{
		return
			(pixel_format == PixelFormat::Gray) ||
			(pixel_format == PixelFormat::GrayFloat);
	}

	bool IsDepthStencilFormat(PixelFormat pixel_format)
	{
		return (pixel_format == PixelFormat::DepthStencil);
	}

	int GetPixelSize(PixelFormat pixel_format)
	{
		if (pixel_format == PixelFormat::BGR)
			return sizeof(PixelBGR);
		if (pixel_format == PixelFormat::BGRA)
			return sizeof(PixelBGRA);
		if (pixel_format == PixelFormat::Gray)
			return sizeof(PixelGray);
		if (pixel_format == PixelFormat::RGBFloat)
			return sizeof(PixelRGBFloat);
		if (pixel_format == PixelFormat::RGBAFloat)
			return sizeof(PixelRGBAFloat);
		if (pixel_format == PixelFormat::GrayFloat)
			return sizeof(PixelGrayFloat);
		if (pixel_format == PixelFormat::DepthStencil)
			return sizeof(PixelDepthStencil);
		return 0;
	}

	PixelDescription GetPixelDescription(PixelFormat pixel_format)
	{
		if (pixel_format == PixelFormat::BGR)
			return { PixelComponentType::UnsignedChar, 3 };
		if (pixel_format == PixelFormat::BGRA)
			return { PixelComponentType::UnsignedChar, 4 };
		if (pixel_format == PixelFormat::Gray)
			return { PixelComponentType::UnsignedChar, 1 };
		if (pixel_format == PixelFormat::RGBFloat)
			return { PixelComponentType::Float, 3 };
		if (pixel_format == PixelFormat::RGBAFloat)
			return { PixelComponentType::Float, 4 };
		if (pixel_format == PixelFormat::GrayFloat)
			return { PixelComponentType::Float, 1 };			
		if (pixel_format == PixelFormat::DepthStencil)
			return { PixelComponentType::DepthStencil, 1 };
		return {};
	}

	// ==============================================================================================
	// PixelDescription methods
	// ==============================================================================================

	bool DoLoadFromJSON(JSONReadConfiguration config, PixelDescription& dst)
	{
		JSONTools::GetAttribute(config, "component_type", dst.component_type);
		JSONTools::GetAttribute(config, "component_count", dst.component_count);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, PixelDescription const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "component_type", src.component_type);
		JSONTools::SetAttribute(json, "component_count", src.component_count);
		return true;
	}

	PixelFormat GetPixelFormat(PixelDescription const & pixel_description)
	{
		if (pixel_description.component_count == 1)
		{
			if (pixel_description.component_type == PixelComponentType::UnsignedChar)
				return PixelFormat::Gray;
			if (pixel_description.component_type == PixelComponentType::Float)
				return PixelFormat::GrayFloat;
		}
		else if (pixel_description.component_count == 3)
		{
			if (pixel_description.component_type == PixelComponentType::UnsignedChar)
				return PixelFormat::BGR;
			if (pixel_description.component_type == PixelComponentType::Float)
				return PixelFormat::RGBFloat;
		}
		else if (pixel_description.component_count == 4)
		{
			if (pixel_description.component_type == PixelComponentType::UnsignedChar)
				return PixelFormat::BGRA;
			if (pixel_description.component_type == PixelComponentType::Float)
				return PixelFormat::RGBAFloat;
		}
		return PixelFormat::Unknown;
	}


#if 0

	// ==============================================================================================
	// PixelFormat methods
	// ==============================================================================================

	PixelFormat const PixelFormat::Gray			= PixelFormat(PixelComponentType::UnsignedChar, 1);
	PixelFormat const PixelFormat::BGR			= PixelFormat(PixelComponentType::UnsignedChar, 3);
	PixelFormat const PixelFormat::BGRA			= PixelFormat(PixelComponentType::UnsignedChar, 4);
	PixelFormat const PixelFormat::GrayFloat	= PixelFormat(PixelComponentType::Float, 1);
	PixelFormat const PixelFormat::RGBFloat		= PixelFormat(PixelComponentType::Float, 3);
	PixelFormat const PixelFormat::RGBAFloat	= PixelFormat(PixelComponentType::Float, 4);
	PixelFormat const PixelFormat::DepthStencil	= PixelFormat(PixelComponentType::DepthStencil, 1);

	int PixelFormat::GetPixelSize() const
	{
		if (IsValid())
		{
			if (component_type == PixelComponentType::UnsignedChar)
				return component_count * sizeof(unsigned char);
			if (component_type == PixelComponentType::Float)
				return component_count * sizeof(float);
			if (component_type == PixelComponentType::DepthStencil)
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
		if (component_type != PixelComponentType::UnsignedChar && component_type != PixelComponentType::Float)
			return false;
		if (component_count != 1 && component_count != 3 && component_count != 4)
			return false;
		return true;
	}

	bool PixelFormat::IsDepthStencilPixel() const
	{
		if (component_type == PixelComponentType::DepthStencil)
			return true;
		return false;
	}

	bool PixelFormat::operator == (PixelFormat other) const
	{
		return (component_type == other.component_type) && (component_count == other.component_count);
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, PixelFormat& dst)
	{
		JSONTools::GetAttribute(config, "component_type", dst.component_type);
		JSONTools::GetAttribute(config, "component_count", dst.component_count);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json * json, PixelFormat src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "component_type", src.component_type);
		JSONTools::SetAttribute(json, "component_count", src.component_count);
		return true;
	}

#endif

}; // namespace chaos
