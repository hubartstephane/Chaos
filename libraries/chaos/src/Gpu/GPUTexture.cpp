#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUTexture::GPUTexture(GPUDevice* in_device, GLuint in_id, TextureDescription const & in_texture_description):
		GPUDeviceResourceInterface(in_device),
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
			texture_description = {};
			glDeleteTextures(1, &texture_id);
			texture_id = 0;
		}
	}

	void GPUTexture::GenerateMipmaps()
	{
		if (texture_id != 0)
			if (texture_description.use_mipmaps)
				glGenerateTextureMipmap(texture_id);
	}

	void GPUTexture::SetWrapMethods(TextureWrapMethods const & wrap_methods)
	{
		if (texture_id != 0)
		{
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, (GLenum)wrap_methods.wrap_x);
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, (GLenum)wrap_methods.wrap_y);
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, (GLenum)wrap_methods.wrap_z);
		}
	}

	void GPUTexture::SetMinificationFilter(TextureMinificationFilter min_filter)
	{
		if (texture_id != 0)
			glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, (GLenum)min_filter);
	}

	void GPUTexture::SetMagnificationFilter(TextureMagnificationFilter mag_filter)
	{
		if (texture_id != 0)
			glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, (GLenum)mag_filter);
	}

	void GPUTexture::SetSubImage(ImageDescription const& image_description)
	{

	#if 0
		GLPixelFormat gl_pixel_format = GLTextureTools::GetGLPixelFormat(image_description.pixel_format);

		int type = (effective_image.pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR) ? GL_UNSIGNED_BYTE : GL_FLOAT;

		GLPixelFormat slice_pixel_format = GLTextureTools::GetGLPixelFormat(effective_image.pixel_format);

		int mipmap_level = 0;
		int start_x = 0;

		glTextureSubImage1D(texture_id, mipmap_level, start_x, image.width, gl_pixel_format.format, type, texture_buffer);
#endif

	}

}; // namespace chaos
