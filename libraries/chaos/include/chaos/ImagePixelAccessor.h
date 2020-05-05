#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ImageDescription.h>
#include <chaos/PixelTypes.h>
#include <chaos/PixelFormat.h>

namespace chaos
{
	template<typename PIXEL_TYPE>
	class ImagePixelAccessor
	{
	public:

		/** constructor */
		ImagePixelAccessor(ImageDescription const& in_description) : description(in_description) {}
		/** copy constructor */
		ImagePixelAccessor(ImagePixelAccessor const& src) = default;

		/** check whether the object is valid (the required PIXEL_TYPE does not correspond to the one for the bitmap) */
		bool IsValid() const
		{
			PixelFormat accessor_format = PixelFormat::GetPixelFormat<PIXEL_TYPE>();
			if (accessor_format != description.pixel_format)
				return false;
			return true;
		}

		/** get the description */
		ImageDescription const& GetImageDescription() const { return description; }

		/** access a pixel */
		PIXEL_TYPE& operator () (int x, int y)
		{
			assert(x >= 0);
			assert(y >= 0);
			assert(x < description.width);
			assert(y < description.height);

			char* base = ((char*)description.data) + y * description.pitch_size;

			return *(((PIXEL_TYPE*)base) + x);
		}

		/** access a pixel */
		PIXEL_TYPE const& operator () (int x, int y) const
		{
			assert(x >= 0);
			assert(y >= 0);
			assert(x < description.width);
			assert(y < description.height);

			char const* base = ((char const*)description.data) + y * description.pitch_size;

			return *(((PIXEL_TYPE const*)base) + x);
		}

	protected:

		/** the description we are working on */
		ImageDescription description;

	};

}; // namespace chaos

