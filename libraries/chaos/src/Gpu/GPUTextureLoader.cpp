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
		if (!image.IsValid(true) || image.IsEmpty(true))
			return nullptr;

		TextureType texture_type = GLTextureTools::GetTexture2DTypeFromSize(image.width, image.height);

		TextureDescription texture_description;
		texture_description.type         = texture_type;
		texture_description.pixel_format = image.pixel_format;
		texture_description.width        = image.width;
		texture_description.height       = image.height;
		texture_description.depth        = 1;
		texture_description.use_mipmaps  = parameters.reserve_mipmaps;

		GPUTexture * result = GetGPUDevice()->CreateTexture(texture_description);
		if (result == nullptr)
			return nullptr;

		result->SetSubImage(image);

		result->SetMinificationFilter(parameters.min_filter);
		result->SetMagnificationFilter(parameters.mag_filter);
		result->SetWrapMethods(parameters.wrap_methods);

		if (parameters.build_mipmaps && parameters.reserve_mipmaps)
			result->GenerateMipmaps();

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


	GPUTexture * GPUTextureLoader::GenTextureObject(CubeMapImages const * cubemap, PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters) const
	{
		assert(cubemap != nullptr);

		if (cubemap->IsEmpty())
			return nullptr;

		int cubemap_size = cubemap->GetCubeMapSize();
		if (cubemap_size < 0)
			return nullptr;

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

		struct SubImageFaceInfo
		{
			bool valid = false;
			bool conversion_required = false;
			bool central_symetry = false;
		};

		std::array<SubImageFaceInfo, 6> subimage_face_info;

		PixelFormat pixel_format = cubemap->GetMergedPixelFormat(merge_params);

		for (size_t i = (int)CubeMapImageType::ImageLeft; i <= (int)CubeMapImageType::ImageBack; ++i)
		{
			// ensure the image is valid and not empty
			ImageDescription image = cubemap->GetImageFaceDescription((CubeMapImageType)i);
			if (image.data == nullptr || !image.pixel_format.IsValid())
				continue;
			subimage_face_info[i].valid = true;

			// test whether a conversion/copy is required
			if ((image.pixel_format.component_count == 1) && (pixel_format.component_count != 1))
				subimage_face_info[i].conversion_required = true;

			if (is_single_image)
			{
				CubeMapSingleImageLayoutFaceInfo face_info = cubemap->GetSingleImageLayoutFaceInfo((CubeMapImageType)i);
				if (face_info.transform == ImageTransform::CENTRAL_SYMETRY)
					subimage_face_info[i].central_symetry = subimage_face_info[i].conversion_required = true;
			}
			// compute memory required
			if (subimage_face_info[i].conversion_required)
				required_allocation = std::max(required_allocation, (size_t)ImageTools::GetMemoryRequirementForAlignedTexture(pixel_format, cubemap_size, cubemap_size));
		}

		// allocate the buffer
		char * conversion_buffer = nullptr;
		if (required_allocation != 0)
		{
			conversion_buffer = new char[required_allocation];
			if (conversion_buffer == nullptr)
				return nullptr;
		}

		TextureDescription texture_description;
		texture_description.type = TextureType::TextureCubeMap;
		texture_description.pixel_format = pixel_format;
		texture_description.width = cubemap_size;
		texture_description.height = cubemap_size;
		texture_description.depth = 1;
		texture_description.use_mipmaps = parameters.reserve_mipmaps;

		// create the texture
		GPUTexture* result = GetGPUDevice()->CreateTexture(texture_description);
		if (result == nullptr)
		{
			if (conversion_buffer != nullptr)
				delete[](conversion_buffer);
			return nullptr;
		}

		// fill the faces in GPU with the images of CubeMap
		for (size_t i = int(CubeMapImageType::ImageLeft); i <= int(CubeMapImageType::ImageBack); ++i)
		{
			// ensure the image is valid and not empty
			if (!subimage_face_info[i].valid)
				continue;

			// do the conversion, central symetry
			ImageDescription image = cubemap->GetImageFaceDescription((CubeMapImageType)i);

			ImageDescription effective_image = (subimage_face_info[i].conversion_required) ?
				ImageTools::ConvertPixels(image, pixel_format, conversion_buffer, subimage_face_info[i].central_symetry ? ImageTransform::CENTRAL_SYMETRY : ImageTransform::NO_TRANSFORM) :
				image;

			CubeMapFaceType face_type;
			switch(CubeMapImageType(i))
			{
				case CubeMapImageType::ImageLeft:
					face_type = CubeMapFaceType::NegativeX; break;
				case CubeMapImageType::ImageRight:
					face_type = CubeMapFaceType::PositiveX; break;
				case CubeMapImageType::ImageTop:
					face_type = CubeMapFaceType::NegativeY; break;
				case CubeMapImageType::ImageBottom:
					face_type = CubeMapFaceType::PositiveY; break;
				case CubeMapImageType::ImageFront:
					face_type = CubeMapFaceType::PositiveZ; break;
				case CubeMapImageType::ImageBack:
					face_type = CubeMapFaceType::NegativeZ; break;
			}

			result->SetSubImageCubeMap(effective_image, face_type);
		}

		// this is smoother to clamp at edges
		TextureWrapMethods smoother_wrap_methods = parameters.wrap_methods;
		smoother_wrap_methods.wrap_x = TextureWrapMethod::ClampToEdge;
		smoother_wrap_methods.wrap_y = TextureWrapMethod::ClampToEdge;
		smoother_wrap_methods.wrap_z = TextureWrapMethod::ClampToEdge;

		result->SetMinificationFilter(parameters.min_filter);
		result->SetMagnificationFilter(parameters.mag_filter);
		result->SetWrapMethods(smoother_wrap_methods);

		if (parameters.build_mipmaps && parameters.reserve_mipmaps)
			result->GenerateMipmaps();

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
