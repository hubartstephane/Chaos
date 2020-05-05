#include <chaos/BitmapAtlasFilter.h>
#include <chaos/ImageTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/JSONTools.h>
#include <chaos/ImagePixelAccessor.h>

namespace chaos
{



	namespace BitmapAtlas
	{
		FIBITMAP* BitmapAtlasFilter::ProcessImage(ImageDescription const& desc)
		{
			return ImageTools::GenFreeImage<chaos::PixelBGRA>(10 + desc.width, 10 + desc.height, [desc](ImageDescription const & new_desc) 
			{
				ImagePixelAccessor<chaos::PixelBGRA> accessor1(new_desc);
				ImagePixelAccessor<chaos::PixelBGR> accessor2(new_desc);

				bool b1 = accessor1.IsValid();
				bool b2 = accessor2.IsValid();

				for (int j = 0; j < new_desc.height; ++j)
				{
					for (int i = 0; i < new_desc.width; ++i)
					{
						accessor1(i, j).R = (unsigned char)i;
						accessor1(i, j).A = 255;


					}
				}
			
			});
			return nullptr;
		}


	}; // namespace BitmapAtlas

}; // namespace chaos

