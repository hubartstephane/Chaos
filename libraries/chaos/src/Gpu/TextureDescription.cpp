#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	glm::ivec2 SurfaceDescription::GetSize(int mipmap) const
	{
		return glm::ivec2(width >> mipmap, height >> mipmap);
	}

	PixelFormat SurfaceDescription::GetPixelFormat() const
	{
		return pixel_format;
	}

	bool operator == (SurfaceDescription const & src1, SurfaceDescription const & src2)
	{
		return
			(src1.width == src2.width) &&
			(src1.height == src2.height) &&
			(src1.depth == src2.depth) &&
			(src1.pixel_format == src2.pixel_format);
	}

	bool TextureDescription::IsValid() const
	{
		// no negative size
		if (width <= 0 || height <= 0 || depth <= 0)
			return false;

		// some checks
		switch (type)
		{
		case TextureType::Texture1D:
			if (depth > 1) // use depth only for arrays
				return false;
		case TextureType::Texture1DArray:
			if (height > 1)
				return false;
			break;

		case TextureType::Texture2D:
			if (depth > 1) // use depth only for arrays
				return false;
			break;

		case TextureType::TextureCubeMap:
			if (depth > 1) // use depth only for arrays
				return false;
		case TextureType::TextureCubeMapArray:
			if (width != height)
				return false;
			break;
		}
		return true;
	}

	bool operator == (TextureDescription const & src1, TextureDescription const & src2)
	{
		SurfaceDescription const & surf1 = src1;
		SurfaceDescription const & surf2 = src2;

		return
			(surf1 == surf2) &&
			(src1.type == src2.type) &&
			(src1.use_mipmaps == src2.use_mipmaps);
	}

}; // namespace chaos
