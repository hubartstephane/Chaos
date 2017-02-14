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
	}

	int PixelFormat::GetPixelSize() const
	{
		if (!IsValid())
		{
			if (component_type == TYPE_UNSIGNED_CHAR)
				return component_count * sizeof(unsigned char);
			if (component_type == TYPE_FLOAT)
				return component_count * sizeof(float);
		}
		return 0;	
	}

	bool PixelFormat::IsValid() const
	{
		if (component_type != TYPE_UNSIGNED_CHAR && component_type != TYPE_FLOAT)
			return false;
		if (component_count != 1 && component_count != 3 && component_count != 4)
			return false;
		return true;
	}

	bool PixelFormat::operator == (PixelFormat const & other) const
	{
		return (component_type == other.component_type) && (component_count == other.component_count);	
	}

	bool PixelFormat::operator != (PixelFormat const & other) const
	{
		return !operator == (other);
	}

	int PixelFormat::GetBPP() const
	{
		if (!IsValid())
			return 0;
		if (component_type == TYPE_FLOAT)
			return 0;
		return component_count * 8;
	}

	PixelFormat PixelFormat::FromImage(FIBITMAP * image)
	{
		assert(image != nullptr);

		PixelFormat result;
	
		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(image);
		if (image_type == FIT_BITMAP)
		{			
			int bpp = FreeImage_GetBPP(image); // ignore other format than 8, 24 and 32 bpp
			if (bpp != 8 && bpp != 24 && bpp != 32)
				return result;			

			result.component_type  = TYPE_UNSIGNED_CHAR;
			result.component_count = bpp / 8;
		}
		else if (image_type == FIT_FLOAT || image_type == FIT_RGBF || image_type == FIT_RGBAF) // floating points format are accepted
		{
			result.component_type  = TYPE_FLOAT;

			if (image_type == FIT_FLOAT)
				result.component_count = 1;
			else if (image_type == FIT_RGBF)
				result.component_count = 3;
			else if (image_type == FIT_RGBAF)
				result.component_count = 4;
		}
		return result;
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
	}

	void PixelFormatMerger::Reset(PixelFormatMergeParams const & in_params)
	{
		operator = (PixelFormatMerger(in_params)); // copy from a newly created instance
	}

	void PixelFormatMerger::Merge(PixelFormat src)
	{
		assert(src.IsValid());

		if (!params.accept_luminance && src.component_count == 1) // transform luminance into RGB
			src.component_count = 3;

		if (!params.accept_float && src.component_type == PixelFormat::TYPE_FLOAT) // transform float into unsigned char
			src.component_count = PixelFormat::TYPE_UNSIGNED_CHAR;	

		if (!result_is_available)
		{
			result = src;
			result_is_available = true;
		}
		else
		{
			result.component_type = max(result.component_type, src.component_type);
			result.component_count = max(result.component_count, src.component_count);		
		}
	}

	bool PixelFormatMerger::GetResult(PixelFormat & value) const
	{
		if (result_is_available)
			value = result;
		return result_is_available;	
	}

}; // namespace chaos
