#include <chaos/Texture.h>


namespace chaos
{
	glm::ivec2 TextureDescription::GetSize(int mipmap) const
	{
		return glm::ivec2(width >> mipmap, height >> mipmap);
	}

	Texture::Texture(GLuint in_id, TextureDescription const & in_texture_description) :
		texture_id(in_id),
		texture_description(in_texture_description)
	{
	}

	Texture::~Texture()
	{
		DoRelease();
	}

	bool Texture::DoRelease()
	{
		if (texture_id == 0)
			return false;
		texture_description = TextureDescription();
		glDeleteTextures(1, &texture_id);
		texture_id = 0;
		return true;
	}

}; // namespace chaos
