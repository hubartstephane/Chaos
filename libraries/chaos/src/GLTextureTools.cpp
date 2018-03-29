#include <chaos/GLTextureTools.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

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

	GLPixelFormat::GLPixelFormat(GLenum in_format, GLenum in_internal_format) :
		format(in_format),
		internal_format(in_internal_format) {}

	bool GLPixelFormat::IsValid() const
	{
		if (
			format != GL_RED &&
			format != GL_BGR &&
			format != GL_BGRA &&
			format != GL_RGB &&
			format != GL_RGBA)
			return false;

		if (
			internal_format != GL_R8 &&
			internal_format != GL_RGB8 &&
			internal_format != GL_RGBA8 &&
			internal_format != GL_R32F &&
			internal_format != GL_RGB32F &&
			internal_format != GL_RGBA32F)
			return false;

		return true;
	}


	PixelFormat GLTextureTools::GetTexturePixelFormat(GLuint texture_id, GLint level)
	{
		if (texture_id != 0)
		{
			GLint internal_format = 0;
			glGetTextureLevelParameteriv(texture_id, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);	

			if (internal_format == GL_R8)
				return PixelFormat(PixelFormat::TYPE_UNSIGNED_CHAR, 1);
			if (internal_format == GL_RGB8)
				return PixelFormat(PixelFormat::TYPE_UNSIGNED_CHAR, 3);
			if (internal_format == GL_RGBA8)
				return PixelFormat(PixelFormat::TYPE_UNSIGNED_CHAR, 4);
			if (internal_format == GL_R32F)
				return PixelFormat(PixelFormat::TYPE_FLOAT, 1);
			if (internal_format == GL_RGB32F)
				return PixelFormat(PixelFormat::TYPE_FLOAT, 3);
			if (internal_format == GL_RGBA32F)
				return PixelFormat(PixelFormat::TYPE_FLOAT, 4);
		}
		return PixelFormat();
	}

	char * GLTextureTools::GetTextureImage(GLuint texture_id, GLint level, ImageDescription & desc)
	{
		char * result = nullptr;

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
					PixelFormat pixel_format = GetTexturePixelFormat(texture_id, level);
					if (pixel_format.IsValid())
					{
						GLPixelFormat gl_pixel_format = GetGLPixelFormat(pixel_format);
						if (gl_pixel_format.IsValid())
						{
							GLenum type = (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)? GL_UNSIGNED_BYTE : GL_FLOAT;

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

	GLPixelFormat GLTextureTools::GetGLPixelFormat(PixelFormat const & pixel_format) // format / internal format
	{
		// XXX : GL_LUMINANCE / GL_LUMINANCE8 deprecated in OpenGL 4.5
		if (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
		{
			if (pixel_format.component_count == 1)
				return GLPixelFormat(GL_RED, GL_R8);
			if (pixel_format.component_count == 3)
				return GLPixelFormat(GL_BGR, GL_RGB8); // FreeImage produce BGR(A) images
			if (pixel_format.component_count == 4)
				return GLPixelFormat(GL_BGRA, GL_RGBA8);
		}
		else if (pixel_format.component_type == PixelFormat::TYPE_FLOAT) 
		{
			if (pixel_format.component_count == 1)
				return GLPixelFormat(GL_RED, GL_R32F);
			if (pixel_format.component_count == 3)
				return GLPixelFormat(GL_RGB, GL_RGB32F);
			if (pixel_format.component_count == 4)
				return GLPixelFormat(GL_RGBA, GL_RGBA32F);
		}
		return GLPixelFormat(GL_NONE, GL_NONE);
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
			GLPixelFormat gl_formats = GetGLPixelFormat(image.pixel_format);
			assert(gl_formats.IsValid());

			GLenum format          = gl_formats.format;
			GLenum internal_format = gl_formats.internal_format;
			GLenum type            = (image.pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)? GL_UNSIGNED_BYTE : GL_FLOAT;

			char * texture_buffer = PrepareGLTextureTransfert(image);
			if (texture_buffer != nullptr)
			{			
				// store the pixels
				if (target == GL_TEXTURE_1D)
				{
					int level_count = GetMipmapLevelCount(image.width);
					glTextureStorage1D(result.texture_id, level_count, internal_format, image.width);
					glTextureSubImage1D(result.texture_id, 0, 0, image.width, format, type, texture_buffer);
				}
				else
				{
					int level_count = GetMipmapLevelCount(image.width, image.height);
					glTextureStorage2D(result.texture_id, level_count, internal_format, image.width, image.height);
					glTextureSubImage2D(result.texture_id, 0, 0, 0, image.width, image.height, format, type, texture_buffer);
				}

				result.texture_description.type            = target;
				result.texture_description.internal_format = internal_format;
				result.texture_description.width           = image.width;
				result.texture_description.height          = image.height;
				result.texture_description.depth           = 1;

				// apply parameters
				GenTextureApplyParameters(result, parameters);
			}
			else
			{
				glDeleteTextures(1, &result.texture_id);
				result = GenTextureResult();
			}
		}
		return result;
	}

	GenTextureResult GLTextureTools::GenTexture(FIBITMAP * image, GenTextureParameters const & parameters)
	{
		assert(image != nullptr);
		return GenTexture(ImageTools::GetImageDescription(image), parameters);
	}

	GenTextureResult GLTextureTools::GenTexture(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters)
	{
		GenTextureResult result;

		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p)) 
		{
			FilePathParam path(p, config_path);
			return GenTexture(path, parameters);
		}
#if 0
		// skybox descriptions ?
		nlohmann::json const * faces = JSONTools::GetStructure(json, "faces");
		if (faces != nullptr)
		{
			if (faces->is_array() || faces->is_object())
			{
				std::string left;
				std::string right;
				std::string top;
				std::string bottom;
				std::string front;
				std::string back;

				std::string single;

				bool single_image   = false;
				bool multiple_image = false;

				SkyBoxImages skybox;
				if (faces->is_array())
				{
					if (faces->size() == 1)
					{
						single_image |= JSONTools::GetAttributeByIndex(faces, 0, single);
					}
					else
					{
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 0, left);
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 1, right);
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 2, top);
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 3, bottom);
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 4, front);
						multiple_image |= JSONTools::GetAttributeByIndex(faces, 5, back);
					}				
				}

				if (single_image)
					skybox = SkyBoxTools::LoadSingleSkyBox(single);


			}




		}	
#endif
		return result;
	}

	GenTextureResult GLTextureTools::GenTexture(FilePathParam const & path, GenTextureParameters const & parameters)
	{
		GenTextureResult result;

		Buffer<char> buffer = FileTools::LoadFile(path, false);
		if (buffer != nullptr)
		{
			FIBITMAP * image = ImageTools::LoadImageFromBuffer(buffer);
			if (image != nullptr)
			{
				result = GenTexture(image, parameters);
				FreeImage_Unload(image); 
			}
			else
			{
				nlohmann::json json;
				if (JSONTools::Parse(buffer, json))
					result = GenTexture(json, path.GetResolvedPath(), parameters);
			}
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

	int GLTextureTools::GetCubeMapLayerValueFromSkyBoxFace(int face, int level)
	{
#if 0
		// previous code was using GL_enum
		GLenum targets[] = {
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // LEFT
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, // RIGHT      
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // BOTTOM
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // TOP
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // FRONT
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // BACK
		};

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

#endif

		if (face == SkyBoxImages::IMAGE_RIGHT)
			return 0 + 6 * level;
		if (face == SkyBoxImages::IMAGE_LEFT)
			return 1 + 6 * level;
		if (face == SkyBoxImages::IMAGE_BOTTOM)
			return 2 + 6 * level;
		if (face == SkyBoxImages::IMAGE_TOP)
			return 3 + 6 * level;
		if (face == SkyBoxImages::IMAGE_FRONT)
			return 4 + 6 * level;
		if (face == SkyBoxImages::IMAGE_BACK)
			return 5 + 6 * level;
		return -1;
	}

	GenTextureResult GLTextureTools::GenTexture(SkyBoxImages const * skybox, chaos::PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters)
	{
		assert(skybox != nullptr);
		assert(!skybox->IsEmpty());

		GenTextureResult result;

		PixelFormat final_pixel_format = skybox->GetMergedPixelFormat(merge_params);

		int size = skybox->GetSkyBoxSize();

		// detect whether some conversion will be required and the size of buffer required (avoid multiple allocations)
		//
		// There are 2 kinds of alterations to do :
		//  - if there is a central symetry to do, will have to make a special 'copy'
		//
		//  - LUMINANCE textures are obsolet in GL 4. The texture give RED pixels except if we use 
		//    glTextureParameteri( ..., GL_TEXTURE_SWIZZLE_XXX, GL_RED)
		//
		//    That is possible if the whole cubemap is LUMINANCE
		//    But if some face are LUMINANCE and some not, we ll have to do 
		//    the conversion ourselves because we can not apply GL_TEXTURE_SWIZZLE_XXX on 
		//    independant face
		//
		bool is_single_image = skybox->IsSingleImage();

		size_t required_allocation = 0;

		bool face_valid[6] = { false, false, false, false, false, false };
		bool conversion_required[6] = { false, false, false, false, false, false };
		bool central_symetry[6] = { false, false, false, false, false, false };

		for (int i = SkyBoxImages::IMAGE_LEFT; i <= SkyBoxImages::IMAGE_BACK; ++i)
		{
			// ensure the image is valid and not empty
			ImageDescription image = skybox->GetImageFaceDescription(i);
			if (image.data == nullptr || !image.pixel_format.IsValid())
				continue;
			face_valid[i] = true;

			// test whether a conversion/copy is required
			if ((image.pixel_format.component_count == 1) && (final_pixel_format.component_count != 1))
				conversion_required[i] = true;

			if (is_single_image)
			{
				glm::ivec3 position_and_flags = skybox->GetPositionAndFlags(i);
				if (position_and_flags.z == SkyBoxImages::IMAGE_CENTRAL_SYMETRY)
					central_symetry[i] = conversion_required[i] = true;
			}
			// compute memory required
			if (conversion_required[i])
				required_allocation = max(required_allocation, (size_t)ImageTools::GetMemoryRequirementForAlignedTexture(final_pixel_format, size, size));
		}

		// allocate the buffer
		char * conversion_buffer = nullptr;
		if (required_allocation != 0)
		{
			conversion_buffer = new char[required_allocation];
			if (conversion_buffer == nullptr)
				return result;
		}

		// GPU-allocate the texture
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &result.texture_id);
		if (result.texture_id > 0)
		{
			GLPixelFormat gl_final_pixel_format = GLTextureTools::GetGLPixelFormat(final_pixel_format);

			// generate the cube-texture : select as internal format the one given by the MERGED PIXEL FORMAT
			int level_count = GetMipmapLevelCount(size, size);
			glTextureStorage2D(result.texture_id, level_count, gl_final_pixel_format.internal_format, size, size);

			// fill the faces in GPU with the images of SkyBox
			for (int i = SkyBoxImages::IMAGE_LEFT; i <= SkyBoxImages::IMAGE_BACK; ++i)
			{
				// ensure the image is valid and not empty
				if (!face_valid[i])
					continue;

				// do the conversion, central symetry
				ImageDescription image = skybox->GetImageFaceDescription(i);

				ImageDescription effective_image = (conversion_required[i]) ?
					ImageTools::ConvertPixels(image, final_pixel_format, conversion_buffer, central_symetry[i]) :
					image;

				// fill glPixelStorei(...)
				// do not remove this line from the loop. Maybe future implementation will accept image with same size but different pitch          

				char * texture_buffer = PrepareGLTextureTransfert(effective_image);
				if (texture_buffer != nullptr)
				{
					// fill GPU
					int depth = GetCubeMapLayerValueFromSkyBoxFace(i, 0);

					GLPixelFormat gl_face_pixel_format = GLTextureTools::GetGLPixelFormat(effective_image.pixel_format);

					glTextureSubImage3D(
						result.texture_id,
						0,
						0, 0, depth,
						size, size, 1,
						gl_face_pixel_format.format,
						effective_image.pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR ? GL_UNSIGNED_BYTE : GL_FLOAT,
						texture_buffer
					);								
				}
			}

			// finalize the result information
			result.texture_description.type = GL_TEXTURE_CUBE_MAP;
			result.texture_description.internal_format = gl_final_pixel_format.internal_format;
			result.texture_description.width = size;
			result.texture_description.height = size;
			result.texture_description.depth = 1;

			// this is smoother to clamp at edges
			GenTextureParameters tmp = parameters;
			tmp.wrap_s = GL_CLAMP_TO_EDGE;
			tmp.wrap_r = GL_CLAMP_TO_EDGE;
			tmp.wrap_t = GL_CLAMP_TO_EDGE;
			GenTextureApplyParameters(result, tmp);
		}

		// release the buffer
		if (conversion_buffer != nullptr)
			delete[](conversion_buffer);

		return result;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters)
	{
		GenTextureResult texture_result = GenTexture(json, config_path, parameters);
		if (texture_result.texture_id > 0)
			return new Texture(texture_result.texture_id, texture_result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters)
	{
		GenTextureResult texture_result = GenTexture(image, parameters);
		if (texture_result.texture_id > 0)
			return new Texture(texture_result.texture_id, texture_result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(FIBITMAP * image, GenTextureParameters const & parameters)
	{
		GenTextureResult texture_result = GenTexture(image, parameters);
		if (texture_result.texture_id > 0)
			return new Texture(texture_result.texture_id, texture_result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(SkyBoxImages const * skybox, PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters)
	{
		GenTextureResult texture_result = GenTexture(skybox, merge_params, parameters);
		if (texture_result.texture_id > 0)
			return new Texture(texture_result.texture_id, texture_result.texture_description);
		return nullptr;
	}

	boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters)
	{
		GenTextureResult texture_result = GenTexture(path, parameters);
		if (texture_result.texture_id > 0)
			return new Texture(texture_result.texture_id, texture_result.texture_description);
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

		if (result.texture_description.internal_format == GL_R8 || result.texture_description.internal_format == GL_R32F)
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

	char * GLTextureTools::PrepareGLTextureTransfert(ImageDescription const & desc)
	{	
		char * result = (char*)desc.data;

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

}; // namespace chaos
