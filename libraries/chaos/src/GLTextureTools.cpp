#include <chaos/GLTextureTools.h>
#include <chaos/MathTools.h>


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

	ImageDescription GLTextureTools::GetTextureImage(GLuint texture_id, GLint level)
	{
		ImageDescription result;
		if (texture_id != 0)
		{
			GLint width  = 0;				
			glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_WIDTH, &width);
			if (width > 0)
			{
				GLint height = 0;
				glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_HEIGHT, &height);
				if (height > 0)
				{
					GLint internal_format = 0;
					glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

					int component_type = 0;
					int component_count = 0;
					GLenum format = GL_NONE;
					GLenum type = GL_NONE;
					if (internal_format == GL_R8)
					{
						format = GL_RED;
						component_type = ImageDescription::TYPE_UNSIGNED_CHAR;
						component_count = 1;
					}
					else if (internal_format == GL_RGB8)
					{
						format = GL_BGR;
						component_type = ImageDescription::TYPE_UNSIGNED_CHAR;
						component_count = 3;
					}
					else if (internal_format == GL_RGBA8)
					{
						format = GL_BGRA;
						component_type = ImageDescription::TYPE_UNSIGNED_CHAR;
						component_count = 4;
					}













					if (component_type != 0 && component_count != 0)
					{
						result.width           = width;
						result.height          = height;
						result.component_type  = component_type;
						result.component_count = component_count;
						result.line_size       = width * result.GetPixelSize();
						result.pitch_size      = result.line_size; // no padding
						result.padding_size    = 0;

						int pixel_size = result.GetPixelSize();

						size_t bufsize = width * height * pixel_size;

						result.data = new char[bufsize];
						if (result.data != nullptr)
						{
							glPixelStorei(GL_PACK_ALIGNMENT, 1);
							glPixelStorei(GL_PACK_ROW_LENGTH, result.pitch_size / pixel_size);
							glGetTextureImage(texture_id, level, format, GL_UNSIGNED_BYTE, bufsize, result.data);
						}
						else
							result = ImageDescription();
					}
				}
			}					
		}
		return result;
	}

	bool GLTextureTools::IsArrayTextureType(GLenum type)
	{
		if (type == GL_TEXTURE_1D_ARRAY)
			return true;
		if (type == GL_TEXTURE_2D_ARRAY)
			return true;
		if (type == GL_TEXTURE_CUBE_MAP_ARRAY)
			return true;
		return false;
	}

	bool GLTextureTools::IsFlatTextureType(GLenum type)
	{
		if (type == GL_TEXTURE_1D)
			return true;
		if (type == GL_TEXTURE_2D)
			return true;
		if (type == GL_TEXTURE_3D)
			return true;
		if (type == GL_TEXTURE_RECTANGLE)
			return true;
		if (type == GL_TEXTURE_CUBE_MAP)
			return true;
		return false;
	}

	GLenum GLTextureTools::ToFlatTextureType(GLenum type)
	{
		if (IsFlatTextureType(type)) // already flat
			return type;

		if (type == GL_TEXTURE_1D_ARRAY)
			return GL_TEXTURE_1D;
		if (type == GL_TEXTURE_2D_ARRAY)
			return GL_TEXTURE_2D;
		if (type == GL_TEXTURE_CUBE_MAP_ARRAY)
			return GL_TEXTURE_CUBE_MAP;

		return GL_NONE;
	}

	GLenum GLTextureTools::ToArrayTextureType(GLenum type)
	{
		if (IsArrayTextureType(type)) // already array
			return type;

		if (type == GL_TEXTURE_1D)
			return GL_TEXTURE_1D_ARRAY;
		if (type == GL_TEXTURE_2D)
			return GL_TEXTURE_2D_ARRAY;
		if (type == GL_TEXTURE_CUBE_MAP)
			return GL_TEXTURE_CUBE_MAP_ARRAY;  // XXX : GL_TEXTURE_3D and GL_TEXTURE_RECTANGLE has no correspondance in arrays

		return GL_NONE;
	}

	int GLTextureTools::GetMipmapLevelCount(int width, int height)
	{
		return GetMipmapLevelCount(max(width, height));
	}

	int GLTextureTools::GetMipmapLevelCount(int width)
	{
		assert(width > 0);
		return MathTools::bsr(width) + 1;
	}

	std::pair<GLenum, GLenum> GLTextureTools::GetTextureFormats(int component_type, int component_count)
	{
		// XXX : GL_LUMINANCE / GL_LUMINANCE8 deprecated in OpenGL 4.5
		if (component_type == ImageDescription::TYPE_UNSIGNED_CHAR)
		{
			if (component_count == 1)
				return std::make_pair(GL_RED, GL_R8);
			if (component_count == 3)
				return std::make_pair(GL_BGR, GL_RGB8);
			if (component_count == 4)
				return std::make_pair(GL_BGRA, GL_RGBA8);				
		}
		else if (component_type == ImageDescription::TYPE_FLOAT)
		{
			if (component_count == 1)
				return std::make_pair(GL_NONE, GL_NONE);
			if (component_count == 3)
				return std::make_pair(GL_NONE, GL_NONE);
			if (component_count == 4)
				return std::make_pair(GL_NONE, GL_NONE);	
		}
		return std::make_pair(GL_NONE, GL_NONE);
	}

	GenTextureResult GLTextureTools::GenTexture(ImageDescription const & image, GenTextureParameters const & parameters)
	{
		GenTextureResult result;
		if (!image.IsValid() || image.IsEmpty())
			return result;

		GLenum target = GetTextureTargetFromSize(image.width, image.height, parameters.rectangle_texture);  // compute the format
		
		glCreateTextures(target, 1, &result.texture_id);
		if (result.texture_id > 0)
		{  
			// choose format and internal format (beware FreeImage is BGR/BGRA)
			std::pair<GLenum, GLenum> all_formats = GetTextureFormats(image.component_type, image.component_count);
			assert(all_formats.first != GL_NONE);
			assert(all_formats.second != GL_NONE);

			GLenum format          = all_formats.first;
			GLenum internal_format = all_formats.second;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, image.pitch_size / image.GetPixelSize());

			// store the pixels
			if (target == GL_TEXTURE_1D)
			{
				int level_count = GetMipmapLevelCount(image.width);
				glTextureStorage1D(result.texture_id, level_count, internal_format, image.width);
				glTextureSubImage1D(result.texture_id, 0, 0, image.width, format, GL_UNSIGNED_BYTE, image.data);
			}
			else
			{
				int level_count = GetMipmapLevelCount(image.width, image.height);
				glTextureStorage2D(result.texture_id, level_count, internal_format, image.width, image.height);
				glTextureSubImage2D(result.texture_id, 0, 0, 0, image.width, image.height, format, GL_UNSIGNED_BYTE, image.data);
			}

			result.texture_description.type            = target;
			result.texture_description.internal_format = internal_format;
			result.texture_description.width           = image.width;
			result.texture_description.height          = image.height;
			result.texture_description.depth           = 1;

			// apply parameters
			GenTextureApplyParameters(result, parameters);
		}
		return result;
	}

	GenTextureResult GLTextureTools::GenTexture(FIBITMAP const * image, GenTextureParameters const & parameters)
	{
		assert(image != nullptr);
		return GenTexture(ImageTools::GetImageDescription(image), parameters);
	}

	GenTextureResult GLTextureTools::GenTexture(char const * filename, GenTextureParameters const & parameters)
	{
		GenTextureResult result;

		FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
		if (image != nullptr)
		{
			result = GenTexture(image, parameters);
			FreeImage_Unload(image);  
		}
		return result;
	}

	// There are lots of very uncleared referenced for faces orientation
	// Most of pictures found one GoogleImage do not correspond to OpenGL but DirectX
	// There are differences between OpenGL & DirectX implementation
	//
	// I found this one that seems to work almost fine with OpenGL 
	//  - horizontal skybox OK
	//  - vertical   skybox OK
	//  - multiple   skybox OK
	//
	// Problems : the face junctions are good, but the skybox is inverted (top to down).
	//            the simplest solution is to access in shader the cube map with "-direction" instead of "direction" :
	//
	//                vec4 color = texture(cube_texture, -direction)
	//
	//            => in fact inverting the Y should be enougth
	//
	//                vec4 color = texture(cube_texture, direction * vec3(1.0,   -1.0,    1.0))
	//
	// http://www.3dcpptutorials.sk/index.php?id=24
	//
	//        +------+
	//        |  -Y  |
	//        |      |
	// +------+------+------+------+
	// |  -X  |  +Z  |  +X  |  -Z  |
	// |      |      |      |      |
	// +------+------+------+------+
	//        |  +Y  |
	//        |      |
	//        +------+
	//
	//  0 = +X = right
	//  1 = -X = left
	//  2 = +Y = bottom
	//  3 = -Y = top
	//  4 = +Z = front
	//  5 = -Z = back
	//
	// Differences between comes from the fact that OpenGL & Direct have different axis
	//   +Y / -Y   are to be swapped (from one implementation to the other)
	//   +Z / -Z   are to be swapped 
	//
	// Textures for OpenGL are oriented :
	//
	//  ^
	//  |
	//  |
	//  +------>
	// 0, 0
	//
	// Textures for DirectX are oriented :
	//
	// 0, 0
	//  +------>
	//  |
	//  |
	//  v
	//

	int GLTextureTools::GetLayerValueFromCubeMapFace(GLenum face, int level)
	{
		if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_X)
			return 0 + 6 * level;
		if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_X)
			return 1 + 6 * level;
		if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_Y)
			return 2 + 6 * level;
		if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
			return 3 + 6 * level;
		if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
			return 4 + 6 * level;
		if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
			return 5 + 6 * level;
		return -1;
	}

	GenTextureResult GLTextureTools::GenTexture(SkyBoxImages const * skybox, GenTextureParameters const & parameters)
	{
		assert(skybox != nullptr);
		assert(!skybox->IsEmpty());

		GLenum target = GL_TEXTURE_CUBE_MAP;

		GenTextureResult result;
		glCreateTextures(target, 1, &result.texture_id);
		if (result.texture_id > 0)
		{
			int bpp  = skybox->GetSkyBoxBPP();
			int size = skybox->GetSkyBoxSize();

			std::pair<GLenum, GLenum> all_formats = GetTextureFormatsFromBPP(bpp);
			assert(all_formats.first != GL_NONE);
			assert(all_formats.second != GL_NONE);

			GLenum format          = all_formats.first;
			GLenum internal_format = all_formats.second;

			GLenum targets[] = {
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // LEFT
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, // RIGHT      
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // BOTTOM
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // TOP
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // FRONT
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // BACK
			};

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int level_count = GetMipmapLevelCount(size, size);
			glTextureStorage2D(result.texture_id, level_count, internal_format, size, size);

			for (int i = SkyBoxImages::IMAGE_LEFT ; i <= SkyBoxImages::IMAGE_BACK ; ++i)
			{
				ImageDescription image = skybox->GetImageFaceDescription(i);

				int pixel_size = image.GetPixelSize();

				void const * data = image.data;
				GLint        unpack_row_length = image.pitch_size / pixel_size;
				char       * new_buffer = nullptr;

				if (skybox->IsSingleImage()) // in single image, there may be some inversion to correct with a temporary buffer
				{
					glm::ivec3 position_and_flags = skybox->GetPositionAndFlags(i);
					if (position_and_flags.z == SkyBoxImages::IMAGE_CENTRAL_SYMETRY)
					{
						new_buffer = new char[image.width * image.height * pixel_size];
						if (new_buffer != nullptr)
						{
							ImageDescription new_image = image;

							new_image.data         = new_buffer;
							new_image.pitch_size   = new_image.line_size;
							new_image.padding_size = 0;

							ImageTools::CopyPixelsWithCentralSymetry(image, new_image, 0, 0, 0, 0, image.width, image.height); // do the symmetry
							unpack_row_length = 0;
							data              = new_buffer;
						}
					}
				}

				glPixelStorei(GL_UNPACK_ROW_LENGTH, unpack_row_length); // do not remove this line from the loop. Maybe future implementation will accept                                                                             // image with same size but different pitch

				int depth = GetLayerValueFromCubeMapFace(targets[i], 0);

				glTextureSubImage3D(result.texture_id, 0, 0, 0, depth, image.width, image.height, 1, format, GL_UNSIGNED_BYTE, data);

				if (new_buffer != nullptr)
					delete [](new_buffer);
			}

			result.texture_description.type            = target;
			result.texture_description.internal_format = internal_format;
			result.texture_description.width           = size;
			result.texture_description.height          = size;
			result.texture_description.depth           = 1;

			// this is smoother to clamp at edges
			GenTextureParameters tmp = parameters;
			tmp.wrap_s = GL_CLAMP_TO_EDGE;
			tmp.wrap_r = GL_CLAMP_TO_EDGE;
			tmp.wrap_t = GL_CLAMP_TO_EDGE;
			GenTextureApplyParameters(result, tmp);
		}
		return result;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters)
	{
		GenTextureResult result = GenTexture(image, parameters);
		if (result.texture_id > 0)
			return new Texture(result.texture_id, result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(FIBITMAP const * image, GenTextureParameters const & parameters)
	{
		GenTextureResult result = GenTexture(image, parameters);
		if (result.texture_id > 0)
			return new Texture(result.texture_id, result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(SkyBoxImages const * skybox, GenTextureParameters const & parameters)
	{
		GenTextureResult result = GenTexture(skybox, parameters);
		if (result.texture_id > 0)
			return new Texture(result.texture_id, result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(char const * filename, GenTextureParameters const & parameters)
	{
		GenTextureResult result = GenTexture(filename, parameters);
		if (result.texture_id > 0)
			return new Texture(result.texture_id, result.texture_description);
		return nullptr;
	}

	void GLTextureTools::GenTextureApplyParameters(GenTextureResult const & result, GenTextureParameters const & parameters)
	{
		// there are to set of functions
		//   - glTexParameteri(TARGET ...)
		// and
		//   - glTextureParameteri(TEXTURE_ID ...)
		glTextureParameteri(result.texture_id, GL_TEXTURE_WRAP_S, parameters.wrap_s);
		glTextureParameteri(result.texture_id, GL_TEXTURE_WRAP_T, parameters.wrap_t);
		glTextureParameteri(result.texture_id, GL_TEXTURE_WRAP_R, parameters.wrap_r);
		glTextureParameteri(result.texture_id, GL_TEXTURE_MAG_FILTER, parameters.mag_filter);
		glTextureParameteri(result.texture_id, GL_TEXTURE_MIN_FILTER, parameters.min_filter);

		if (result.texture_description.internal_format == GL_R8)
		{
			glTextureParameteri(result.texture_id, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTextureParameteri(result.texture_id, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTextureParameteri(result.texture_id, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTextureParameteri(result.texture_id, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}

		if (parameters.build_mipmaps)
			if (result.texture_description.type != GL_TEXTURE_RECTANGLE) // not working with RECTANGLE (crash)
				glGenerateTextureMipmap(result.texture_id);
	}

	GLenum GLTextureTools::GetTextureTargetFromSize(int width, int height, bool rectangle_texture)
	{
		if (width == height) // and power of 2 ?
			return GL_TEXTURE_2D;
		else if (height == 1)
			return GL_TEXTURE_1D;
		else 
			return (rectangle_texture)? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
	}

	// GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP, or GL_TEXTURE_CUBE_MAP_ARRAY


}; // namespace chaos
