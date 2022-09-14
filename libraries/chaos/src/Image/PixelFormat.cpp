#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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

	CHAOS_IMPLEMENT_ENUM_METHOD(PixelComponentType, pixel_component_type_encoding);

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

	bool LoadFromJSON(nlohmann::json const& json, PixelFormat& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "component_type", dst.component_type);
		JSONTools::GetAttribute(json, "component_count", dst.component_count);
		return true;
	}

	bool SaveIntoJSON(nlohmann::json& json, PixelFormat const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "component_type", src.component_type);
		JSONTools::SetAttribute(json, "component_count", src.component_count);
		return true;
	}

}; // namespace chaos
