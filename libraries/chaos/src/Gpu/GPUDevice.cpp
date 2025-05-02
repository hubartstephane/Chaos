#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	bool GPUDevice::Initialize(JSONReadConfiguration config)
	{
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false)) // do not recursively update all child objects
			return false;
		return OnInitialize(config);
	}

	void GPUDevice::Finalize()
	{

	}

	bool GPUDevice::OnInitialize(JSONReadConfiguration config)
	{
		return true;
	}

	GPUTexture* GPUDevice::CreateTexture(TextureDescription const& in_texture_description)
	{
		// check for coherency
		if (in_texture_description.type == TextureType::TextureCubeMap || in_texture_description.type == TextureType::TextureCubeMapArray)
			if (in_texture_description.width != in_texture_description.height)
				return nullptr;

		// tweak for single channel formats
		GLPixelFormat gl_formats = GLTextureTools::GetGLPixelFormat(in_texture_description.pixel_format);
		if (!gl_formats.IsValid())
			return nullptr;

		// create the opengl texture
		GLuint texture_id = 0;
		glCreateTextures((GLenum)in_texture_description.type, 1, &texture_id);
		if (texture_id == 0)
			return nullptr;

		// tweak for single channel formats
		if (gl_formats.internal_format == GL_R8 || gl_formats.internal_format == GL_R32F)
		{
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}

		// reserve storage
		int mipmap_count = 1;
		if (in_texture_description.use_mipmaps)
		{
			switch (in_texture_description.type)
			{
				case TextureType::Texture1D:
				case TextureType::Texture1DArray:
					mipmap_count = GLTextureTools::GetMipmapLevelCount(in_texture_description.width);
					break;
				case TextureType::Texture2D:
				case TextureType::Texture2DArray:
				case TextureType::TextureCubeMap:
				case TextureType::TextureCubeMapArray:
					mipmap_count = GLTextureTools::GetMipmapLevelCount(
						std::max(in_texture_description.width, in_texture_description.height));
					break;
				case TextureType::Texture3D:
					mipmap_count = GLTextureTools::GetMipmapLevelCount(
						std::max(std::max(in_texture_description.width, in_texture_description.height), in_texture_description.depth));
					break;
				default:
					assert(0);
					break;
			}
		}

		switch (in_texture_description.type)
		{
		case TextureType::Texture1D:
			glTextureStorage1D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width);
			break;
		case TextureType::Texture1DArray:
			glTextureStorage2D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.depth);
			break;
		case TextureType::Texture2D:
		case TextureType::TextureCubeMap:
			glTextureStorage2D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.height);
			break;
		case TextureType::Texture2DArray:
		case TextureType::Texture3D:
		case TextureType::TextureCubeMapArray:
			glTextureStorage3D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.height, in_texture_description.depth);
			break;
		default:
			assert(0);
			break;
		}

		return new GPUTexture(this, texture_id, in_texture_description);
	}



}; // namespace chaos
