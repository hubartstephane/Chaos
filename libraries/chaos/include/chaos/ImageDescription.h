#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelFormat.h>

namespace chaos
{

	/** 
	* ImageFaceDescription : data to work with pixels
	*/

	class ImageDescription
	{
	public:

		/** constructor */
		ImageDescription() = default;

		ImageDescription(void * in_data, int in_width, int in_height, PixelFormat const & in_pixel_format, int in_padding = 0);

		/** returns true whether the description is valid */
		bool IsValid() const;
		/** returns true whether the image is empty */
		bool IsEmpty() const;
		/** get the image information for a sub image */
		ImageDescription GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const;

	public:

		/** the buffer */
		void * data = nullptr;
		/** the image width */
		int    width = 0;
		/** the image height */
		int    height = 0;	
		/** the pixel format */
		PixelFormat pixel_format;
		/** size of line in bytes (exclude padding) : width * pixel_size */
		int    line_size = 0;
		/** size of line in bytes (including padding) : padding + line_size */
		int    pitch_size = 0;
		/** padding a the end of a line in bytes */
		int    padding_size = 0;
	};

}; // namespace chaos

