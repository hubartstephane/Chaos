#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{


	GPUTexture * GPUTextureLoader::LoadObject(char const * name, nlohmann::json const * json, GenTextureParameters const & parameters) const
	{
		return LoadObjectHelper(name, json, [this, &parameters](nlohmann::json const * json)
		{
			return GenTextureObject(json, parameters);
		},
		[this](GPUTexture* texture)
		{
			manager->textures.push_back(texture);
		});
	}

	GPUTexture * GPUTextureLoader::LoadObject(FilePathParam const & path, char const * name, GenTextureParameters const & parameters) const
	{
		return LoadObjectHelper(path, name, [this, &parameters](FilePathParam const& path)
		{
			return GenTextureObject(path, parameters);
		},
		[this](GPUTexture* texture)
		{
			manager->textures.push_back(texture);
		});
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

		TextureType texture_type = GLTextureTools::GetTexture2DTypeFromSize(image.width, image.height, parameters.rectangle_texture);  // compute the format

		GLuint texture_id = 0;
		glCreateTextures((GLenum)texture_type, 1, &texture_id);
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
			if (texture_type == TextureType::Texture1D)
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
			texture_description.type = texture_type;
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

		Buffer<char> ascii_buffer = FileTools::LoadFile(path, LoadFileFlag::ASCII | LoadFileFlag::NO_ERROR_TRACE); // ascii mode for JSON
		if (ascii_buffer == nullptr)
		{
			GLLog::Error("GPUTextureLoader::GenTextureObject: fail to load [%s]", path.GetResolvedPath().string().c_str());
			return nullptr;
		}
		// while i am not sure an additionnal 0 in buffer wont be treated as a corruption by Free_Image
		// i work with a clamped buffer without this ascii 0 terminal
		Buffer<char> noascii_buffer;
		noascii_buffer.data = ascii_buffer.data;
		noascii_buffer.bufsize = ascii_buffer.bufsize - 1;

		if (FIBITMAP* image = ImageTools::LoadImageFromBuffer(noascii_buffer))
		{
			result = GenTextureObject(image, parameters);
			FreeImage_Unload(image);
		}
		else
		{
			nlohmann::json json;
			if (JSONTools::ParseRecursive(ascii_buffer, path.GetResolvedPath(), json))
				result = GenTextureObject(&json, parameters);
			else
				GLLog::Error("GPUTextureLoader: unknown format for [%s]", path.GetResolvedPath().string().c_str());
		}
		return result;
	}

	// There are lots of very uncleared referenced for faces orientation
	// Most of pictures found one GoogleImage do not correspond to OpenGL but DirectX
	// There are differences between OpenGL & DirectX implementation
	//
	// I found this one that seems to work almost fine with OpenGL
	//  - horizontal cubemap OK
	//  - vertical   cubemap OK
	//  - multiple   cubemap OK
	//
	// Problems : the face junctions are good, but the cubemap is inverted (top to down).
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

	int GPUTextureLoader::GetCubeMapLayerValueFromCubeMapFace(CubeMapImageType face, int level)
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

		if (face == CubeMapImageType::ImageRight)
			return 0 + 6 * level;
		if (face == CubeMapImageType::ImageLeft)
			return 1 + 6 * level;
		if (face == CubeMapImageType::ImageBottom)
			return 2 + 6 * level;
		if (face == CubeMapImageType::ImageTop)
			return 3 + 6 * level;
		if (face == CubeMapImageType::ImageFront)
			return 4 + 6 * level;
		if (face == CubeMapImageType::ImageBack)
			return 5 + 6 * level;
		return -1;
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(CubeMapImages const * cubemap, PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters) const
	{
		assert(cubemap != nullptr);

		GPUTexture * result = nullptr;

		if (cubemap->IsEmpty())
			return nullptr;

		PixelFormat final_pixel_format = cubemap->GetMergedPixelFormat(merge_params);

		int size = cubemap->GetCubeMapSize();

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
		bool is_single_image = cubemap->IsSingleImage();

		size_t required_allocation = 0;

		bool face_valid[6] = { false, false, false, false, false, false };
		bool conversion_required[6] = { false, false, false, false, false, false };
		bool central_symetry[6] = { false, false, false, false, false, false };

		for (size_t i = (int)CubeMapImageType::ImageLeft; i <= (int)CubeMapImageType::ImageBack; ++i)
		{
			// ensure the image is valid and not empty
			ImageDescription image = cubemap->GetImageFaceDescription((CubeMapImageType)i);
			if (image.data == nullptr || !image.pixel_format.IsValid())
				continue;
			face_valid[i] = true;

			// test whether a conversion/copy is required
			if ((image.pixel_format.component_count == 1) && (final_pixel_format.component_count != 1))
				conversion_required[i] = true;

			if (is_single_image)
			{
				glm::ivec3 position_and_flags = cubemap->GetPositionAndFlags((CubeMapImageType)i);
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

			// fill the faces in GPU with the images of CubeMap
			for (size_t i = int(CubeMapImageType::ImageLeft); i <= int(CubeMapImageType::ImageBack); ++i)
			{
				// ensure the image is valid and not empty
				if (!face_valid[i])
					continue;

				// do the conversion, central symetry
				ImageDescription image = cubemap->GetImageFaceDescription((CubeMapImageType)i);

				ImageDescription effective_image = (conversion_required[i]) ?
					ImageTools::ConvertPixels(image, final_pixel_format, conversion_buffer, central_symetry[i]? ImageTransform::CENTRAL_SYMETRY : ImageTransform::NO_TRANSFORM) :
					image;

				// fill glPixelStorei(...)
				// do not remove this line from the loop. Maybe future implementation will accept image with same size but different pitch

				char * texture_buffer = GLTextureTools::PrepareGLTextureTransfert(effective_image);
				if (texture_buffer != nullptr)
				{
					// fill GPU
					int depth = GetCubeMapLayerValueFromCubeMapFace((CubeMapImageType)i, 0);

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
			texture_description.type = TextureType::TextureCubeMap;
			texture_description.pixel_format = final_pixel_format;
			texture_description.width = size;
			texture_description.height = size;
			texture_description.depth = 1;

			// this is smoother to clamp at edges
			GenTextureParameters tmp = parameters;
			tmp.wrap_s = TextureWrapMethod::ClampToEdge;
			tmp.wrap_r = TextureWrapMethod::ClampToEdge;
			tmp.wrap_t = TextureWrapMethod::ClampToEdge;

			GLTextureTools::GenTextureApplyParameters(texture_id, texture_description, tmp);
			result = new GPUTexture(texture_id, texture_description);
		}

		// release the buffer
		if (conversion_buffer != nullptr)
			delete[](conversion_buffer);

		return result;
	}

	GPUTexture * GPUTextureLoader::GenTextureObject(nlohmann::json const * json, GenTextureParameters const & parameters) const
	{
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p);
			return GenTextureObject(path, parameters);
		}

		// cubemap descriptions ?
		if (nlohmann::json const* faces = JSONTools::GetAttributeStructureNode(json, "faces"))
		{
			boost::filesystem::path left_path;
			boost::filesystem::path right_path;
			boost::filesystem::path top_path;
			boost::filesystem::path bottom_path;
			boost::filesystem::path front_path;
			boost::filesystem::path back_path;

			boost::filesystem::path single_path;

			bool single_image = false;
			bool multiple_image = false;

			CubeMapImages cubemap;
			if (faces->is_array())
			{
				if (faces->size() == 1)
				{
					single_image |= JSONTools::GetElement(faces, 0, single_path);
				}
				else
				{
					multiple_image |= JSONTools::GetElement(faces, 0, left_path);
					multiple_image |= JSONTools::GetElement(faces, 1, right_path);
					multiple_image |= JSONTools::GetElement(faces, 2, top_path);
					multiple_image |= JSONTools::GetElement(faces, 3, bottom_path);
					multiple_image |= JSONTools::GetElement(faces, 4, front_path);
					multiple_image |= JSONTools::GetElement(faces, 5, back_path);
				}
			}
			else
			{
				single_image |= JSONTools::GetAttribute(faces, "single", single_path);
				if (!single_image)
				{
					multiple_image |= JSONTools::GetAttribute(faces, "left", left_path);
					multiple_image |= JSONTools::GetAttribute(faces, "right", right_path);
					multiple_image |= JSONTools::GetAttribute(faces, "top", top_path);
					multiple_image |= JSONTools::GetAttribute(faces, "bottom", bottom_path);
					multiple_image |= JSONTools::GetAttribute(faces, "front", front_path);
					multiple_image |= JSONTools::GetAttribute(faces, "back", back_path);
				}
			}

			if (single_image || multiple_image)
			{
				if (single_image)
				{
					cubemap = CubeMapTools::LoadSingleCubeMap(single_path);
				}
				else if (multiple_image)
				{
					cubemap = CubeMapTools::LoadMultipleCubeMap(left_path, right_path, top_path, bottom_path, front_path, back_path);
				}
				return GenTextureObject(&cubemap, PixelFormatMergeParams(), parameters);
			}
		}
		return nullptr;
	}

}; // namespace chaos
