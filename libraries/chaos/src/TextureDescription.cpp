#include <chaos/TextureDescription.h>

namespace chaos
{
	glm::ivec2 TextureDescription::GetSize(int mipmap) const
	{
		return glm::ivec2(width >> mipmap, height >> mipmap);
	}

#if 0
	GLPixelFormat TextureDescription::GetPixelFormat() const
	{




		return GLPixelFormat();
	}
#endif

}; // namespace chaos
