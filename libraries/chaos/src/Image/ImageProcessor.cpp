#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static std::vector<FIBITMAP*> ProcessAnimatedImage_ClearResult(std::vector<FIBITMAP*>& images)
	{
		for (FIBITMAP* img : images)
			FreeImage_Unload(img);
		images.clear();
		return std::vector<FIBITMAP*>(); // in case of error, destroy all allocated resources
	}

	std::vector<FIBITMAP*> ImageProcessor::ProcessImageFrames(std::vector<FIBITMAP*> const& src, BitmapGridAnimationInfo const & grid_anim) const
	{
		assert(src.size() == 1 || grid_anim.GetFrameCount() == 0); // cannot have both multiple image and a grid structure

		std::vector<FIBITMAP*> result;

		// grid animation
		if (grid_anim.GetFrameCount() > 1)
		{
			ImageDescription src_desc = ImageTools::GetImageDescription(src[0]);

			// create an image for all cell in the grid. Store them temporary in result
			for (int y = 0; y < grid_anim.grid_size.y; ++y)
			{
				for (int x = 0; x < grid_anim.grid_size.x; ++x)
				{
					int w = src_desc.width / grid_anim.grid_size.x;
					int h = src_desc.height / grid_anim.grid_size.y;
					ImageDescription sub_desc = src_desc.GetSubImageDescription(x * w, y * h, w, h);

					FIBITMAP* image = ProcessImage(sub_desc);
					if (image == nullptr)
					{
						ImageProcessorLog::Error("ImageProcessor : ProcessImage() failed for GRID");
						return ProcessAnimatedImage_ClearResult(result);
					}
					// push first so that the image is released in case of failure
					result.push_back(image);
					// check compatibility with previous elements
					if (result.size() > 1)
					{
						ImageDescription d1 = ImageTools::GetImageDescription(image);
						ImageDescription d2 = ImageTools::GetImageDescription(result[0]);
						if (d1.width != d2.width || d1.height != d2.height || d1.pixel_format != d2.pixel_format)
						{
							ImageProcessorLog::Error("ImageProcessor : when processing GRID resulting images should all have same size and same format");
							return ProcessAnimatedImage_ClearResult(result);
						}
					}
				}
			}

			assert(result.size() > 0);

			// recompose the image to have a grid
			ImageDescription d = ImageTools::GetImageDescription(result[0]);
			FIBITMAP * image = ImageTools::GenFreeImage(d.pixel_format, d.width * grid_anim.grid_size.x, d.height * grid_anim.grid_size.y);
			if (image == nullptr)
			{
				ImageProcessorLog::Error("ImageProcessor : failed allocating image");
				return ProcessAnimatedImage_ClearResult(result);
			}

			ImageDescription dst_desc = ImageTools::GetImageDescription(image);

			for (int y = 0; y < grid_anim.grid_size.y; ++y)
			{
				for (int x = 0; x < grid_anim.grid_size.x; ++x)
				{
					int w = dst_desc.width / grid_anim.grid_size.x;
					int h = dst_desc.height / grid_anim.grid_size.y;
					ImageDescription src = ImageTools::GetImageDescription(result[(size_t)(x + y * grid_anim.grid_size.x)]);

					ImageTools::CopyPixels(src, dst_desc, 0, 0, x * w, y * h, w, h);
				}
			}

			// replace the result
			ProcessAnimatedImage_ClearResult(result);
			result.push_back(image);
		}
		// frame animation or no animation
		else if (src.size() > 0)
		{
			for (size_t i = 0; i < src.size(); ++i)
			{
				FIBITMAP * image = ProcessImage(ImageTools::GetImageDescription(src[i]));
				if (image == nullptr)
				{
					ImageProcessorLog::Error("ImageProcessor : ProcessImage() failed for singe/multiple images");
					return ProcessAnimatedImage_ClearResult(result);
				}
				result.push_back(image);
			}
		}
		return result;
	}

	FIBITMAP* ImageProcessor::ProcessImage(ImageDescription const & src_desc) const
	{
		return nullptr;
	}

}; // namespace chaos

