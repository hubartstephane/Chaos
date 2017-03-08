#include <chaos/SkyBoxTools.h>
#include <chaos/ImageTools.h>

namespace chaos
{

	// XXX : for FreeImage, scanlines are stored bottom to top
	//      
	// - ...
	// - scanline 2 (offset = 2 x pitch)
	// - scanline 1 (offset = 1 x pitch)
	// - scanline 0 (offset = 0)
	//
	// Nevertheless the coordinate system is from up to down
	//
	//  0,0
	//   +------->  
	//   |
	//   |               top should be lower than bottom
	//   v
	//
	// XXX : for FreeImage, pixels are stored has BGR or BGRA (not RGB / RGBA)
	//


	SkyBoxSingleDisposition const SkyBoxSingleDisposition::HorizontalDisposition = 
	{
		4, 3,
		{
			glm::ivec3(0, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // left
			glm::ivec3(2, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // right
			glm::ivec3(1, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // top
			glm::ivec3(1, 0, SkyBoxImages::IMAGE_NO_TRANSFORM), // bottom
			glm::ivec3(1, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // front
			glm::ivec3(3, 1, SkyBoxImages::IMAGE_NO_TRANSFORM)  // back	
		}
	};
	SkyBoxSingleDisposition const SkyBoxSingleDisposition::VerticalDisposition = 
	{
		3, 4,
		{
			glm::ivec3(0, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // left
			glm::ivec3(2, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // right
			glm::ivec3(1, 3, SkyBoxImages::IMAGE_NO_TRANSFORM), // top
			glm::ivec3(1, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // bottom
			glm::ivec3(1, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // front
			glm::ivec3(1, 0, SkyBoxImages::IMAGE_CENTRAL_SYMETRY)  // back  	
		}
	};

	glm::ivec3 SkyBoxSingleDisposition::GetPositionAndFlags(int image_type) const
	{
		assert(image_type >= 0);
		assert(image_type <= 5);
		return image_position[image_type];
	}

	SkyBoxImages::SkyBoxImages(SkyBoxImages && other)
	{
		for (int i = IMAGE_FIRST_INDEX ; i <= IMAGE_LAST_INDEX ; ++i)
		{
			std::swap(images[i], other.images[i]);
			std::swap(release_images[i], other.release_images[i]);
		}
	}

	SkyBoxImages::~SkyBoxImages()
	{
		Release();
	}

	SkyBoxImages & SkyBoxImages::operator = (SkyBoxImages && other)
	{
		for (int i = IMAGE_FIRST_INDEX ; i <= IMAGE_LAST_INDEX ; ++i)
		{
			std::swap(images[i], other.images[i]);
			std::swap(release_images[i], other.release_images[i]);
		}
		return *this;   
	}

	void SkyBoxImages::Release()
	{
		for (int i = IMAGE_FIRST_INDEX ; i <= IMAGE_LAST_INDEX ; ++i)
		{
			if (images[i] != nullptr)
			{
				if (release_images[i])
					FreeImage_Unload(images[i]);
				images[i] = nullptr;		
			}
		}
	}

	int SkyBoxImages::GetSkyBoxSize() const
	{
		if (images[IMAGE_SINGLE] != nullptr)
			return GetSingleImageSize(images[IMAGE_SINGLE]);
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
			if (images[i] != nullptr)
				return GetMultipleImageSize(images[i]);
		return -1;
	}

	int SkyBoxImages::GetSingleImageSize(ImageDescription const & image_description)
	{
		int width  = image_description.width;
		int height = image_description.height;
		if (width > 0 && height > 0)
		{
			// image should be 4/3
			int min_size = min(width, height);    
			if (min_size % 3 != 0)
				return -1;
			int max_size = max(width, height);
			if (max_size % 4 != 0)
				return -1;
			// each sub image should be square
			min_size /= 3;
			max_size /= 4;
			if (min_size == max_size)
				return min_size;
		}
		return -1;
	}
	
	int SkyBoxImages::GetSingleImageSize(FIBITMAP * image)
	{
		assert(image != nullptr);
		return GetSingleImageSize(ImageTools::GetImageDescription(image));
	}

	bool SkyBoxImages::IsSingleImageHorizontal() const
	{
		if (!IsSingleImage())
			return false;
		int width  = FreeImage_GetWidth(images[IMAGE_SINGLE]);
		int height = FreeImage_GetHeight(images[IMAGE_SINGLE]);
		return (width > height);
	}

	bool SkyBoxImages::IsSingleImageVertical() const
	{
		if (!IsSingleImage())
			return false;
		int width  = FreeImage_GetWidth(images[IMAGE_SINGLE]);
		int height = FreeImage_GetHeight(images[IMAGE_SINGLE]);
		return (height > width);
	}

	int SkyBoxImages::GetMultipleImageSize(ImageDescription const & image_description)
	{
		int width  = image_description.width;
		int height = image_description.height;
		if (width == height && width > 0)
			return width;
		return -1;	
	}

	int SkyBoxImages::GetMultipleImageSize(FIBITMAP * image)
	{
		assert(image != nullptr);
		return GetMultipleImageSize(ImageTools::GetImageDescription(image));
	}

	glm::ivec3 SkyBoxImages::GetPositionAndFlags(int image_type) const
	{
		assert(IsSingleImage());

		SkyBoxSingleDisposition const & dispo = (IsSingleImageHorizontal())? 
			SkyBoxSingleDisposition::HorizontalDisposition : 
			SkyBoxSingleDisposition::VerticalDisposition;
		return dispo.GetPositionAndFlags(image_type);
	}

	SkyBoxImages SkyBoxImages::ToMultipleImages() const
	{
		SkyBoxImages result;

		// must be single (so it is non empty)
		if (!IsSingleImage()) 
			return result;

		// get the single image description
		ImageDescription src_image_desc = ImageTools::GetImageDescription(images[IMAGE_SINGLE]);
		if (!src_image_desc.IsValid())
			return result;
		
		// the wanted size for every face
		int size = GetSingleImageSize(src_image_desc); 

		// iterate over all faces
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
		{
			glm::ivec3 position_and_flags = GetPositionAndFlags(i);

			int left   = position_and_flags.x * size; // number of pixels / number of images aligned
			int bottom = position_and_flags.y * size;

			// allocate a face => in case of error, forget about result and returns an empty object
			FIBITMAP * image = ImageTools::GenFreeImage(src_image_desc.pixel_format, size, size);
			if (image == nullptr)
				return SkyBoxImages();

			// copy pixels
			ImageDescription dst_image_desc = ImageTools::GetImageDescription(image);

			int src_x = left;
			int src_y = bottom;
			int dst_x = 0;
			int dst_y = 0;

			int flag = position_and_flags.z;
			ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size, (flag == SkyBoxImages::IMAGE_CENTRAL_SYMETRY));
				
			result.images[i] = image;
			result.release_images[i] = true;
		}

		return result;
	}

	SkyBoxImages SkyBoxImages::ToSingleImage(bool bHorizontal, glm::vec4 const & fill_color, PixelFormatMergeParams const & merge_params) const
	{
		SkyBoxImages result;

		// must be multiple 
		if (IsSingleImage() || IsEmpty()) 
			return result;

		// find the final format
		PixelFormat final_pixel_format = GetMergedPixelFormat(merge_params);
		if (!final_pixel_format.IsValid())
			return result;

		// find the final format and size
		int size = -1;
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
		{
			FIBITMAP * face_image = images[i];
			if (face_image == nullptr)
				continue;		

			ImageDescription face_image_desc = ImageTools::GetImageDescription(face_image);
			if (size < 0)
				size = GetMultipleImageSize(face_image_desc);
		}

		if (size <= 0)
			return result;

		// get the disposition
		SkyBoxSingleDisposition const & dispo = bHorizontal? 
			SkyBoxSingleDisposition::HorizontalDisposition : 
			SkyBoxSingleDisposition::VerticalDisposition;
			
		// allocate the image
		int new_image_width  = size * dispo.image_count_horiz;
		int new_image_height = size * dispo.image_count_vert;

		FIBITMAP * new_image = ImageTools::GenFreeImage(final_pixel_format, new_image_width, new_image_height);
		if (new_image == nullptr)
			return result;
		result.SetImage(IMAGE_SINGLE, new_image, true);
	
		// fill the background (Blue - Green - Red - Alpha)
    ImageDescription dst_image_desc = ImageTools::GetImageDescription(new_image);

		ImageTools::FillImageBackground(dst_image_desc, fill_color);
		
		// copy the faces into the single image
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
		{
			FIBITMAP * image = images[i];
			if (image == nullptr)
				continue;

			ImageDescription src_image_desc = ImageTools::GetImageDescription(image);

			glm::ivec3 position_and_flags = dispo.GetPositionAndFlags(i);

			int sub_image_index_x = position_and_flags.x; // number of pixels / number of images aligned
			int sub_image_index_y = position_and_flags.y;

			int left   = sub_image_index_x * size;
			int bottom = sub_image_index_y * size;

			int src_x = 0;    // src = multiple => corner = origin
			int src_y = 0;
			int dst_x = left;
			int dst_y = bottom;

			int flag = position_and_flags.z;
			ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size, (flag == SkyBoxImages::IMAGE_CENTRAL_SYMETRY));
		}

		return result;
	}

	PixelFormat SkyBoxImages::GetMergedPixelFormat(PixelFormatMergeParams const & merge_params) const
	{
		PixelFormatMerger pixel_format_merger(merge_params);

		if (!IsEmpty())
		{
			if (IsSingleImage())
				pixel_format_merger.Merge(ImageTools::GetPixelFormat(images[IMAGE_SINGLE]));
			else
				for (int i = IMAGE_LEFT; i <= IMAGE_BACK; ++i)
					if (images[i] != nullptr)
						pixel_format_merger.Merge(ImageTools::GetPixelFormat(images[i]));
		}
		return pixel_format_merger.GetResult();
	}
	
	ImageDescription SkyBoxImages::GetImageFaceDescription(int image_type) const
	{
		assert(image_type >= IMAGE_LEFT && image_type <= IMAGE_BACK);

		if (IsSingleImage())
		{
			glm::ivec3 position_and_flags = GetPositionAndFlags(image_type);

			ImageDescription src_image_desc = ImageTools::GetImageDescription(images[IMAGE_SINGLE]);

			int sub_image_index_x = position_and_flags.x; // number of pixels / number of images aligned
			int sub_image_index_y = position_and_flags.y;

			int size = GetSingleImageSize(images[IMAGE_SINGLE]); // size of each face
			int x    = sub_image_index_x * size;
			int y    = sub_image_index_y * size;

			return src_image_desc.GetSubImageDescription(x, y, size, size);   
		}
		else
		{
			FIBITMAP * image = images[image_type];
			if (image != nullptr)
				return ImageTools::GetImageDescription(image);
		}
		return ImageDescription();
	}

	bool SkyBoxImages::IsEmpty() const // Empty = no images
	{
		for (int i = IMAGE_FIRST_INDEX ; i <= IMAGE_LAST_INDEX ; ++i)
			if (images[i] != nullptr)
				return false;
		return true;  
	}

	bool SkyBoxImages::IsSingleImage() const
	{
		return (images[IMAGE_SINGLE] != nullptr);
	}

	bool SkyBoxImages::IsMultipleImage() const // no single image, at least one multiple image
	{
		if (images[IMAGE_SINGLE] != nullptr)
			return false;
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
			if (images[i] != nullptr)			
				return true;
		return false;  
	}

	bool SkyBoxImages::IsMultipleImageComplete() const // no single image, every face defined
	{
		if (images[IMAGE_SINGLE] != nullptr)
			return false;
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
			if (images[i] == nullptr)			
				return false;
		return true;  
	}

	bool SkyBoxImages::AreImageCompatible(FIBITMAP * image1, FIBITMAP * image2)
	{
		assert(image1 != nullptr);
		assert(image2 != nullptr);
		if (FreeImage_GetWidth(image1) != FreeImage_GetWidth(image2))
			return false;
		if (FreeImage_GetHeight(image1) != FreeImage_GetHeight(image2))
			return false;
		return true;
	}

	FIBITMAP * SkyBoxImages::GetImage(int face) const
	{
		assert(face >= IMAGE_FIRST_INDEX);
		assert(face <= IMAGE_LAST_INDEX);
		return images[face];
	}

	bool SkyBoxImages::SetImage(int image_type, FIBITMAP * image, bool release_image)
	{
		assert(image != nullptr);
		assert(image_type >= IMAGE_FIRST_INDEX && image_type <= IMAGE_LAST_INDEX); 

		// test whether the pixel format is supported
		ImageDescription description = ImageTools::GetImageDescription(image);
		if (!description.IsValid())
			return false;

		if (image_type == IMAGE_SINGLE)
		{
			// ensure image is 3/4 or 4/3
			int size = GetSingleImageSize(description);
			if (size <= 0)
				return false;
			// if multiple face skybox, we cannot set the image
			if (IsEmpty() || IsSingleImage()) 
			{
				if (images[IMAGE_SINGLE] != nullptr && release_images[IMAGE_SINGLE])
					FreeImage_Unload(images[IMAGE_SINGLE]);
				images[IMAGE_SINGLE] = image;
				release_images[IMAGE_SINGLE] = release_image;
				return true;
			}
		}
		else
		{
			// ensure image size is square
			int size = GetMultipleImageSize(description);
			if (size <= 0)
				return false;

			if (!IsSingleImage()) // cannot set a face if object is single face
			{
				// get another image for comparison (one that will not be erased by this call)
				FIBITMAP * other_image = nullptr;
				for (int i = IMAGE_LEFT ; (i <= IMAGE_BACK) && (other_image == nullptr) ; ++i)
					if (i != image_type)
						other_image = images[i];

				// if there is other images in the skybox, ensure for compatibility
				if (other_image != nullptr && !AreImageCompatible(image, other_image))
					return false;

				// release previous image
				FIBITMAP * previous_image = images[image_type];
				if (previous_image != nullptr && release_images[image_type])
						FreeImage_Unload(previous_image);

				// do the writing
				images[image_type] = image;
				release_images[image_type] = release_image;
				return true;
			}
		}
		return false;
	}

	SkyBoxImages SkyBoxTools::LoadSingleSkyBox(char const * filename)
	{  
		SkyBoxImages result;
		DoLoadMultipleSkyBox_OneImage(result, filename, SkyBoxImages::IMAGE_SINGLE);
		return result;
	}

	SkyBoxImages SkyBoxTools::LoadMultipleSkyBox(char const * left_image, char const * right_image, char const * top_image, char const * bottom_image, char const * front_image, char const * back_image)
	{
		SkyBoxImages result;
		if (
			!DoLoadMultipleSkyBox_OneImage(result, left_image,   SkyBoxImages::IMAGE_LEFT)   ||
			!DoLoadMultipleSkyBox_OneImage(result, right_image,  SkyBoxImages::IMAGE_RIGHT)  ||
			!DoLoadMultipleSkyBox_OneImage(result, top_image,    SkyBoxImages::IMAGE_TOP)    ||
			!DoLoadMultipleSkyBox_OneImage(result, bottom_image, SkyBoxImages::IMAGE_BOTTOM) ||
			!DoLoadMultipleSkyBox_OneImage(result, front_image,  SkyBoxImages::IMAGE_FRONT)  ||
			!DoLoadMultipleSkyBox_OneImage(result, back_image,   SkyBoxImages::IMAGE_BACK)
			)
			result.Release(); // release whole object in case of error
		return result;
	}

	bool SkyBoxTools::DoLoadMultipleSkyBox_OneImage(SkyBoxImages & skybox, char const * filename, int image_index)
	{
		if (filename != nullptr) 
		{
			FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
			if (image == nullptr)
				return false;

			if (!skybox.SetImage(image_index, image, false))
			{
				FreeImage_Unload(image);
				return false;
			}
		}
		return true;
	}
};

