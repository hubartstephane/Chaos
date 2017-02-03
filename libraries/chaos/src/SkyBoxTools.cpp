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
		glm::ivec3(0, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // left
		glm::ivec3(2, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // right
		glm::ivec3(1, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // top
		glm::ivec3(1, 0, SkyBoxImages::IMAGE_NO_TRANSFORM), // bottom
		glm::ivec3(1, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // front
		glm::ivec3(3, 1, SkyBoxImages::IMAGE_NO_TRANSFORM)  // back
	};
	SkyBoxSingleDisposition const SkyBoxSingleDisposition::VerticalDisposition = 
	{
		3, 4,
		glm::ivec3(0, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // left
		glm::ivec3(2, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // right
		glm::ivec3(1, 3, SkyBoxImages::IMAGE_NO_TRANSFORM), // top
		glm::ivec3(1, 1, SkyBoxImages::IMAGE_NO_TRANSFORM), // bottom
		glm::ivec3(1, 2, SkyBoxImages::IMAGE_NO_TRANSFORM), // front
		glm::ivec3(1, 0, SkyBoxImages::IMAGE_CENTRAL_SYMETRY)  // back  
	};

	glm::ivec3 SkyBoxSingleDisposition::GetPositionAndFlags(int image_type) const
	{
		if (image_type == SkyBoxImages::IMAGE_LEFT)
			return left_image_position;
		if (image_type == SkyBoxImages::IMAGE_RIGHT)
			return right_image_position;
		if (image_type == SkyBoxImages::IMAGE_TOP)
			return top_image_position;
		if (image_type == SkyBoxImages::IMAGE_BOTTOM)
			return bottom_image_position;
		if (image_type == SkyBoxImages::IMAGE_FRONT)
			return front_image_position;
		if (image_type == SkyBoxImages::IMAGE_BACK)
			return back_image_position;

		assert(0);
		static glm::ivec3 const wrong_result = glm::ivec3(0, 0, 0);
		return wrong_result;
	}

	SkyBoxImages::SkyBoxImages(SkyBoxImages && other)
	{
		std::swap(single_image, other.single_image);
		std::swap(left_image,   other.left_image);
		std::swap(right_image,  other.right_image);
		std::swap(top_image,    other.top_image);
		std::swap(bottom_image, other.bottom_image);
		std::swap(front_image,  other.front_image);
		std::swap(back_image,   other.back_image);
	}

	SkyBoxImages::~SkyBoxImages()
	{
		Release(true);
	}

	SkyBoxImages & SkyBoxImages::operator = (SkyBoxImages && other)
	{
		std::swap(single_image, other.single_image);
		std::swap(left_image,   other.left_image);
		std::swap(right_image,  other.right_image);
		std::swap(top_image,    other.top_image);
		std::swap(bottom_image, other.bottom_image);
		std::swap(front_image,  other.front_image);
		std::swap(back_image,   other.back_image);
		return *this;   
	}

	void SkyBoxImages::Release(bool bFreeMemory)
	{
		if (bFreeMemory)
		{
			if (single_image != nullptr)
				FreeImage_Unload(single_image);
			if (left_image != nullptr)
				FreeImage_Unload(left_image);
			if (right_image != nullptr)
				FreeImage_Unload(right_image);
			if (top_image != nullptr)
				FreeImage_Unload(top_image);
			if (bottom_image != nullptr)
				FreeImage_Unload(bottom_image);
			if (front_image != nullptr)
				FreeImage_Unload(front_image);
			if (back_image != nullptr)
				FreeImage_Unload(back_image);
		}

		single_image = nullptr;
		left_image   = nullptr;
		right_image  = nullptr;
		top_image    = nullptr;
		bottom_image = nullptr;
		front_image  = nullptr;
		back_image   = nullptr;
	}

	int SkyBoxImages::GetSkyBoxSize() const
	{
		if (single_image != nullptr)
			return GetSingleImageSize(single_image);

		if (left_image != nullptr)
			return GetMultipleImageSize(left_image);
		if (right_image != nullptr)
			return GetMultipleImageSize(right_image);
		if (top_image != nullptr)
			return GetMultipleImageSize(top_image);
		if (bottom_image != nullptr)
			return GetMultipleImageSize(bottom_image);
		if (front_image != nullptr)
			return GetMultipleImageSize(front_image);
		if (back_image != nullptr)
			return GetMultipleImageSize(back_image);
		return -1;
	}

	int SkyBoxImages::GetSkyBoxBPP() const
	{
		if (single_image != nullptr)
			return FreeImage_GetBPP(single_image);

		if (left_image != nullptr)
			return FreeImage_GetBPP(left_image);
		if (right_image != nullptr)
			return FreeImage_GetBPP(right_image);
		if (top_image != nullptr)
			return FreeImage_GetBPP(top_image);
		if (bottom_image != nullptr)
			return FreeImage_GetBPP(bottom_image);
		if (front_image != nullptr)
			return FreeImage_GetBPP(front_image);
		if (back_image != nullptr)
			return FreeImage_GetBPP(back_image);
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
		int width  = FreeImage_GetWidth(single_image);
		int height = FreeImage_GetHeight(single_image);
		return (width > height);
	}

	bool SkyBoxImages::IsSingleImageVertical() const
	{
		if (!IsSingleImage())
			return false;
		int width  = FreeImage_GetWidth(single_image);
		int height = FreeImage_GetHeight(single_image);
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
		ImageDescription src_image_desc = ImageTools::GetImageDescription(single_image);
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
			if (flag == SkyBoxImages::IMAGE_NO_TRANSFORM)
				ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size);
			else if (flag == SkyBoxImages::IMAGE_CENTRAL_SYMETRY)
				ImageTools::CopyPixelsWithCentralSymetry(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size);

			result.SetImagePtrUnchecked(i, image);
		}

		return result;
	}







	SkyBoxImages SkyBoxImages::ToSingleImage(bool bHorizontal, glm::vec4 const & fill_color, PixelFormatMergeParams const & merge_params) const
	{
		SkyBoxImages result;

		// must be multiple 
		if (IsSingleImage() || IsEmpty()) 
			return result;

		// find the final format and size
		int size = -1;

		PixelFormatMerger pixel_format_merger(merge_params);
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
		{
			FIBITMAP * face_image = GetImage(i);
			if (face_image == nullptr)
				continue;		

			ImageDescription face_image_desc = ImageTools::GetImageDescription(face_image);
		
			pixel_format_merger.Merge(face_image_desc.pixel_format);		
			if (size < 0)
				size = GetMultipleImageSize(face_image_desc);
		}

		PixelFormat final_pixel_format;
		if (size <= 0 || !pixel_format_merger.GetResult(final_pixel_format))
			return result;
		if (!final_pixel_format.IsValid())
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
#if 0
		unsigned char bgra[4];
		bgra[0] = (unsigned char)(fill_color.b * 255.0f);
		bgra[1] = (unsigned char)(fill_color.g * 255.0f);
		bgra[2] = (unsigned char)(fill_color.r * 255.0f);
		bgra[3] = (unsigned char)(fill_color.a * 255.0f);

		FreeImage_FillBackground(new_image, bgra, (bpp == 24)? FI_COLOR_IS_RGB_COLOR : FI_COLOR_IS_RGBA_COLOR);
#endif
		
		// copy the faces into the single image
		ImageDescription dst_image_desc = ImageTools::GetImageDescription(new_image);
		for (int i = IMAGE_LEFT ; i <= IMAGE_BACK ; ++i)
		{
			FIBITMAP * image = GetImage(i);
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
			if (flag == SkyBoxImages::IMAGE_NO_TRANSFORM)
				ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size);
			else if (flag == SkyBoxImages::IMAGE_CENTRAL_SYMETRY)
				ImageTools::CopyPixelsWithCentralSymetry(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size);
		}

		return result;
	}

	ImageDescription SkyBoxImages::GetImageFaceDescription(int image_type) const
	{
		assert(image_type >= IMAGE_LEFT && image_type <= IMAGE_BACK);

		if (IsSingleImage())
		{
			glm::ivec3 position_and_flags = GetPositionAndFlags(image_type);

			ImageDescription src_image_desc = ImageTools::GetImageDescription(single_image);

			int sub_image_index_x = position_and_flags.x; // number of pixels / number of images aligned
			int sub_image_index_y = position_and_flags.y;

			int size = GetSingleImageSize(single_image); // size of each face
			int x    = sub_image_index_x * size;
			int y    = sub_image_index_y * size;

			return src_image_desc.GetSubImageDescription(x, y, size, size);   
		}
		else
		{
			FIBITMAP * image = GetImage(image_type);
			if (image != nullptr)
				return ImageTools::GetImageDescription(image);
		}
		return ImageDescription();
	}

	bool SkyBoxImages::IsEmpty() const // Empty = no images
	{
		if (single_image != nullptr)
			return false;
		if (left_image != nullptr)
			return false;
		if (right_image != nullptr)
			return false;
		if (top_image != nullptr)
			return false;
		if (bottom_image != nullptr)
			return false;
		if (front_image != nullptr)
			return false;
		if (back_image != nullptr)
			return false;
		return true;  
	}

	bool SkyBoxImages::IsSingleImage() const
	{
		return (single_image != nullptr);
	}

	bool SkyBoxImages::IsMultipleImage() const // no single image, at least one multiple image
	{
		if (single_image != nullptr)
			return false;
		if (left_image != nullptr)
			return true;
		if (right_image != nullptr)
			return true;
		if (top_image != nullptr)
			return true;
		if (bottom_image != nullptr)
			return true;
		if (front_image != nullptr)
			return true;
		if (back_image != nullptr)
			return true;
		return false;  
	}

	bool SkyBoxImages::IsMultipleImageComplete() const // no single image, every face defined
	{
		if (single_image != nullptr)
			return false;
		if (left_image == nullptr)
			return false;
		if (right_image == nullptr)
			return false;
		if (top_image == nullptr)
			return false;
		if (bottom_image == nullptr)
			return false;
		if (front_image == nullptr)
			return false;
		if (back_image == nullptr)
			return false;
		return true;
	}

	FIBITMAP * SkyBoxImages::GetImage(int image_type) const
	{
		if (image_type == IMAGE_SINGLE)
			return single_image;
		if (image_type == IMAGE_LEFT)
			return left_image;
		if (image_type == IMAGE_RIGHT)
			return right_image;
		if (image_type == IMAGE_TOP)
			return top_image;
		if (image_type == IMAGE_BOTTOM)
			return bottom_image;
		if (image_type == IMAGE_FRONT)
			return front_image;
		if (image_type == IMAGE_BACK)
			return back_image;
		return nullptr;
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

	void SkyBoxImages::SetImagePtrUnchecked(int image_type, FIBITMAP * image)
	{
		if (image_type == IMAGE_SINGLE)
			single_image = image;
		else if (image_type == IMAGE_LEFT)
			left_image = image;
		else if (image_type == IMAGE_RIGHT)
			right_image = image;
		else if (image_type == IMAGE_TOP)
			top_image = image;
		else if (image_type == IMAGE_BOTTOM)
			bottom_image = image;
		else if (image_type == IMAGE_FRONT)
			front_image = image;
		else if (image_type == IMAGE_BACK)
			back_image = image;
	}

	bool SkyBoxImages::SetImage(int image_type, FIBITMAP * image, bool bReleasePrevious)
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
				if (single_image != nullptr && bReleasePrevious)
					FreeImage_Unload(single_image);
				single_image = image;
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
						other_image = GetImage(i);

				// if there is other images in the skybox, ensure for compatibility
				if (other_image != nullptr && !AreImageCompatible(image, other_image))
					return false;

				// release previous image
				if (bReleasePrevious)
				{
					FIBITMAP * previous_image = GetImage(image_type); 
					if (previous_image != nullptr)
						FreeImage_Unload(previous_image);
				}

				// do the writing
				SetImagePtrUnchecked(image_type, image);

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
			result.Release(true); // release whole object in case of error
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

