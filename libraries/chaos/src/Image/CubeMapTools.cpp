#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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

	CubeMapSingleImageLayout const * CubeMapSingleImageLayout::GetLayout(CubeMapSingleImageLayoutType layout_type)
	{
		static const CubeMapSingleImageLayout horizontal_layout =
		{
			4, 3,
			{
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageLeft,   glm::ivec2(0, 1), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageRight,  glm::ivec2(2, 1), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageTop,    glm::ivec2(1, 2), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageBottom, glm::ivec2(1, 0), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageFront,  glm::ivec2(1, 1), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageBack,   glm::ivec2(3, 1), ImageTransform::NO_TRANSFORM)
			}
		};
		static const CubeMapSingleImageLayout vertical_layout =
		{
			3, 4,
			{
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageLeft,   glm::ivec2(0, 2), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageRight,  glm::ivec2(2, 2), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageTop,    glm::ivec2(1, 3), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageBottom, glm::ivec2(1, 1), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageFront,  glm::ivec2(1, 2), ImageTransform::NO_TRANSFORM),
				CubeMapSingleImageLayoutFaceInfo(CubeMapImageType::ImageBack,   glm::ivec2(1, 0), ImageTransform::CENTRAL_SYMETRY)
			}
		};

		switch (layout_type)
		{
			case CubeMapSingleImageLayoutType::Horizontal:
				return &horizontal_layout;
			case CubeMapSingleImageLayoutType::Vertical:
				return &vertical_layout;
			default:
				assert(0);
		}

		return nullptr;
	}

	CubeMapSingleImageLayoutFaceInfo CubeMapSingleImageLayout::GetFaceInfo(CubeMapImageType image_type) const
	{
		assert((size_t)image_type >= (size_t)CubeMapImageType::ImageLeft);
		assert((size_t)image_type <= (size_t)CubeMapImageType::ImageBack);

		for (CubeMapSingleImageLayoutFaceInfo const & info : face_info)
			if (info.face == image_type)
				return info;
		assert(0);
		return {}; // should never happens
	}

	CubeMapImages::CubeMapImages(CubeMapImages && other) noexcept
	{
		for (size_t i = 0 ; i < images.size() ; ++i)
			std::swap(images[i], other.images[i]);
		for (size_t i = 0; i < release_images.size(); ++i)
			std::swap(release_images[i], other.release_images[i]);
	}

	CubeMapImages::~CubeMapImages()
	{
		Release();
	}

	CubeMapImages & CubeMapImages::operator = (CubeMapImages && other) noexcept
	{
		for (size_t i = 0; i < images.size(); ++i)
			std::swap(images[i], other.images[i]);
		for (size_t i = 0; i < release_images.size(); ++i)
			std::swap(release_images[i], other.release_images[i]);
		return *this;
	}

	void CubeMapImages::Release()
	{
		for (size_t i = 0; i < images.size(); ++i)
		{
			if (images[i] != nullptr)
			{
				if (release_images[i])
					FreeImage_Unload(images[i]);
				images[i] = nullptr;
			}
		}
	}

	int CubeMapImages::GetCubeMapSize() const
	{
		if (images[(size_t)CubeMapImageType::ImageSingle] != nullptr)
			return GetSingleImageSize(images[(size_t)CubeMapImageType::ImageSingle]);
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
			if (images[i] != nullptr)
				return GetMultipleImageSize(images[i]);
		return -1;
	}

	int CubeMapImages::GetSingleImageSize(ImageDescription const & image_description)
	{
		int width  = image_description.width;
		int height = image_description.height;
		if (width > 0 && height > 0)
		{
			// image should be 4/3
			int min_size = std::min(width, height);
			if (min_size % 3 != 0)
				return -1;
			int max_size = std::max(width, height);
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

	int CubeMapImages::GetSingleImageSize(FIBITMAP * image)
	{
		assert(image != nullptr);
		return GetSingleImageSize(ImageTools::GetImageDescription(image));
	}

	bool CubeMapImages::IsSingleImageHorizontal() const
	{
		if (!IsSingleImage())
			return false;
		int width  = FreeImage_GetWidth(images[(size_t)CubeMapImageType::ImageSingle]);
		int height = FreeImage_GetHeight(images[(size_t)CubeMapImageType::ImageSingle]);
		return (width > height);
	}

	bool CubeMapImages::IsSingleImageVertical() const
	{
		if (!IsSingleImage())
			return false;
		int width  = FreeImage_GetWidth(images[(size_t)CubeMapImageType::ImageSingle]);
		int height = FreeImage_GetHeight(images[(size_t)CubeMapImageType::ImageSingle]);
		return (height > width);
	}

	int CubeMapImages::GetMultipleImageSize(ImageDescription const & image_description)
	{
		int width  = image_description.width;
		int height = image_description.height;
		if (width == height && width > 0)
			return width;
		return -1;
	}

	int CubeMapImages::GetMultipleImageSize(FIBITMAP * image)
	{
		assert(image != nullptr);
		return GetMultipleImageSize(ImageTools::GetImageDescription(image));
	}

	CubeMapSingleImageLayoutFaceInfo CubeMapImages::GetSingleImageLayoutFaceInfo(CubeMapImageType image_type) const
	{
		assert(IsSingleImage());

		CubeMapSingleImageLayout const * layout = nullptr;
		if (IsSingleImageHorizontal())
			layout = CubeMapSingleImageLayout::GetLayout(CubeMapSingleImageLayoutType::Horizontal);
		else if (IsSingleImageVertical())
			layout = CubeMapSingleImageLayout::GetLayout(CubeMapSingleImageLayoutType::Vertical);

		return layout->GetFaceInfo(image_type);
	}

	CubeMapImages CubeMapImages::ToMultipleImages() const
	{
		CubeMapImages result;

		// must be single (so it is non empty)
		if (!IsSingleImage())
			return result;

		// get the single image description
		ImageDescription src_image_desc = ImageTools::GetImageDescription(images[(size_t)CubeMapImageType::ImageSingle]);
		if (!src_image_desc.IsValid(false))
			return result;

		// the wanted size for every face
		int size = GetSingleImageSize(src_image_desc);

		// iterate over all faces
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
		{
			CubeMapSingleImageLayoutFaceInfo face_info = GetSingleImageLayoutFaceInfo((CubeMapImageType)i);

			int left   = face_info.position.x * size; // number of pixels / number of images aligned
			int bottom = face_info.position.y * size;

			// allocate a face => in case of error, forget about result and returns an empty object
			FIBITMAP * image = ImageTools::GenFreeImage(src_image_desc.pixel_format, size, size);
			if (image == nullptr)
				return CubeMapImages();

			// copy pixels
			ImageDescription dst_image_desc = ImageTools::GetImageDescription(image);

			int src_x = left;
			int src_y = bottom;
			int dst_x = 0;
			int dst_y = 0;

			ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size, face_info.transform);

			result.images[i] = image;
			result.release_images[i] = true;
		}

		return result;
	}

	CubeMapImages CubeMapImages::ToSingleImage(CubeMapSingleImageLayoutType layout_type, glm::vec4 const & fill_color, PixelFormatMergeParams const & merge_params) const
	{
		CubeMapImages result;

		// must be multiple
		if (IsSingleImage() || IsEmpty())
			return result;

		// get the layout
		CubeMapSingleImageLayout const* layout = CubeMapSingleImageLayout::GetLayout(layout_type);
		if (layout == nullptr)
			return result;

		// find the final format
		PixelFormat pixel_format = GetMergedPixelFormat(merge_params);
		if (!pixel_format.IsValid())
			return result;

		// find the final format and size
		int size = -1;
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
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

		// allocate the image
		int new_image_width  = size * layout->horizontal_image_count;
		int new_image_height = size * layout->vertical_image_count;

		FIBITMAP * new_image = ImageTools::GenFreeImage(pixel_format, new_image_width, new_image_height);
		if (new_image == nullptr)
			return result;
		result.SetImage(CubeMapImageType::ImageSingle, new_image, true);

		// fill the background (Blue - Green - Red - Alpha)
		ImageDescription dst_image_desc = ImageTools::GetImageDescription(new_image);

		ImageTools::FillImageBackground(dst_image_desc, fill_color);

		// copy the faces into the single image
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
		{
			FIBITMAP * image = images[i];
			if (image == nullptr)
				continue;

			ImageDescription src_image_desc = ImageTools::GetImageDescription(image);

			CubeMapSingleImageLayoutFaceInfo face_info = layout->GetFaceInfo((CubeMapImageType)i);

			int sub_image_index_x = face_info.position.x; // number of pixels / number of images aligned
			int sub_image_index_y = face_info.position.y;

			int left   = sub_image_index_x * size;
			int bottom = sub_image_index_y * size;

			int src_x = 0;    // src = multiple => corner = origin
			int src_y = 0;
			int dst_x = left;
			int dst_y = bottom;

			ImageTools::CopyPixels(src_image_desc, dst_image_desc, src_x, src_y, dst_x, dst_y, size, size, face_info.transform);
		}

		return result;
	}

	PixelFormat CubeMapImages::GetMergedPixelFormat(PixelFormatMergeParams const & merge_params) const
	{
		PixelFormatMerger pixel_format_merger(merge_params);

		if (!IsEmpty())
		{
			if (IsSingleImage())
				pixel_format_merger.Merge(ImageTools::GetPixelFormat(images[(size_t)CubeMapImageType::ImageSingle]));
			else
				for (size_t i = (size_t)CubeMapImageType::ImageLeft; i <= (size_t)CubeMapImageType::ImageBack; ++i)
					if (images[i] != nullptr)
						pixel_format_merger.Merge(ImageTools::GetPixelFormat(images[i]));
		}
		return pixel_format_merger.GetResult();
	}

	ImageDescription CubeMapImages::GetImageFaceDescription(CubeMapImageType image_type) const
	{
		assert((size_t)image_type >= (size_t)CubeMapImageType::ImageLeft);
		assert((size_t)image_type <= (size_t)CubeMapImageType::ImageBack);

		if (IsSingleImage())
		{
			CubeMapSingleImageLayoutFaceInfo face_info = GetSingleImageLayoutFaceInfo(image_type);

			ImageDescription src_image_desc = ImageTools::GetImageDescription(images[(size_t)CubeMapImageType::ImageSingle]);

			int sub_image_index_x = face_info.position.x; // number of pixels / number of images aligned
			int sub_image_index_y = face_info.position.y;

			int size = GetSingleImageSize(images[(size_t)CubeMapImageType::ImageSingle]); // size of each face
			int x    = sub_image_index_x * size;
			int y    = sub_image_index_y * size;

			return src_image_desc.GetSubImageDescription(x, y, size, size);
		}
		else
		{
			FIBITMAP * image = images[(size_t)image_type];
			if (image != nullptr)
				return ImageTools::GetImageDescription(image);
		}
		return ImageDescription();
	}

	bool CubeMapImages::IsEmpty() const // Empty = no images
	{
		for (size_t i = 0 ; i < images.size() ; ++i)
			if (images[i] != nullptr)
				return false;
		return true;
	}

	bool CubeMapImages::IsSingleImage() const
	{
		return (images[(size_t)CubeMapImageType::ImageSingle] != nullptr);
	}

	bool CubeMapImages::IsMultipleImage() const // no single image, at least one multiple image
	{
		if (images[(size_t)CubeMapImageType::ImageSingle] != nullptr)
			return false;
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
			if (images[i] != nullptr)
				return true;
		return false;
	}

	bool CubeMapImages::IsMultipleImageComplete() const // no single image, every face defined
	{
		if (images[(size_t)CubeMapImageType::ImageSingle] != nullptr)
			return false;
		for (size_t i = (size_t)CubeMapImageType::ImageLeft ; i <= (size_t)CubeMapImageType::ImageBack ; ++i)
			if (images[i] == nullptr)
				return false;
		return true;
	}

	bool CubeMapImages::AreImageCompatible(FIBITMAP * image1, FIBITMAP * image2)
	{
		assert(image1 != nullptr);
		assert(image2 != nullptr);
		if (FreeImage_GetWidth(image1) != FreeImage_GetWidth(image2))
			return false;
		if (FreeImage_GetHeight(image1) != FreeImage_GetHeight(image2))
			return false;
		return true;
	}

	FIBITMAP * CubeMapImages::GetImage(CubeMapImageType face) const
	{
		assert((size_t)face < images.size());
		return images[(size_t)face];
	}

	bool CubeMapImages::SetImage(CubeMapImageType image_type, FIBITMAP * image, bool release_image)
	{
		assert(image != nullptr);
		assert((size_t)image_type >= (size_t)CubeMapImageType::ImageLeft);
		assert((size_t)image_type <= (size_t)CubeMapImageType::ImageSingle);

		// test whether the pixel format is supported
		ImageDescription description = ImageTools::GetImageDescription(image);
		if (!description.IsValid(false))
			return false;

		if (image_type == CubeMapImageType::ImageSingle)
		{
			// ensure image is 3/4 or 4/3
			int size = GetSingleImageSize(description);
			if (size <= 0)
				return false;
			// if multiple face cubemap, we cannot set the image
			if (IsEmpty() || IsSingleImage())
			{
				if (images[(size_t)CubeMapImageType::ImageSingle] != nullptr && release_images[(size_t)CubeMapImageType::ImageSingle])
					FreeImage_Unload(images[(size_t)CubeMapImageType::ImageSingle]);
				images[(size_t)CubeMapImageType::ImageSingle] = image;
				release_images[(size_t)CubeMapImageType::ImageSingle] = release_image;
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
				for (size_t i = (size_t)CubeMapImageType::ImageLeft ; (i <= (size_t)CubeMapImageType::ImageBack) && (other_image == nullptr) ; ++i)
					if (i != (size_t)image_type)
						other_image = images[i];

				// if there is other images in the cubemap, ensure for compatibility
				if (other_image != nullptr && !AreImageCompatible(image, other_image))
					return false;

				// release previous image
				FIBITMAP * previous_image = images[(size_t)image_type];
				if (previous_image != nullptr && release_images[(size_t)image_type])
					FreeImage_Unload(previous_image);

				// do the writing
				images[(size_t)image_type] = image;
				release_images[(size_t)image_type] = release_image;
				return true;
			}
		}
		return false;
	}

	CubeMapImages CubeMapTools::LoadSingleCubeMap(FilePathParam const & path)
	{
		CubeMapImages result;
		DoLoadMultipleCubeMap_OneImage(result, path, CubeMapImageType::ImageSingle);
		return result;
	}

	CubeMapImages CubeMapTools::LoadMultipleCubeMap(FilePathParam const & left_image, FilePathParam const & right_image, FilePathParam const & top_image, FilePathParam const & bottom_image, FilePathParam const & front_image, FilePathParam const & back_image)
	{
		CubeMapImages result;
		if (
			!DoLoadMultipleCubeMap_OneImage(result, left_image, CubeMapImageType::ImageLeft)   ||
			!DoLoadMultipleCubeMap_OneImage(result, right_image, CubeMapImageType::ImageRight)  ||
			!DoLoadMultipleCubeMap_OneImage(result, top_image, CubeMapImageType::ImageTop)    ||
			!DoLoadMultipleCubeMap_OneImage(result, bottom_image, CubeMapImageType::ImageBottom) ||
			!DoLoadMultipleCubeMap_OneImage(result, front_image, CubeMapImageType::ImageFront)  ||
			!DoLoadMultipleCubeMap_OneImage(result, back_image, CubeMapImageType::ImageBack)
			)
			result.Release(); // release whole object in case of error
		return result;
	}

	bool CubeMapTools::DoLoadMultipleCubeMap_OneImage(CubeMapImages & cubemap, FilePathParam const & path, CubeMapImageType image_index)
	{
		FIBITMAP * image = ImageTools::LoadImageFromFile(path);
		if (image == nullptr)
			return false;

		if (!cubemap.SetImage(image_index, image, false))
		{
			FreeImage_Unload(image);
			return false;
		}
		return true;
	}
}; // namespace chaos

