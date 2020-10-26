#include <chaos/Chaos.h>

namespace chaos
{
	ImageDescription::ImageDescription(void * in_data, int in_width, int in_height, PixelFormat const & in_pixel_format, int in_padding):
		data(in_data),
		width(in_width),
		height(in_height),		
		pixel_format(in_pixel_format),
		padding_size(in_padding)
	{
		line_size  = width * pixel_format.GetPixelSize();
		pitch_size = line_size + padding_size;  

		assert(IsValid(true));
	}

	bool ImageDescription::IsEmpty(bool accept_uninitialized_content) const
	{ 
		if (width == 0 || height == 0)
			return true;
		if (data == nullptr && !accept_uninitialized_content)
			return true;
		return false;
	}

	bool ImageDescription::IsValid(bool accept_uninitialized_content) const
	{
		// size cannot be negative
		if (width < 0 || height < 0)
			return false;
		// should not have a buffer if the image is empty
		if (width == 0 || height == 0)
			if (data != nullptr)
				return false;
		// if image is empty, image size should be 0
		if (data == nullptr)
			if (width != 0 || height != 0)
				if (!accept_uninitialized_content)
					return false;
		// the format should be well known
		if (!pixel_format.IsValid())
			return false;
		// test data consistency (only if there is a buffer)
		if (data != nullptr)
		{
			if (line_size != width * pixel_format.GetPixelSize())
				return false;
			if (pitch_size < 0 || padding_size < 0)
				return false;
			if (pitch_size != padding_size + line_size)
				return false;
		}
		return true;		
	}

	ImageDescription ImageDescription::GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const
	{ 
		ImageDescription result;

		// test whether we are a valid description
		if (!IsValid(false))
			return result;
		// want empty image : return null image 
		if (wanted_width < 0 || wanted_height < 0)
			return result;
		// wrong parameters : return null image
		if (x < 0 || x + wanted_width > width)
			return result;
		if (y < 0 || y + wanted_height > height)
			return result;

		int pixel_size = pixel_format.GetPixelSize();

		result.width        = wanted_width;
		result.height       = wanted_height;
		result.pixel_format = pixel_format;
		result.pitch_size   = pitch_size;
		result.line_size    = wanted_width * pixel_size;
		result.padding_size = result.pitch_size - result.line_size;

		if (wanted_width != 0 && wanted_height != 0 && !IsEmpty(false))
		{
			int offset = 0;    
			offset += (x * pixel_size);
			offset += (y * pitch_size); 		
			result.data = ((char*)data) + offset;
		}	

		return result;
	}
}; // namespace chaos
