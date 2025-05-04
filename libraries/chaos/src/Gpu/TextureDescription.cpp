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

}; // namespace chaos
