#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		/**
		* BitmapAtlasFilter : take an image as an entry and returns a new image as output
		*/

		class BitmapAtlasFilter : public ReferencedObject
		{
		public:

			/** the image processing method to override */
			virtual FIBITMAP* ProcessImage(ImageDescription const& desc);

		};

	}; // namespace BitmapAtlas

}; // namespace chaos
