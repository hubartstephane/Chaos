#include <chaos/PixelFormat.h>

namespace chaos
{
	// ==============================================================================================
	// PixelFormat methods
	// ==============================================================================================

	PixelFormat::PixelFormat(int in_format)
	{
		if (in_format == FORMAT_GRAY)
		{
			component_type = TYPE_UNSIGNED_CHAR;
			component_count = 1;
		}
		else if (in_format == FORMAT_RGB)
		{
			component_type = TYPE_UNSIGNED_CHAR;
			component_count = 3;
		}
		else if (in_format == FORMAT_RGBA)
		{
			component_type = TYPE_UNSIGNED_CHAR;
			component_count = 4;
		}
		else if (in_format == FORMAT_GRAY_FLOAT)
		{
			component_type = TYPE_FLOAT;
			component_count = 1;
		}
		else if (in_format == FORMAT_RGB_FLOAT)
		{
			component_type = TYPE_FLOAT;
			component_count = 3;
		}
		else if (in_format == FORMAT_RGBA_FLOAT)
		{
			component_type = TYPE_FLOAT;
			component_count = 4;
		}
		else if (in_format == FORMAT_DEPTH_STENCIL)
		{
			component_type = TYPE_DEPTH_STENCIL;
			component_count = 1;
		}
	}

	int PixelFormat::GetFormat() const
	{
		if (component_type == TYPE_UNSIGNED_CHAR)
		{
			if (component_count == 1)
				return FORMAT_GRAY;
			if (component_count == 3)
				return FORMAT_RGB;
			if (component_count == 4)
				return FORMAT_RGBA;
		}
		else if (component_type == TYPE_FLOAT)
		{
			if (component_count == 1)
				return FORMAT_GRAY_FLOAT;
			if (component_count == 3)
				return FORMAT_RGB_FLOAT;
			if (component_count == 4)
				return FORMAT_RGBA_FLOAT;
		}
		else if (component_type == TYPE_DEPTH_STENCIL)
		{
			return FORMAT_DEPTH_STENCIL;
		}
		return FORMAT_UNKNOWN;
	}

	int PixelFormat::GetPixelSize() const
	{
		if (IsValid())
		{
			if (component_type == TYPE_UNSIGNED_CHAR)
				return component_count * sizeof(unsigned char);
			if (component_type == TYPE_FLOAT)
				return component_count * sizeof(float);
			if (component_type == TYPE_DEPTH_STENCIL)
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
		if (component_type != TYPE_UNSIGNED_CHAR && component_type != TYPE_FLOAT)
			return false;
		if (component_count != 1 && component_count != 3 && component_count != 4)
			return false;
		return true;
	}

	bool PixelFormat::IsDepthStencilPixel() const
	{
		if (component_type == TYPE_DEPTH_STENCIL)
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

		if (!params.accept_luminance && src.component_count == 1) // transform luminance into RGB
			src.component_count = 3;

		if (!params.accept_float && src.component_type == PixelFormat::TYPE_FLOAT) // transform float into unsigned char
			src.component_count = PixelFormat::TYPE_UNSIGNED_CHAR;	

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
	}

	PixelFormat PixelFormatMerger::GetResult() const
	{
		return result;
	}

}; // namespace chaos
