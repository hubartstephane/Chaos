#include <chaos/BitmapAtlasFilter.h>
#include <chaos/ImageTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/JSONTools.h>
#include <chaos/ImagePixelAccessor.h>

namespace chaos
{



	namespace BitmapAtlas
	{
		FIBITMAP* BitmapAtlasFilter::ProcessImage(ImageDescription const& src_desc)
		{
			return ImageTools::GenFreeImage<chaos::PixelBGRA>(src_desc.width, src_desc.height, [src_desc](ImageDescription const & dst_desc)
			{
				ImagePixelAccessor<chaos::PixelBGRA> src_acc(src_desc);
				ImagePixelAccessor<chaos::PixelBGRA> dst_acc(dst_desc);

				if (src_acc.IsValid() && dst_acc.IsValid())
				{
					for (int j = 0; j < dst_desc.height; ++j)
					{
						for (int i = 0; i < dst_desc.width; ++i)
						{
#if 1


							dst_acc(i, j).R = (unsigned char)i;
							dst_acc(i, j).B = 0;
							dst_acc(i, j).G = 0;
							dst_acc(i, j).A = 255;

#else

							dst_acc(i, j).R = src_acc(i, j).B;
							dst_acc(i, j).B = 0;
							dst_acc(i, j).G = src_acc(i, j).G;
							dst_acc(i, j).A = src_acc(i, j).A;
#endif
						}
					}
				}
			});
			return nullptr;
		}


	}; // namespace BitmapAtlas

}; // namespace chaos

