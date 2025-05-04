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
				return TextureType::TextureCubeMapArray;

			return TextureType::Unknown; // other types can't be array
		}

		int GetMipmapLevelCount(int width, int height, int depth)
		{
			return GetMipmapLevelCount(std::max(std::max(width, height), depth));
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

		TextureType GetTexture2DTypeFromSize(int width, int height)
		{
			return (height == 1)?
				TextureType::Texture1D:
				TextureType::Texture2D;
		}

	}; // namespace GLTextureTools

}; // namespace chaos
