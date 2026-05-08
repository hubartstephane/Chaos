#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	FIBITMAP* ImageProcessorAddAlpha::ProcessImage(ImageDescription const& src_desc) const
	{
		if (src_desc.pixel_format == PixelFormat::DepthStencil)
		{
			ImageProcessorLog::Error("ImageProcessorAddAlpha : cannot process DepthStencil format");
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

}; // namespace chaos

