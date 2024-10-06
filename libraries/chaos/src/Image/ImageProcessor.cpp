#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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
						Log::Error("ImageProcessor : ProcessImage() failed for GRID");
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
							Log::Error("ImageProcessor : when processing GRID resulting images should all have same size and same format");
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
				Log::Error("ImageProcessor : failed allocating image");
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
					Log::Error("ImageProcessor : ProcessImage() failed for singe/multiple images");
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

	// ================================================================
	// ImageProcessorOutline functions
	// ================================================================

	FIBITMAP* ImageProcessorOutline::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format == PixelFormat::DepthStencil)
		{
			Log::Error("ImageProcessorOutline : cannot process DepthStencil format");
			return nullptr;
		}

		return DoImageProcessing(src_desc, [this, src_desc](auto src_accessor) -> FIBITMAP *
		{
			if (!src_accessor.IsValid())
				return nullptr;

			using accessor_type = decltype(src_accessor);

			int dest_width = src_desc.width + 2 * distance;
			int dest_height = src_desc.height + 2 * distance;

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

				using pixel_type = typename accessor_type::type;

				PixelRGBAFloat o = color;
				PixelRGBAFloat e = empty_color;

				pixel_type outline, empty;
				PixelConverter::Convert(outline, o);
				PixelConverter::Convert(empty, e);

				int d2 = distance * distance;

				// all pixels on destination images
				for (int y = 0; y < dest_height; ++y)
				{
					for (int x = 0; x < dest_width; ++x)
					{
						int src_x = x - distance;
						int src_y = y - distance;

						// search whether we must add an outline
						int min_src_x = std::max(0, src_x - distance);
						int max_src_x = std::min(src_x + distance, src_desc.width - 1);

						int min_src_y = std::max(0, src_y - distance);
						int max_src_y = std::min(src_y + distance, src_desc.height - 1);

						bool all_neighboor_empty = true;
						for (int sy = min_src_y; (sy <= max_src_y) && all_neighboor_empty; ++sy)
						{
							for (int sx = min_src_x; (sx <= max_src_x) && all_neighboor_empty; ++sx)
							{
								int dx = src_x - sx;
								int dy = src_y - sy;
								if (dx * dx + dy * dy <= d2)
									all_neighboor_empty = !color_filter.Filter(src_accessor(sx, sy));
							}
						}

						// put the pixel on destination
						if (all_neighboor_empty)
							dst_accessor(x, y) = empty;
						else if (src_x >= 0 && src_x < src_desc.width && src_y >= 0 && src_y < src_desc.height && color_filter.Filter(src_accessor(src_x, src_y)))
							dst_accessor(x, y) = src_accessor(src_x, src_y);
						else
							dst_accessor(x, y) = outline;
					}
				}
			}
			return result;
		});
	}

	bool ImageProcessorOutline::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!ImageProcessor::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "distance", distance);
		JSONTools::SetAttribute(json, "color_filter", color_filter);
		JSONTools::SetAttribute(json, "color", color);
		JSONTools::SetAttribute(json, "empty_color", empty_color);
		return true;
	}

	bool ImageProcessorOutline::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!ImageProcessor::SerializeFromJSON(config))
			return false;
		JSONTools::GetAttribute(config, "distance", distance);
		JSONTools::GetAttribute(config, "color_filter", color_filter);
		JSONTools::GetAttribute(config, "color", color);
		JSONTools::GetAttribute(config, "empty_color", empty_color);
		return true;
	}

	// ================================================================
	// ImageProcessorAddAlpha functions
	// ================================================================

	FIBITMAP* ImageProcessorAddAlpha::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format == PixelFormat::DepthStencil)
		{
			Log::Error("ImageProcessorAddAlpha : cannot process DepthStencil format");
			return nullptr;
		}

		return DoImageProcessing(src_desc, [this, src_desc](auto src_accessor) -> FIBITMAP*
		{
			if (!src_accessor.IsValid())
				return nullptr;

			using accessor_type = decltype(src_accessor);

			int dest_width = src_desc.width;
			int dest_height = src_desc.height;

			// generate the image
			FIBITMAP* result = ImageTools::GenFreeImage(PixelFormat::BGRA, dest_width, dest_height);
			if (result != nullptr)
			{
				ImagePixelAccessor<PixelBGRA> dst_accessor(ImageTools::GetImageDescription(result));
				if (!dst_accessor.IsValid())
				{
					FreeImage_Unload(result);
					return nullptr;
				}

				using pixel_type = typename accessor_type::type;

				for (int j = 0; j < src_desc.height; ++j)
				{
					for (int i = 0; i < src_desc.width; ++i)
					{
						pixel_type p = src_accessor(i, j);

						// compute the distance to black
						PixelRGBAFloat c;
						PixelConverter::Convert(c, p);
						float l = std::min(strength * (c.R * c.R + c.G * c.G + c.B * c.B), 1.0f);

						// the distance to black is considered as alpha
						PixelBGRA p2;
						PixelConverter::Convert(p2, p);
						p2.A = char(255.0f * l);

						dst_accessor(i, j) = p2;
					}
				}
			}
			return result;
		});
	}

	bool ImageProcessorAddAlpha::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!ImageProcessor::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "strength", strength);
		return true;
	}

	bool ImageProcessorAddAlpha::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!ImageProcessor::SerializeFromJSON(config))
			return false;
		JSONTools::GetAttribute(config, "strength", strength);
		return true;
	}

	// ================================================================
	// ImageProcessorShadow functions
	// ================================================================

	FIBITMAP* ImageProcessorShadow::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format == PixelFormat::DepthStencil)
		{
			Log::Error("ImageProcessorShadow : cannot process DepthStencil format");
			return nullptr;
		}

		return nullptr;
#if 0

		return DoImageProcessing(src_desc, [this, src_desc](auto src_accessor) -> FIBITMAP*
		{
			if (!src_accessor.IsValid())
				return nullptr;

			using accessor_type = decltype(src_accessor);

			int dest_width = src_desc.width + 2 * distance;
			int dest_height = src_desc.height + 2 * distance;

			// generate the image
			FIBITMAP* result = ImageTools::GenFreeImage(src_desc.pixel_format, dest_width, dest_height);
			if (result != nullptr)
			{
				accessor_type dst_accessor(ImageTools::GetImageDescription(result));
				if (!dst_accessor.IsValid())
				{
					FreeImage_Unload(result);
					return nullptr;
				}

				using pixel_type = typename accessor_type::type;

				PixelRGBAFloat o = color;
				PixelRGBAFloat e = empty_color;

				pixel_type outline, empty;
				PixelConverter::Convert(outline, o);
				PixelConverter::Convert(empty, e);

				int d2 = distance * distance;

				// all pixels on destination images
				for (int y = 0; y < dest_height; ++y)
				{
					for (int x = 0; x < dest_width; ++x)
					{
						int src_x = x - distance;
						int src_y = y - distance;

						// search whether we must add an outline
						int min_src_x = std::max(0, src_x - distance);
						int max_src_x = std::min(src_x + distance, src_desc.width - 1);

						int min_src_y = std::max(0, src_y - distance);
						int max_src_y = std::min(src_y + distance, src_desc.height - 1);

						bool all_neighboor_empty = true;
						for (int sy = min_src_y; (sy <= max_src_y) && all_neighboor_empty; ++sy)
						{
							for (int sx = min_src_x; (sx <= max_src_x) && all_neighboor_empty; ++sx)
							{
								int dx = src_x - sx;
								int dy = src_y - sy;
								if (dx * dx + dy * dy <= d2)
									all_neighboor_empty = !color_filter.Filter(src_accessor(sx, sy));
							}
						}

						// put the pixel on destination
						if (all_neighboor_empty)
							dst_accessor(x, y) = empty;
						else if (src_x >= 0 && src_x < src_desc.width && src_y >= 0 && src_y < src_desc.height && color_filter.Filter(src_accessor(src_x, src_y)))
							dst_accessor(x, y) = src_accessor(src_x, src_y);
						else
							dst_accessor(x, y) = outline;
					}
				}
			}
			return result;
		});
#endif
	}

	bool ImageProcessorShadow::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!ImageProcessor::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "offset", offset);
		JSONTools::SetAttribute(json, "color_filter", color_filter);
		JSONTools::SetAttribute(json, "color", color);
		JSONTools::SetAttribute(json, "empty_color", empty_color);
		return true;
	}

	bool ImageProcessorShadow::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!ImageProcessor::SerializeFromJSON(config))
			return false;
		JSONTools::GetAttribute(config, "offset", offset);
		JSONTools::GetAttribute(config, "color_filter", color_filter);
		JSONTools::GetAttribute(config, "color", color);
		JSONTools::GetAttribute(config, "empty_color", empty_color);
		return true;
	}

}; // namespace chaos

