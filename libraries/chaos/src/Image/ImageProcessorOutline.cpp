#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	FIBITMAP* ImageProcessorOutline::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format == PixelFormat::DepthStencil)
		{
			ImageProcessorLog::Error("ImageProcessorOutline : cannot process DepthStencil format");
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

}; // namespace chaos

