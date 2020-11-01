#include <chaos/Chaos.h>

namespace chaos
{
	// XXX : from breakpoint and crash tests, it seems that
	//  - Buffer<char> should not be destroyed/altered until you re done using MEMORY
	//  - for FIBITMAP, you can close the MEMORY once FreeImage_LoadFromMemory(...) is done
	//  - for FIMULTIBITMAP, it seems that you should not destroy the MEMORY until you have finished accessing the pages
	//
	//   Lifetime(buffer) > Lifetime(MEMORY)
	//   Lifetime(MEMORY) < or > Lifetime(BITMAP)  (unrelated)	
	//   Lifetime(MEMORY) > Lifetime(MULTIBITMAP)
	//
	// XXX : FreeImage_LoadMultiBitmap...(...) functions
	//
	//   - can read GIF correctly and lock pages even if there is a single image
	//   - for other formats, the lock page fails

	//
	// XXX : the usage of FreeImage_FillBackground(...) is rather unclear
	//
	// for RGBA unsigned values, ALPHA must be 255 !!! (for RGBA float values, ALPHA is supported)
	//
	// for RGBA float values, we must use C float array
	//
	// for palettized image, there is a search in texture
	//
	//   => use our own implementation

	void ImageTools::FillImageBackground(ImageDescription & image_description, glm::vec4 const & color)
	{
		if (image_description.IsEmpty(false))
			return;

		meta::for_each<PixelTypes>([image_description, color](auto value) -> bool
		{
			using pixel_type = typename decltype(value)::type;

			// check we are working with correct pixel format
			if (image_description.pixel_format != PixelFormat::GetPixelFormat<pixel_type>())
				return false;

			// take the color with the most precision ...
			PixelRGBAFloat rgba_color; 
			rgba_color.R = color.x;
			rgba_color.G = color.y;
			rgba_color.B = color.z;
			rgba_color.A = color.w;

			// ... convert it into the wanted PixelType
			pixel_type dst_color;
			PixelConverter::Convert(dst_color, rgba_color); 

			// step 1 : fill line 1 (with standard assignement)
			ImagePixelAccessor<pixel_type> dst_acc(image_description);

			pixel_type* line1 = &dst_acc(0, 0);
			for (int c = 0; c < image_description.width; ++c)
				line1[c] = dst_color;

			// step2 : fill other lines with memcpy(...) : should be faster
			for (int l = 1; l < image_description.height; ++l)
			{
				pixel_type* line2 = &dst_acc(0, l);
				memcpy(line2, line1, image_description.line_size);
			}

			return true;
				
		}, false);

#if 0 // keep for example, but the template implementation should fix the alpha issue

		assert(image != nullptr);

		PixelFormat pixel_format = ImageTools::GetPixelFormat(image);
		if (pixel_format.IsValid())
		{
			if (pixel_format.component_count == 1) // GRAY
			{
				if (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)
				{
					unsigned char gray_color = (unsigned char)(255.0f * (color.r + color.g + color.b) / 3.0f);

					unsigned char bgra[4];
					bgra[0] = gray_color;
					bgra[1] = gray_color;
					bgra[2] = gray_color;
					bgra[3] = 255; // XXX : important for FreeImage (depending of the flag of FI_COLOR_IS_RGBA_COLOR)

					FreeImage_FillBackground(image, bgra, FI_COLOR_IS_RGB_COLOR);
				}
				else if (pixel_format.component_type == PixelComponentType::FLOAT)
				{
					float gray_color = (color.r + color.g + color.b) / 3.0f;

					float rgba[4];
					rgba[0] = gray_color;
					rgba[1] = gray_color;
					rgba[2] = gray_color;
					rgba[3] = 1.0f;

					FreeImage_FillBackground(image, rgba, FI_COLOR_IS_RGB_COLOR);
				}
			}
			else // COLOR
			{
				if (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)
				{
					unsigned char bgra[4];
					bgra[0] = (unsigned char)(color.b * 255.0f);
					bgra[1] = (unsigned char)(color.g * 255.0f);
					bgra[2] = (unsigned char)(color.r * 255.0f);
					bgra[3] = 255; // XXX : important for FreeImage (depending of the flag of FI_COLOR_IS_RGBA_COLOR)

					FreeImage_FillBackground(image, bgra, FI_COLOR_IS_RGB_COLOR);

				}
				else if (pixel_format.component_type == PixelComponentType::FLOAT)
				{
					float rgba[4];
					rgba[0] = color.r;
					rgba[1] = color.g;
					rgba[2] = color.b;
					rgba[3] = color.a;

					FreeImage_FillBackground(image, rgba, FI_COLOR_IS_RGBA_COLOR);
				}
			}
		}
#endif
	}

	FIBITMAP * ImageTools::GenFreeImage(PixelFormat const & pixel_format, int width, int height)
	{
		assert(width >= 0);
		assert(height >= 0);

		FIBITMAP * result = nullptr;

		// test whether pixel format is valid and supported
		if (!pixel_format.IsValid())
			return result;

		// get freeimage format
		int bpp = 0;
		FREE_IMAGE_TYPE image_type = GetFreeImageType(pixel_format, &bpp);
		if (image_type == FIT_UNKNOWN)
			return nullptr;

		// allocate the freeimage
		return FreeImage_AllocateT(image_type, width, height, bpp);
	}

	FIBITMAP * ImageTools::GenFreeImage(ImageDescription const & src_desc)
	{
		// ensure the source is valid
		if (!src_desc.IsValid(false))
			return nullptr;
		// check type and bpp
		int bpp = 0;
		FREE_IMAGE_TYPE image_type = GetFreeImageType(src_desc.pixel_format, &bpp);
		if (image_type == FIT_UNKNOWN)
			return nullptr;
		// allocate the new texture
		FIBITMAP * result = FreeImage_AllocateT(image_type, src_desc.width, src_desc.height, bpp);
		if (result == nullptr)
			return nullptr;
		// fill the bitmap
		ImageDescription dst_desc = GetImageDescription(result);
		ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, 0, 0, src_desc.width, src_desc.height, ImageTransform::NO_TRANSFORM);

		return result;
	}


#if 0

	FIBITMAP * ImageTools::GenFreeImage(GLuint texture_id, GLint level)
	{
		FIBITMAP * result = nullptr;

		ImageDescription desc;
		char * pixels = GLTextureTools::GetTextureImage(texture_id, level, desc);
		if (pixels != nullptr)
		{
			result = GenFreeImage(desc);
			delete[] pixels;
		}
		return result;
	}
#endif


	FREE_IMAGE_FORMAT ImageTools::GetFreeImageFormat(PixelFormat const & pixel_format)
	{
		return (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR) ? FIF_PNG : FIF_EXR;
	}

	FREE_IMAGE_TYPE ImageTools::GetFreeImageType(PixelFormat const & pixel_format, int * bpp)
	{
		if (pixel_format.component_type == PixelComponentType::UNSIGNED_CHAR)
		{
			if (pixel_format.component_count == 1 || pixel_format.component_count == 3 || pixel_format.component_count == 4)
			{
				if (bpp != nullptr)
					*bpp = pixel_format.component_count * 8;
				return FIT_BITMAP;
			}
		}
		else if (pixel_format.component_type == PixelComponentType::FLOAT)
		{
			if (pixel_format.component_count == 1)
				return FIT_FLOAT;
			if (pixel_format.component_count == 3)
				return FIT_RGBF;
			if (pixel_format.component_count == 4)
				return FIT_RGBAF;
		}
		else if (pixel_format.component_type == PixelComponentType::DEPTH_STENCIL)
		{
			assert(0);
		}
		return FIT_UNKNOWN;
	}

	PixelFormat ImageTools::GetPixelFormat(FIBITMAP * image)
	{
		assert(image != nullptr);

		PixelFormat result;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(image);
		if (image_type == FIT_BITMAP)
		{
			int bpp = FreeImage_GetBPP(image); // ignore other format than 8, 24 and 32 bpp
			if (bpp != 8 && bpp != 24 && bpp != 32)
				return result;

			result.component_type = PixelComponentType::UNSIGNED_CHAR;
			result.component_count = bpp / 8;
		}
		else if (image_type == FIT_FLOAT || image_type == FIT_RGBF || image_type == FIT_RGBAF) // floating points format are accepted
		{
			result.component_type = PixelComponentType::FLOAT;

			if (image_type == FIT_FLOAT)
				result.component_count = 1;
			else if (image_type == FIT_RGBF)
				result.component_count = 3;
			else if (image_type == FIT_RGBAF)
				result.component_count = 4;
		}
		return result;
	}

	ImageDescription ImageTools::GetImageDescription(FIBITMAP * image)
	{
		assert(image != nullptr);

		// test whether we can handle that format
		PixelFormat pixel_format = ImageTools::GetPixelFormat(image);
		if (pixel_format.IsValid())
		{
			ImageDescription result;
			result.width = FreeImage_GetWidth(image);
			result.height = FreeImage_GetHeight(image);
			result.pixel_format = pixel_format;
			result.line_size = FreeImage_GetLine(image);
			result.pitch_size = FreeImage_GetPitch(image);
			result.padding_size = result.pitch_size - result.line_size;
			result.data = FreeImage_GetBits(image);

			// test whether the result is valid (line_size & pitch come from FreeImage ... just in case ... helps ensure ::IsValid() implementation is correct)
			if (result.IsValid(false))
				return result;
		}
		return ImageDescription();
	}

	//
	// To copy pixels and make conversions, we have to
	//
	//   - from src_desc => get a Pixel Type
	//   - from dst_desc => get a Pixel Type
	//
	// we use the meat::for_each(...) function twice for that.

	void ImageTools::CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height, ImageTransform image_transform)
	{
		// all possible SRC pixel types
		meta::for_each<PixelTypes>([src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height, image_transform](auto value) -> bool
		{
			using src_pixel_type = typename decltype(value)::type;

			if (src_desc.pixel_format != PixelFormat::GetPixelFormat<src_pixel_type>())
				return false;

			// all possible DST pixel types
			meta::for_each<PixelTypes>([src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height, image_transform](auto value) -> bool
			{
				using dst_pixel_type = typename decltype(value)::type;

				if (dst_desc.pixel_format != PixelFormat::GetPixelFormat<dst_pixel_type>())
					return false;

				ImagePixelAccessor<src_pixel_type> src_acc(src_desc);
				ImagePixelAccessor<dst_pixel_type> dst_acc(dst_desc);

				// normal copy
				if (image_transform == ImageTransform::NO_TRANSFORM)
				{
					if (boost::is_same<dst_pixel_type, src_pixel_type>::value)
					{
						for (int l = 0; l < height; ++l) // optimized version using memcopy, if there is no conversion to do
						{
							src_pixel_type const* src_line = &src_acc(src_x, src_y + l);
							dst_pixel_type		* dst_line = &dst_acc(dst_x, dst_y + l);
							memcpy(dst_line, src_line, width * sizeof(src_pixel_type));
						}
					}
					else
					{
						for (int l = 0; l < height; ++l)
						{
							src_pixel_type const* src_line = &src_acc(src_x, src_y + l);
							dst_pixel_type		* dst_line = &dst_acc(dst_x, dst_y + l);
							for (int c = 0; c < width; ++c)
								PixelConverter::Convert(dst_line[c], src_line[c]);
						}
					}
				}
				// copy with central symetry 
				//   no interest to test if source type and dest types are identical because we cannot use memcpy(...) due to symetry
				else if (image_transform == ImageTransform::CENTRAL_SYMETRY)
				{
					for (int l = 0; l < height; ++l)
					{
						src_pixel_type const* src_line = &src_acc(src_x, src_y + l);
						dst_pixel_type		* dst_line = &dst_acc(dst_x, dst_y + height - 1 - l);
						for (int c = 0; c < width; ++c)
							PixelConverter::Convert(dst_line[width - 1 - c], src_line[c]);
					}
				}
				else
				{
					assert(0);
				}
				return true;
			});

			return true;
		});
	}

	int ImageTools::GetMemoryRequirementForAlignedTexture(PixelFormat const & pixel_format, int width, int height)
	{
		return ((width * pixel_format.GetPixelSize() + 3) & ~3) // aligned rows
			* height
			+ 3; // for base alignment
	}

	ImageDescription ImageTools::GetImageDescriptionForAlignedTexture(PixelFormat const & pixel_format, int width, int height, char * buffer)
	{
		ImageDescription result;

		int pixel_size = pixel_format.GetPixelSize();
		int line_size = width * pixel_size;
		int pitch_size = ((line_size + 3) & ~3);
		int padding = (pitch_size - line_size);

		void * aligned_buffer = (void*)((((uintptr_t)buffer) + 3) & ~3);

		result.pixel_format = pixel_format;
		result.width = width;
		result.height = height;
		result.data = aligned_buffer;
		result.line_size = line_size;
		result.pitch_size = pitch_size;
		result.padding_size = padding;

		return result;
	}

	ImageDescription ImageTools::ConvertPixels(ImageDescription const & src_desc, PixelFormat const & final_pixel_format, char * conversion_buffer, ImageTransform image_transform)
	{
		ImageDescription result = GetImageDescriptionForAlignedTexture(final_pixel_format, src_desc.width, src_desc.height, conversion_buffer);
		assert(result.IsValid(false));
		ImageTools::CopyPixels(src_desc, result, 0, 0, 0, 0, result.width, result.height, image_transform); // do the conversion + symmetry
		return result;
	}

	bool ImageTools::IsGrayscaleImage(FIBITMAP * image, bool * alpha_needed)
	{
		assert(image != nullptr);

		// pointer on alpha_needed
		bool tmp_alpha_needed = false;
		if (alpha_needed == nullptr)
			alpha_needed = &tmp_alpha_needed;

		*alpha_needed = false;

		// ignore unhandled pixel format
		PixelFormat pixel_format = ImageTools::GetPixelFormat(image);
		if (!pixel_format.IsValid())
			return false;

		// should never happens ..
		if (pixel_format.IsDepthStencilPixel())
		{
			assert(0);
			return false;
		}

		// multiple components => not grayscale
		if (pixel_format.component_count != 1)
			return false;

		// a 'luminance' image is a grayscale
		if (pixel_format.component_type == PixelComponentType::FLOAT)
			return true;

		// 1 component of type UNSIGNED CHAR :
		//   - can be an index on a palette
		//   - can be a 'luminance' value

		RGBQUAD * palette = FreeImage_GetPalette(image); // luminance ?
		if (palette == nullptr)
			return true;

		// test whether the palette is a grayscale palette
		unsigned int color_count = FreeImage_GetColorsUsed(image);
		if (color_count != 256)
			return false;

		for (unsigned int i = 0; i < color_count; ++i)
		{
			if (palette[i].rgbRed != (BYTE)i)
				return false;
			if (palette[i].rgbGreen != (BYTE)i)
				return false;
			if (palette[i].rgbBlue != (BYTE)i)
				return false;

			*alpha_needed |= (palette[i].rgbReserved != 255); // alpha should always be 255 or the palette is interesting for encoding alpha
		}
		return true;
	}

	FIBITMAP * ImageTools::ConvertToSupportedType(FIBITMAP * image, bool can_delete_src)
	{
		if (image == nullptr)
			return nullptr;

		// try convert some format
		if (FreeImage_GetImageType(image) == FIT_BITMAP)
		{
			int bpp = FreeImage_GetBPP(image);

			if (bpp == 8)
			{
				bool alpha_needed = false;
				if (!IsGrayscaleImage(image, &alpha_needed)) // don't want a palette any more (this code is good even if the conversion fails)
				{
					FIBITMAP * other = nullptr;
					if (alpha_needed)
						other = FreeImage_ConvertTo32Bits(image); // keep alpha
					else
						other = FreeImage_ConvertTo24Bits(image);
					if (can_delete_src)
						FreeImage_Unload(image);
					return other;
				}
				return image; // ok					
			}
			else if (bpp == 16) // don't want 16 bpp any more
			{
				FIBITMAP * other = FreeImage_ConvertTo24Bits(image);
				if (can_delete_src)
					FreeImage_Unload(image);
				return other;
			}
		}

		// test whether pixel format is valid
		PixelFormat pixel_format = ImageTools::GetPixelFormat(image);
		if (!pixel_format.IsValid())
		{
			if (can_delete_src)
				FreeImage_Unload(image);
			return nullptr;
		}
		return image;
	}

	// XXX : Lifetime rules are respected (see note at the begining of this file)
	FIBITMAP * ImageTools::LoadImageFromBuffer(Buffer<char> & buffer)
	{
		FIBITMAP * result = nullptr;

		if (buffer != nullptr)
		{
			FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
			if (memory != nullptr)
			{
				FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);
				FIBITMAP * bmp = FreeImage_LoadFromMemory(format, memory, 0);
				result = ConvertToSupportedType(bmp, true);
				FreeImage_CloseMemory(memory);
			}
		}
		return result;
	}

	FIBITMAP * ImageTools::LoadImageFromFile(FilePathParam const & path)
	{
		return LoadImageFromBuffer(FileTools::LoadFile(path, false));
	}

	std::vector<FIBITMAP*> ImageTools::LoadMultipleImagesFromFile(FilePathParam const & path, ImageAnimationDescription * anim_description)
	{
		std::vector<FIBITMAP*> result;
		// load the image and get multi image
		Buffer<char> buffer = FileTools::LoadFile(path, false);
		if (buffer != nullptr)
			result = LoadMultipleImagesFromBuffer(buffer, anim_description);
		return result;
	}

	std::vector<FIBITMAP*> ImageTools::LoadMultipleImagesFromBuffer(Buffer<char> buffer, ImageAnimationDescription * anim_description)
	{
		std::vector<FIBITMAP*> result;

		FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
		if (memory != nullptr)
		{
			// format supporting image paging
			FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);
			if (format == FIF_GIF)
			{
				FIMULTIBITMAP * multi_bitmap = FreeImage_LoadMultiBitmapFromMemory(format, memory, 0);
				if (multi_bitmap != nullptr)
				{
					result = GetMultiImagePages(multi_bitmap, anim_description);
					FreeImage_CloseMultiBitmap(multi_bitmap, 0);
				}
			}
			// format that does not support image paging : simple image reading
			else
			{
				FIBITMAP * bitmap = FreeImage_LoadFromMemory(format, memory, 0);
				if (bitmap != nullptr)
				{
					bitmap = ConvertToSupportedType(bitmap, true); // convert to a supported format : may fails
					if (bitmap != nullptr)
						result.push_back(bitmap);
				}
			}				
			FreeImage_CloseMemory(memory);
		}
		return result;
	}

	std::vector<FIBITMAP *> ImageTools::GetMultiImagePages(FIMULTIBITMAP * multi_bitmap, ImageAnimationDescription * anim_description)
	{
		std::vector<FIBITMAP *> result;
		if (multi_bitmap != nullptr)
		{
			int page_count = FreeImage_GetPageCount(multi_bitmap);
			for (int i = 0; i < page_count; ++i)
			{
				// lock the page
				FIBITMAP * page = FreeImage_LockPage(multi_bitmap, i);
				if (page == nullptr)
					continue;

				// extract animation meta data if anny
				if (anim_description != nullptr && page_count > 1)
					if (GetImageAnimDescription(page, *anim_description)) // as soon as animation meta data has been extracted, ignore for every following pages
						anim_description = nullptr;

				// XXX : due to FreeImage library limitation, we cannot keep a reference to locked page
				//       we have to clone the page. While we may make a format conversion, we can avoid the cloning in some circonstences
				FIBITMAP * other_page = ConvertToSupportedType(page, false);
				if (other_page != nullptr)
				{
					if (other_page != page)
						result.push_back(other_page); // no need to clone the page, we already have a bitmap independant of the FIMULTIBITMAP
					else
						result.push_back(FreeImage_Clone(page)); // we cannot store a reference to locked page : make a costly clone !!!
				}
				FreeImage_UnlockPage(multi_bitmap, page, false);
			}			
		}
		return result;
	}

	bool ImageTools::GetImageAnimDescription(FIBITMAP * image, ImageAnimationDescription & result)
	{
		int frame_duration_ms = 0;		
		if (ReadMetaData(image, FIMD_ANIMATION, "FrameTime", frame_duration_ms))
		{
			result.frame_duration = 0.001f * (float)frame_duration_ms;
			return true;
		}
		return false;
		

		// Some metadata (GIF ?) FreeImage library read them
#if 0 
		std::int32_t frame_duration = 0;
		ReadMetaData(image, FIMD_ANIMATION, "FrameTime", frame_duration);

		std::int32_t loop = 0;
		ReadMetaData(image, FIMD_ANIMATION, "Loop", loop);

		std::int32_t frame_top = 0;
		ReadMetaData(image, FIMD_ANIMATION, "FrameTop", frame_top);

		std::int32_t frame_left = 0;
		ReadMetaData(image, FIMD_ANIMATION, "FrameLeft", frame_left);

		std::int32_t interlaced = 0;
		ReadMetaData(image, FIMD_ANIMATION, "Interlaced", interlaced);

		std::int32_t logical_width = 0;
		ReadMetaData(image, FIMD_ANIMATION, "LogicalWidth", logical_width);

		std::int32_t logical_height = 0;
		ReadMetaData(image, FIMD_ANIMATION, "LogicalHeight", logical_height);

		std::int32_t disposal_method = 0;
		ReadMetaData(image, FIMD_ANIMATION, "DisposalMethod", disposal_method);
#endif
	}


#if _WIN32 || _WIN64

	FIBITMAP* ImageTools::ConvertFromHBITMAP(HBITMAP hBitmap)
	{
		if (hBitmap == NULL)
			return nullptr;

		SIZE size = { 0, 0 };
		GetBitmapDimensionEx(hBitmap, &size);


		// shu46 to finish



		return nullptr;
	}

	HBITMAP ImageTools::ConvertToHBITMAP(FIBITMAP* bitmap, bool flip_vertical)
	{
		HBITMAP result = NULL;

		// early exit
		if (bitmap == nullptr)
			return NULL;

		// ensure we use supported formats (32bpp)
		ImageDescription desc = GetImageDescription(bitmap);

		if (desc.pixel_format != PixelFormat::BGRA)
		{
			FIBITMAP* bitmap_32bpp = FreeImage_ConvertTo32Bits(bitmap);
			if (bitmap_32bpp != nullptr)
			{
				if (flip_vertical)
					FreeImage_FlipVertical(bitmap_32bpp);
				result = ConvertToHBITMAP(bitmap_32bpp, false); // do not flip while already made (this spares a new bitmap creation)
				FreeImage_Unload(bitmap_32bpp);
			}
			return result;
		}

		// convert image to flipped one
		if (flip_vertical)
		{
			bitmap = FreeImage_Clone(bitmap); // has to be destroyed at the end
			if (bitmap == nullptr)
				return NULL;
			FreeImage_FlipVertical(bitmap);
			desc = GetImageDescription(bitmap);
		}

		// create the bitmap
		result = ::CreateBitmap(desc.width, desc.height, 1, 32, desc.data);
		// the bitmap has been exchanged with the flipped version
		if (flip_vertical)
			FreeImage_Unload(bitmap);
		return result;
	}

#endif // #if _WIN32 || _WIN64


}; // namespace chaos

