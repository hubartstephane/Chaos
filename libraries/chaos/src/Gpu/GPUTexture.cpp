#include <chaos/Chaos.h>

namespace chaos
{
	GPUTexture::GPUTexture(GLuint in_id, TextureDescription const & in_texture_description) :
		texture_id(in_id),
		texture_description(in_texture_description)
	{
	}

	GPUTexture::~GPUTexture()
	{
		Release();
	}

	void GPUTexture::Release()
	{
		if (texture_id != 0)
		{
			texture_description = TextureDescription();
			glDeleteTextures(1, &texture_id);
			texture_id = 0;
		}
	}

}; // namespace chaos
