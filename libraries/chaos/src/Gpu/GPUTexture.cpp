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

	int GPUTexture::GetMipmapCount() const
	{
		if (!texture_description.use_mipmaps)
			return 1;

		if (GLTextureTools::IsArrayTextureType(texture_description.type))
			return GLTextureTools::GetMipmapLevelCount(texture_description.width, texture_description.height);
		else
			return GLTextureTools::GetMipmapLevelCount(texture_description.width, texture_description.height, texture_description.depth);
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

	bool GPUTexture::CheckSubImageCommonParameters(ImageDescription const& image_description, glm::ivec3 const& offset, int mipmap_level) const
	{
		if (texture_id == 0)
			return false;

		if (!image_description.IsValid(false))
			return false;

		if (mipmap_level < 0 || mipmap_level >= GetMipmapCount())
			return false;

		if (image_description.pixel_format.component_type != PixelComponentType::UNSIGNED_CHAR && image_description.pixel_format.component_type != PixelComponentType::FLOAT)
			return false;

		if (offset.x < 0 || offset.y < 0 || offset.z < 0)
			return false;

		if ((offset.x + image_description.width > texture_description.width) ||
			(offset.y + image_description.height > texture_description.height) ||
			(offset.z > texture_description.depth))
			return false;

		return true;
	}

	bool GPUTexture::SetSubImageCubeMap(ImageDescription const& image_description, CubeMapFaceType face, glm::ivec3 const& offset, int mipmap_level)
	{
		if (texture_description.type != TextureType::TextureCubeMap && texture_description.type != TextureType::TextureCubeMapArray)
			return false;

		if (!CheckSubImageCommonParameters(image_description, offset, mipmap_level))
			return false;

		GLint depth = GLint(face) + 6 * offset.z;

		GLPixelFormat gl_pixel_format = GLTextureTools::GetGLPixelFormat(image_description.pixel_format);

		GLenum gl_component_type = (image_description.pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR) ?
			GL_UNSIGNED_BYTE :
			GL_FLOAT;

		char* texture_buffer = PreparePixelTransfert(image_description);

		glTextureSubImage3D(
			texture_id,
			mipmap_level,
			offset.x, offset.y, depth,
			image_description.width, image_description.height, 1,
			gl_pixel_format.format,
			gl_component_type,
			texture_buffer
		);

		return true;
	}

	bool GPUTexture::SetSubImage(ImageDescription const& image_description, glm::ivec3 const& offset, int mipmap_level)
	{
		if (texture_description.type != TextureType::Texture1D &&
			texture_description.type != TextureType::Texture1DArray &&
			texture_description.type != TextureType::Texture2D && 
			texture_description.type != TextureType::Texture2DArray &&
			texture_description.type != TextureType::Texture3D)
			return false;

		if (!CheckSubImageCommonParameters(image_description, offset, mipmap_level))
			return false;

		GLPixelFormat gl_pixel_format = GLTextureTools::GetGLPixelFormat(image_description.pixel_format);

		GLenum gl_component_type = (image_description.pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)?
			GL_UNSIGNED_BYTE:
			GL_FLOAT;

		char* texture_buffer = PreparePixelTransfert(image_description);

		switch (texture_description.type)
		{
		case TextureType::Texture1D:

			glTextureSubImage1D(
				texture_id,
				mipmap_level,
				offset.x,
				image_description.width,
				gl_pixel_format.format,
				gl_component_type,
				texture_buffer
			);
			return true;

		case TextureType::Texture1DArray:

			glTextureSubImage2D(
				texture_id,
				mipmap_level,
				offset.x, offset.z, // the array slice for 1D texture is on offset.z
				image_description.width, 1,
				gl_pixel_format.format,
				gl_component_type,
				texture_buffer
			);
			return true;

		case TextureType::Texture2D:

			glTextureSubImage2D(
				texture_id,
				mipmap_level,
				offset.x, offset.y,
				image_description.width, image_description.height,
				gl_pixel_format.format,
				gl_component_type,
				texture_buffer
			);
			return true;

		case TextureType::Texture2DArray:
		case TextureType::Texture3D:

			glTextureSubImage3D(
				texture_id,
				mipmap_level,
				offset.x, offset.y, offset.z,
				image_description.width, image_description.height, 1,
				gl_pixel_format.format,
				gl_component_type,
				texture_buffer
			);
			return true;

		default:
			assert(0);
			return false;
		}

		return true;
	}

	//
	// XXX : When transfering a texture in OpenGL, we can use
	//
	//   GL_UNPACK_ALIGNMENT
	//   GL_UNPACK_ROW_LENGTH
	//   GL_UNPACK_SKIP_PIXELS
	//
	// to define the memory buffer layout.
	//
	//  buffer
	//   x  SKIP_PIXELS +------------+       |
	//   |              |            |       |
	//   |              |            |       |
	//   |              |    Image   |       |
	//   |              |            |       |
	//   |              |            |       |
	//   |              +------------+       |
	//    <--------------------------><----->
	//               ROW LENGTH     some padding
	//
	// OpenGL has a important limitation. Offsets are not defined in bytes but in multiple of Pixel Size !!
	// That means that we have no way to require for an explicit padding !
	//
	// note : with SubImageDefinition, the padding may be a huge value to skip lots of pixel at the end of the scanline
	//
	// In a random ImageDefinition, this is a blocker. We cannot have all paddings we want
	//
	//
	// If the memory disposition (pitch, line size ...) was fully random, we would end copying data from an incorrect layout into a new buffer
	// wasting lots of resources.
	//
	// XXX : There is a solution !!!
	//
	//       for images coming from FreeImage, we can ensure (in specifications) that lines are DWORD aligned
	//       So if we want to transfert a block a pixel starting at pointer B, we will work a virtual pointer B' and use SKIP_PIXELS and ROW_LENGTH
	//       B' has to be DWORD aligned and (B - B') has to be a multiple of DWORD
	//
	//
	// buffer
	// x
	//
	//          B'
	//           x  SKIP_PIXELS' +------------+       |
	//           |               |            |       |
	//           |               |            |       |
	//           |               |    Image   |       |
	//           |               |            |       |
	//           |               |            |       |
	//           |               +------------+       |
	// ---------><-------------------------------------
	//                               ROW LENGTH'
	//
	// XXX : If we are not working with FreeImage but with our own buffer, we have to meet theses requirements :
	//
	//         Row are DWORD aligned
	//
	//       See
	//         ImageTools::GetImageDescriptionForAlignedTexture(...) and
	//         ImageTools::GetMemoryRequirementForAlignedTexture(...)
	//

	char* GPUTexture::PreparePixelTransfert(ImageDescription const& desc)
	{
		char* result = (char*)desc.data;

		int pixel_size = desc.pixel_format.GetPixelSize();
		int row_length = desc.pitch_size / pixel_size;
		int skip_pixel = 0;

		// first pixel already aligned on DWORD ??
		uintptr_t b = (uintptr_t)desc.data;

		// find pointer b' before b that is DWORD aligned and at a distance multiple of pixel size from th real buffer.
		//
		// XXX : with some algebrae knowledge it would be done through a formula
		//       i use a algorithm to fullfill that purpose
		//
		// DWORD aligned => with 4 iterations i should find a pointer that correspond to our needs

		int i = 0;
		while (i < 4 && (b % 4 != 0))
		{
			b -= pixel_size;
			++i;
		}

		if (b % 4 != 0)
			return nullptr;  // cannot find a correct offset to meet wanted alignment after 4 iterations, this will never happen

		skip_pixel = (int)(((uintptr_t)desc.data) - b) / pixel_size;
		result = (char*)b;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, row_length);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixel);

		return result;
	}

	void GPUTexture::OnLastReferenceLost()
	{
		gpu_device->OnTextureUnused(this); // give underlying resource to GPUBufferPool
		GPUResource::OnLastReferenceLost();
	}

}; // namespace chaos
