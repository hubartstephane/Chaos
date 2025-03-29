#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

//
// Some reminders for OpenGL:
// -------------------------
//
//                       +1
//       +--------------+
//       |              |
//       |              |
//       |    screen    |
//       |     space    |
//       |              |
//       |              |
//       +--------------+
//     -1
//
//
//                       1
//       +--------------+
//       |     top      |
//       |              |
//       |    Texture   |  The OpenGL texture is oriented like this
//       |              |
//       |              |
//       |    bottom    |
//       +--------------+
//      0
//
// for FreeImage, the line storage is done with the convention below
//
//       +--------------+
//       | LINE 5       |
//       | LINE 4       |
//       | LINE 3       |
//       | LINE 2       |
//       | LINE 1       |
//       | LINE 0       |
//       +--------------+
//
// but for FreeImage, the coordinates used are (Y is inversed compared to OpenGL)
//
//      0,0
//       +--------------+
//       |     top      |
//       |              |
//       |              |
//       |              |
//       |              |
//       |    bottom    |
//       +--------------+
//

namespace chaos
{
	namespace GLTextureTools
	{
		PixelFormat GetPixelFormat(GLenum internal_format)
		{
			if (internal_format == GL_R8)
				return PixelFormat(PixelComponentType::UNSIGNED_CHAR, 1);
			if (internal_format == GL_RGB8)
				return PixelFormat(PixelComponentType::UNSIGNED_CHAR, 3);
			if (internal_format == GL_RGBA8)
				return PixelFormat(PixelComponentType::UNSIGNED_CHAR, 4);
			if (internal_format == GL_R32F)
				return PixelFormat(PixelComponentType::FLOAT, 1);
			if (internal_format == GL_RGB32F)
				return PixelFormat(PixelComponentType::FLOAT, 3);
			if (internal_format == GL_RGBA32F)
				return PixelFormat(PixelComponentType::FLOAT, 4);
			if (internal_format == GL_DEPTH24_STENCIL8)
				return PixelFormat(PixelComponentType::DEPTH_STENCIL, 1);
			return PixelFormat();
		}

		PixelFormat GetTexturePixelFormat(GLuint texture_id, GLint level)
		{
			if (texture_id != 0)
			{
				GLint internal_format = 0;
				glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
				return GetPixelFormat(internal_format);
			}
			return PixelFormat();
		}

		char* GetTextureImage(GLuint texture_id, GLint level, ImageDescription& desc)
		{
			char* result = nullptr;

			if (texture_id != 0)
			{
				GLint width = 0;
				glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_WIDTH, &width);
				if (width > 0)
				{
					GLint height = 0;
					glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_HEIGHT, &height);
					if (height > 0)
					{
						PixelFormat pixel_format = GetTexturePixelFormat(texture_id, level);
						if (pixel_format.IsValid())
						{
							GLPixelFormat gl_pixel_format = GetGLPixelFormat(pixel_format);
							if (gl_pixel_format.IsValid())
							{
								GLenum type = (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR) ? GL_UNSIGNED_BYTE : GL_FLOAT;

								int buffer_size = ImageTools::GetMemoryRequirementForAlignedTexture(pixel_format, width, height);

								result = new char[buffer_size];
								if (result != nullptr)
								{
									desc = ImageTools::GetImageDescriptionForAlignedTexture(pixel_format, width, height, result);

									glPixelStorei(GL_PACK_ALIGNMENT, 1);

									glPixelStorei(GL_PACK_ROW_LENGTH, desc.pitch_size / desc.pixel_format.GetPixelSize());

									glGetTextureImage(texture_id, level, gl_pixel_format.format, type, buffer_size, desc.data);
								}
							}
						}
					}
				}
			}
			return result;
		}

		bool IsArrayTextureType(TextureType type)
		{
			if (type == TextureType::Texture1DArray)
				return true;
			if (type == TextureType::Texture2DArray)
				return true;
			if (type == TextureType::TextureCubeMapArray)
				return true;
			return false;
		}

		bool IsFlatTextureType(TextureType type)
		{
			if (type == TextureType::Texture1D)
				return true;
			if (type == TextureType::Texture2D)
				return true;
			if (type == TextureType::Texture3D)
				return true;
			if (type == TextureType::TextureRectangle)
				return true;
			if (type == TextureType::TextureCubeMap)
				return true;
			return false;
		}

		TextureType ToFlatTextureType(TextureType type)
		{
			if (IsFlatTextureType(type)) // already flat
				return type;

			if (type == TextureType::Texture1DArray)
				return TextureType::Texture1D;
			if (type == TextureType::Texture2DArray)
				return TextureType::Texture2D;
			if (type == TextureType::TextureCubeMapArray)
				return TextureType::TextureCubeMap;

			return TextureType::Unknown;
		}

		TextureType ToArrayTextureType(TextureType type)
		{
			if (IsArrayTextureType(type)) // already array
				return type;

			if (type == TextureType::Texture1D)
				return TextureType::Texture1DArray;
			if (type == TextureType::Texture2D)
				return TextureType::Texture2DArray;
			if (type == TextureType::TextureCubeMap)
				return TextureType::TextureCubeMapArray;  // XXX : Texture3D and TextureRectangle has no correspondance in arrays

			return TextureType::Unknown;
		}

		int GetMipmapLevelCount(int width, int height)
		{
			return GetMipmapLevelCount(std::max(width, height));
		}

		int GetMipmapLevelCount(int width)
		{
			assert(width > 0);
			return BitTools::bsr(width) + 1;
		}

		GLPixelFormat GetGLPixelFormat(PixelFormat const& pixel_format) // format / internal format
		{
			// XXX : GL_LUMINANCE / GL_LUMINANCE8 deprecated in OpenGL 4.5
			if (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)
			{
				if (pixel_format.component_count == 1)
					return GLPixelFormat(GL_RED, GL_R8);
				if (pixel_format.component_count == 3)
					return GLPixelFormat(GL_BGR, GL_RGB8); // FreeImage produce BGR(A) images
				if (pixel_format.component_count == 4)
					return GLPixelFormat(GL_BGRA, GL_RGBA8);
			}
			else if (pixel_format.component_type == PixelComponentType::FLOAT)
			{
				if (pixel_format.component_count == 1)
					return GLPixelFormat(GL_RED, GL_R32F);
				if (pixel_format.component_count == 3)
					return GLPixelFormat(GL_RGB, GL_RGB32F);
				if (pixel_format.component_count == 4)
					return GLPixelFormat(GL_RGBA, GL_RGBA32F);
			}
			else if (pixel_format.component_type == PixelComponentType::DEPTH_STENCIL)
			{
				return GLPixelFormat(GL_NONE, GL_DEPTH24_STENCIL8);
			}
			return GLPixelFormat(GL_NONE, GL_NONE);
		}

		void GenTextureApplyParameters(GLuint texture_id, TextureDescription const& texture_description, GenTextureParameters const& parameters)
		{
			GLPixelFormat gl_formats = GetGLPixelFormat(texture_description.pixel_format);

			// there are to set of functions
			//   - glTexParameteri(TARGET ...)
			// and
			//   - glTextureParameteri(TEXTURE_ID ...)
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, parameters.wrap_s);
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, parameters.wrap_t);
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, parameters.wrap_r);

			glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, parameters.mag_filter);
			glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, parameters.min_filter);

			if (gl_formats.internal_format == GL_R8 || gl_formats.internal_format == GL_R32F)
			{
				glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_R, GL_RED);
				glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_G, GL_RED);
				glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_B, GL_RED);
				glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_A, GL_ONE);
			}

			if (parameters.build_mipmaps && parameters.reserve_mipmaps)
				if (texture_description.type != TextureType::TextureRectangle) // not working with RECTANGLE (crash)
					glGenerateTextureMipmap(texture_id);
		}

		TextureType GetTexture2DTypeFromSize(int width, int height, bool rectangle_texture)
		{
			if (width == height) // and power of 2 ?
				return TextureType::Texture2D;
			else if (height == 1)
				return TextureType::Texture1D;
			else
				return (rectangle_texture) ? TextureType::TextureRectangle : TextureType::Texture2D;
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
		// note : with SubImageDefinition, the padding maybe a huge value to skip lots of pixel at the end of the scanline
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

		char* PrepareGLTextureTransfert(ImageDescription const& desc)
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

	}; // namespace GLTextureTools

}; // namespace chaos
