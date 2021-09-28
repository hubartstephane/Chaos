#include <chaos/Chaos.h>

namespace chaos
{


	GPUTexture * GPUTextureLoader::LoadObject(char const * name, JSONConfig const & config, GenTextureParameters const & parameters) const
	{
		GPUTexture* result = LoadObjectHelper(name, config, [this, &parameters](JSONConfig const & config) 
		{
			return GenTextureObject(config, parameters);
		});
		if (result != nullptr)
		{
			if (manager != nullptr)
				if (!StringTools::IsEmpty(result->GetName())) // would like to insert the resource in manager, but name is empty
					manager->textures.push_back(result);
		}
		return result;
	}

	GPUTexture * GPUTextureLoader::LoadObject(FilePathParam const & path, char const * name, GenTextureParameters const & parameters) const
	{
		GPUTexture* result = LoadObjectHelper(path, name, [this, &parameters](FilePathParam const& path) 
		{
			return GenTextureObject(path, parameters);
		});
		if (result != nullptr)
		{
			if (manager != nullptr)
				if (!StringTools::IsEmpty(result->GetName())) // would like to insert the resource in manager, but name is empty
					manager->textures.push_back(result);
		}
		return result;
	}

	bool GPUTextureLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager != nullptr && manager->FindTextureByPath(path) != nullptr);
	}

	bool GPUTextureLoader::IsNameAlreadyUsedInManager(ObjectRequest request) const
	{ 
		return (manager != nullptr && manager->FindTexture(request) != nullptr);
	}
	
	GPUTexture * GPUTextureLoader::GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters) const
	{
		GPUTexture * result = nullptr;

		if (!image.IsValid(true) || image.IsEmpty(true))
			return nullptr;

		GLenum target = GLTextureTools::GetTextureTargetFromSize(image.width, image.height, parameters.rectangle_texture);  // compute the format

		GLuint texture_id = 0;
		glCreateTextures(target, 1, &texture_id);
		if (texture_id > 0)
		{  
			// choose format and internal format (beware FreeImage is BGR/BGRA)
			GLPixelFormat gl_formats = GLTextureTools::GetGLPixelFormat(image.pixel_format);
			assert(gl_formats.IsValid());

			GLenum format          = gl_formats.format;
			GLenum internal_format = gl_formats.internal_format;
			GLenum type            = (image.pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)? 
				GL_UNSIGNED_BYTE : 
				GL_FLOAT;

			// get the buffer for the pixels
			char * texture_buffer = (image.data == nullptr)?
				nullptr:
				GLTextureTools::PrepareGLTextureTransfert(image);

			// shuxxx try to work with parameter.level and parameter.border

			// create the texture
			if (target == GL_TEXTURE_1D)
			{
				int level_count = (parameters.reserve_mipmaps) ?
					GLTextureTools::GetMipmapLevelCount(image.width) :
					1;
				glTextureStorage1D(texture_id, level_count, internal_format, image.width);
				if (texture_buffer != nullptr)
					glTextureSubImage1D(texture_id, 0, 0, image.width, format, type, texture_buffer);
			}
			else
			{
				int level_count = (parameters.reserve_mipmaps) ?
					GLTextureTools::GetMipmapLevelCount(image.width, image.height) : 
					1;
				glTextureStorage2D(texture_id, level_count, internal_format, image.width, image.height);
				if (texture_buffer != nullptr)
					glTextureSubImage2D(texture_id, 0, 0, 0, image.width, image.height, format, type, texture_buffer);
			}

			TextureDescription texture_description;
			texture_description.type = target;
			texture_description.pixel_format = image.pixel_format;
			texture_description.width = image.width;
			texture_description.height = image.height;
			texture_description.depth = 1;

			// apply parameters
			GLTextureTools::GenTextureApplyParameters(texture_id, texture_description, parameters);
			result = new GPUTexture(texture_id, texture_description);
		}
		return result;
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(FIBITMAP * image, GenTextureParameters const & parameters) const
	{
		assert(image != nullptr);
		return GenTextureObject(ImageTools::GetImageDescription(image), parameters);
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// load the buffer
		GPUTexture * result = nullptr;

		Buffer<char> ascii_buffer = FileTools::LoadFile(path, true); // ascii mode for JSON 
		if (ascii_buffer != nullptr)
		{
			// while i am not sure an additionnal 0 in buffer wont be treated as a corruption by Free_Image
			// i work with a clamped buffer without this ascii 0 terminal
			Buffer<char> noascii_buffer;
			noascii_buffer.data = ascii_buffer.data;
			noascii_buffer.bufsize = ascii_buffer.bufsize - 1; 

			FIBITMAP * image = ImageTools::LoadImageFromBuffer(noascii_buffer);
			if (image != nullptr)
			{
				result = GenTextureObject(image, parameters);
				FreeImage_Unload(image); 
			}
			else
			{
				nlohmann::json json;
				if (JSONTools::ParseRecursive(ascii_buffer, path.GetResolvedPath(), json))
					result = GenTextureObject(JSONConfig(json, path.GetResolvedPath()), parameters);
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

	int GPUTextureLoader::GetCubeMapLayerValueFromSkyBoxFace(SkyBoxImageType face, int level)
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

		if (face == SkyBoxImageType::IMAGE_RIGHT)
			return 0 + 6 * level;
		if (face == SkyBoxImageType::IMAGE_LEFT)
			return 1 + 6 * level;
		if (face == SkyBoxImageType::IMAGE_BOTTOM)
			return 2 + 6 * level;
		if (face == SkyBoxImageType::IMAGE_TOP)
			return 3 + 6 * level;
		if (face == SkyBoxImageType::IMAGE_FRONT)
			return 4 + 6 * level;
		if (face == SkyBoxImageType::IMAGE_BACK)
			return 5 + 6 * level;
		return -1;
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(SkyBoxImages const * skybox, PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters) const
	{
		assert(skybox != nullptr);

		GPUTexture * result = nullptr;

		if (skybox->IsEmpty())
			return nullptr;

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

		for (size_t i = (int)SkyBoxImageType::IMAGE_LEFT; i <= (int)SkyBoxImageType::IMAGE_BACK; ++i)
		{
			// ensure the image is valid and not empty
			ImageDescription image = skybox->GetImageFaceDescription((SkyBoxImageType)i);
			if (image.data == nullptr || !image.pixel_format.IsValid())
				continue;
			face_valid[i] = true;

			// test whether a conversion/copy is required
			if ((image.pixel_format.component_count == 1) && (final_pixel_format.component_count != 1))
				conversion_required[i] = true;

			if (is_single_image)
			{
				glm::ivec3 position_and_flags = skybox->GetPositionAndFlags((SkyBoxImageType)i);
				if (position_and_flags.z == (int)ImageTransform::CENTRAL_SYMETRY)
					central_symetry[i] = conversion_required[i] = true;
			}
			// compute memory required
			if (conversion_required[i])
				required_allocation = std::max(required_allocation, (size_t)ImageTools::GetMemoryRequirementForAlignedTexture(final_pixel_format, size, size));
		}

		// allocate the buffer
		char * conversion_buffer = nullptr;
		if (required_allocation != 0)
		{
			conversion_buffer = new char[required_allocation];
			if (conversion_buffer == nullptr)
				return nullptr;
		}

		// GPU-allocate the texture
		GLuint texture_id = 0;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture_id);

		if (texture_id > 0)
		{
			GLPixelFormat gl_final_pixel_format = GLTextureTools::GetGLPixelFormat(final_pixel_format);

			// generate the cube-texture : select as internal format the one given by the MERGED PIXEL FORMAT
			int level_count = (parameters.reserve_mipmaps) ?
				GLTextureTools::GetMipmapLevelCount(size, size) :
				1;
			glTextureStorage2D(texture_id, level_count, gl_final_pixel_format.internal_format, size, size);

			// fill the faces in GPU with the images of SkyBox
			for (size_t i = (int)SkyBoxImageType::IMAGE_LEFT; i <= (int)SkyBoxImageType::IMAGE_BACK; ++i)
			{
				// ensure the image is valid and not empty
				if (!face_valid[i])
					continue;

				// do the conversion, central symetry
				ImageDescription image = skybox->GetImageFaceDescription((SkyBoxImageType)i);

				ImageDescription effective_image = (conversion_required[i]) ?
					ImageTools::ConvertPixels(image, final_pixel_format, conversion_buffer, central_symetry[i]? ImageTransform::CENTRAL_SYMETRY : ImageTransform::NO_TRANSFORM) :
					image;

				// fill glPixelStorei(...)
				// do not remove this line from the loop. Maybe future implementation will accept image with same size but different pitch          

				char * texture_buffer = GLTextureTools::PrepareGLTextureTransfert(effective_image);
				if (texture_buffer != nullptr)
				{
					// fill GPU
					int depth = GetCubeMapLayerValueFromSkyBoxFace((SkyBoxImageType)i, 0);

					GLPixelFormat gl_face_pixel_format = GLTextureTools::GetGLPixelFormat(effective_image.pixel_format);

					glTextureSubImage3D(
						texture_id,
						0,
						0, 0, depth,
						size, size, 1,
						gl_face_pixel_format.format,
						effective_image.pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR ? GL_UNSIGNED_BYTE : GL_FLOAT,
						texture_buffer
					);								
				}
			}

			// finalize the result information
			TextureDescription texture_description;
			texture_description.type = GL_TEXTURE_CUBE_MAP;
			texture_description.pixel_format = final_pixel_format;
			texture_description.width = size;
			texture_description.height = size;
			texture_description.depth = 1;

			// this is smoother to clamp at edges
			GenTextureParameters tmp = parameters;
			tmp.wrap_s = GL_CLAMP_TO_EDGE;
			tmp.wrap_r = GL_CLAMP_TO_EDGE;
			tmp.wrap_t = GL_CLAMP_TO_EDGE;

			GLTextureTools::GenTextureApplyParameters(texture_id, texture_description, tmp);
			result = new GPUTexture(texture_id, texture_description);
		}

		// release the buffer
		if (conversion_buffer != nullptr)
			delete[](conversion_buffer);

		return result;
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(JSONConfig const & config, GenTextureParameters const & parameters) const
	{
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(config.json, "path", p))
		{
			FilePathParam path(p, config.config_path);
			return GenTextureObject(path, parameters);
		}

		// skybox descriptions ?
		nlohmann::json const * faces = JSONTools::GetStructure(config.json, "faces");
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

				bool single_image = false;
				bool multiple_image = false;

				SkyBoxImages skybox;
				if (faces->is_array())
				{
					if (faces->size() == 1)
					{
						single_image |= JSONTools::GetAttributeByIndex(*faces, 0, single);
					}
					else
					{
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 0, left);
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 1, right);
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 2, top);
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 3, bottom);
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 4, front);
						multiple_image |= JSONTools::GetAttributeByIndex(*faces, 5, back);
					}
				}
				else
				{
					single_image |= JSONTools::GetAttribute(*faces, "single", single);
					if (!single_image)
					{
						multiple_image |= JSONTools::GetAttribute(*faces, "left", left);
						multiple_image |= JSONTools::GetAttribute(*faces, "right", right);
						multiple_image |= JSONTools::GetAttribute(*faces, "top", top);
						multiple_image |= JSONTools::GetAttribute(*faces, "bottom", bottom);
						multiple_image |= JSONTools::GetAttribute(*faces, "front", front);
						multiple_image |= JSONTools::GetAttribute(*faces, "back", back);
					}
				}

				if (single_image || multiple_image)
				{
					if (single_image)
					{
						FilePathParam single_path(single, config.config_path);
						skybox = SkyBoxTools::LoadSingleSkyBox(single_path);
					}
					else if (multiple_image)
					{
						FilePathParam left_path(left, config.config_path);
						FilePathParam right_path(right, config.config_path);
						FilePathParam top_path(top, config.config_path);
						FilePathParam bottom_path(bottom, config.config_path);
						FilePathParam front_path(front, config.config_path);
						FilePathParam back_path(back, config.config_path);
						skybox = SkyBoxTools::LoadMultipleSkyBox(left_path, right_path, top_path, bottom_path, front_path, back_path);
					}
					return GenTextureObject(&skybox, PixelFormatMergeParams(), parameters);
				}
			}
		}
		return nullptr;
	}

}; // namespace chaos
