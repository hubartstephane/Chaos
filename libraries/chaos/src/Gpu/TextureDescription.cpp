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

}; // namespace chaos
