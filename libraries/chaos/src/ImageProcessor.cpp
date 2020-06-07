#include <chaos/ImageProcessor.h>
#include <chaos/ImageTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/JSONTools.h>
#include <chaos/ImagePixelAccessor.h>
#include <chaos/LogTools.h>

namespace chaos
{

	// ================================================================
	// ImageProcessor functions
	// ================================================================

	static std::vector<FIBITMAP*> ProcessAnimatedImage_ClearResult(std::vector<FIBITMAP*>& images)
	{
		for (FIBITMAP* img : images)
			FreeImage_Unload(img);
		images.clear();
		return std::vector<FIBITMAP*>(); // in case of error, destroy all allocated resources
	}

	std::vector<FIBITMAP*> ImageProcessor::ProcessAnimatedImage(std::vector<FIBITMAP*> const& src, BitmapGridAnimationInfo const & grid_anim) const
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
						LogTools::Error("ImageProcessor : ProcessImage() failed for GRID");
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
							LogTools::Error("ImageProcessor : when processing GRID resulting images should all have same size and same format");
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
				LogTools::Error("ImageProcessor : failed allocating image");
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
					LogTools::Error("ImageProcessor : ProcessImage() failed for singe/multiple images");
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

	bool ImageProcessor::SaveIntoJSON(nlohmann::json& json_entry) const
	{

		return true;
	}

	bool ImageProcessor::LoadFromJSON(nlohmann::json const& json_entry)
	{

		return true;
	}

	// ================================================================
	// ImageProcessorOutline functions
	// ================================================================

	FIBITMAP* ImageProcessorOutline::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format != PixelFormat::BGRA && src_desc.pixel_format != PixelFormat::BGR)
		{
			LogTools::Error("ImageProcessorOutline : can only process PixelBGR & PixelBGRA formats");
			return nullptr;
		}

		return DoImageProcessing(src_desc, [this, src_desc](auto src_accessor) -> FIBITMAP *
		{
			if (!src_accessor.IsValid())
				return nullptr;

			using accessor_type = decltype(src_accessor);

			int d = (int)distance;

			int dest_width = src_desc.width + 2 * d;
			int dest_height = src_desc.height + 2 * d;

			// generate the image
			FIBITMAP * result = ImageTools::GenFreeImage(src_desc.pixel_format, dest_width, dest_height);
			if (result != nullptr)
			{
				accessor_type dst_accessor(ImageTools::GetImageDescription(result));
				if (!dst_accessor.IsValid())
				{
					FreeImage_Unload(result);
					return nullptr;
				}

				// all pixels on destination images
				for (int y = 0; y < dest_height; ++y)
				{
					for (int x = 0; x < dest_width; ++x)
					{
						int src_x = x - d;
						int src_y = y - d;
						if (src_x >= 0 && src_x < src_desc.width && src_y >= 0 && src_y < src_desc.height)
							dst_accessor(x, y) = src_accessor(src_x, src_y);
						else
						{
							if constexpr (std::is_same_v<typename accessor_type::type, PixelBGR>)
							{
								dst_accessor(x, y).R = 0;
								dst_accessor(x, y).G = 255;
								dst_accessor(x, y).B = 0;

							}
							else if constexpr (std::is_same_v<typename accessor_type::type, PixelBGRA>)
							{
								dst_accessor(x, y).R = 0;
								dst_accessor(x, y).G = 255;
								dst_accessor(x, y).B = 0;
								dst_accessor(x, y).A = 255;
							}
						}
					}
				}
			}
			return result;
		});
	}

	bool ImageProcessorOutline::SaveIntoJSON(nlohmann::json& json_entry) const
	{
		if (!ImageProcessor::SaveIntoJSON(json_entry))
			return false;
		JSONTools::SetAttribute(json_entry, "distance", distance);
		JSONTools::SetAttribute(json_entry, "outline_color", outline_color);
		return true;
	}

	bool ImageProcessorOutline::LoadFromJSON(nlohmann::json const& json_entry)
	{
		if (!ImageProcessor::LoadFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "distance", distance);
		JSONTools::GetAttribute(json_entry, "outline_color", outline_color);
		return true;
	}

}; // namespace chaos
