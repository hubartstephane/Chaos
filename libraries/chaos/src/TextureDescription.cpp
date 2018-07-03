#include <chaos/TextureDescription.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
	glm::ivec2 TextureDescription::GetSize(int mipmap) const
	{
		return glm::ivec2(width >> mipmap, height >> mipmap);
	}

	PixelFormat TextureDescription::GetPixelFormat() const
	{
		return GLTextureTools::GetPixelFormat(internal_format);
	}

}; // namespace chaos
