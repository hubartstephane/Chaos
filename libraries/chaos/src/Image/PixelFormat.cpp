#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ==============================================================================================
	// PixelFormat methods
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

	bool PixelFormat::operator == (PixelFormat const & other) const
	{
		return (component_type == other.component_type) && (component_count == other.component_count);
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, PixelFormat& dst)
	{
		JSONTools::GetAttribute(config, "component_type", dst.component_type);
		JSONTools::GetAttribute(config, "component_count", dst.component_count);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json * json, PixelFormat const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "component_type", src.component_type);
		JSONTools::SetAttribute(json, "component_count", src.component_count);
		return true;
	}

}; // namespace chaos
