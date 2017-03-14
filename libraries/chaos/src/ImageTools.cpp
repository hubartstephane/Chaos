#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>


namespace chaos
{
	class FillImageMetaFunc
	{
	public:

		/// constructor
		FillImageMetaFunc(ImageDescription & in_dst_desc, glm::vec4 const & in_color) :	
			dst_desc(in_dst_desc),
			color(in_color)
		{
			assert(dst_desc.IsValid());
			assert(!dst_desc.IsEmpty());
			dst_format = dst_desc.pixel_format.GetFormat();
		}

		/// the dispatch function
		template<typename DST_TYPE>
		void operator()(DST_TYPE dst_color)
		{
			PixelFormat pf = PixelFormat::GetPixelFormat<DST_TYPE>();
			if (pf.GetFormat() == dst_format)
			{
				PixelRGBAFloat rgba_color; // take the color with the most precision ...
				rgba_color.R = color.x;
				rgba_color.G = color.y;
				rgba_color.B = color.z;
				rgba_color.A = color.w;

				PixelConverter::Convert(dst_color, rgba_color); // ... convert it into the wanted PixelType

																// step 1 : fill line 1 (with standard assignement)
				DST_TYPE * d1 = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, 0, 0);
				for (int c = 0; c < dst_desc.width; ++c)
					d1[c] = dst_color;

				// step2 : fill other lines with memcpy(...) : should be faster
				for (int l = 1; l < dst_desc.height; ++l)
				{
					DST_TYPE * d2 = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, 0, l);
					memcpy(d2, d1, dst_desc.line_size);
				}
			}
		}

	public:

		/// the well known format for destination pixels
		int dst_format;
		/// the color to be applyed
		glm::vec4 color;
		/// the parameters for copy
		ImageDescription dst_desc;
	};

	//
	// XXX : the usage of FreeImage_FillBackground(...) is rather unclear
	//
	// for RGBA unsigned values, ALPHA must be 255 !!! (for RGBA float values, ALPHA is supported)
	//
	// for RGBA float values, we must use C float array
	//
	// for palettized image, there is a search in texture
	//

	void ImageTools::FillImageBackground(ImageDescription & image_description, glm::vec4 const & color)
	{
		if (!image_description.IsEmpty())
		{
			FillImageMetaFunc fill_func_map(image_description, color);

			boost::mpl::for_each<PixelTypes>(fill_func_map);
		}

#if 0 // keep for example, but the template implementation should fix the alpha issue

		assert(image != nullptr);

		PixelFormat pixel_format = ImageTools::GetPixelFormat(image);
		if (pixel_format.IsValid())
		{
			if (pixel_format.component_count == 1) // GRAY
			{
				if (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
				{
					unsigned char gray_color = (unsigned char)(255.0f * (color.r + color.g + color.b) / 3.0f);

					unsigned char bgra[4];
					bgra[0] = gray_color;
					bgra[1] = gray_color;						
					bgra[2] = gray_color;
					bgra[3] = 255; // XXX : important for FreeImage (depending of the flag of FI_COLOR_IS_RGBA_COLOR)

					FreeImage_FillBackground(image, bgra, FI_COLOR_IS_RGB_COLOR);
				}
				else if (pixel_format.component_type == PixelFormat::TYPE_FLOAT)
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
				if (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
				{
					unsigned char bgra[4];
					bgra[0] = (unsigned char)(color.b * 255.0f);
					bgra[1] = (unsigned char)(color.g * 255.0f);
					bgra[2] = (unsigned char)(color.r * 255.0f);
					bgra[3] = 255; // XXX : important for FreeImage (depending of the flag of FI_COLOR_IS_RGBA_COLOR)

					FreeImage_FillBackground(image, bgra, FI_COLOR_IS_RGB_COLOR);

				}
				else if (pixel_format.component_type == PixelFormat::TYPE_FLOAT)
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

	FREE_IMAGE_TYPE ImageTools::GetFreeImageType(PixelFormat const & pixel_format, int * bpp)
	{
		if (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
		{
			if (pixel_format.component_count == 1 || pixel_format.component_count == 3 || pixel_format.component_count == 4)
			{
				if (bpp != nullptr)
					*bpp = pixel_format.component_count * 8;
				return FIT_BITMAP;		
			}
		}
		else if (pixel_format.component_type == PixelFormat::TYPE_FLOAT)
		{
			if (pixel_format.component_count == 1)
				return FIT_FLOAT;
			if (pixel_format.component_count == 3)
				return FIT_RGBF;
			if (pixel_format.component_count == 4)
				return FIT_RGBAF;
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

			result.component_type = PixelFormat::TYPE_UNSIGNED_CHAR;
			result.component_count = bpp / 8;
		}
		else if (image_type == FIT_FLOAT || image_type == FIT_RGBF || image_type == FIT_RGBAF) // floating points format are accepted
		{
			result.component_type = PixelFormat::TYPE_FLOAT;

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
			result.width        = FreeImage_GetWidth(image);
			result.height       = FreeImage_GetHeight(image);
			result.pixel_format = pixel_format;
			result.line_size    = FreeImage_GetLine(image);
			result.pitch_size   = FreeImage_GetPitch(image);
			result.padding_size = result.pitch_size - result.line_size;
			result.data         = FreeImage_GetBits(image);

			// test whether the result is valid (line_size & pitch come from FreeImage ... just in case ... helps ensure ::IsValid() implementation is correct)
			if (result.IsValid())
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
	// we use the boost::mpl::for_each(...) function twice for that.
	// and the end we can call a meta function
	//
	//   CopyPixels<DST_TYPE, SRC_TYPE>

	template<typename SRC_TYPE> // forward declaration
	class CopyPixelMetaFunc2;

	class CopyPixelMetaFunc
	{
	public:

		/// constructor
		CopyPixelMetaFunc(ImageDescription const & in_src_desc, ImageDescription & in_dst_desc, int in_src_x, int in_src_y, int in_dst_x, int in_dst_y, int in_width, int in_height, bool in_central_symetry) :
			src_desc(in_src_desc), dst_desc(in_dst_desc),
			src_x(in_src_x), src_y(in_src_y),
			dst_x(in_dst_x), dst_y(in_dst_y),
			width(in_width), height(in_height),
			central_symetry(in_central_symetry)
		{
			assert(src_desc.IsValid());
			assert(dst_desc.IsValid());

			assert(width >= 0);
			assert(height >= 0);
			assert(src_x >= 0 && src_x + width <= src_desc.width);
			assert(src_y >= 0 && src_y + height <= src_desc.height);
			assert(dst_x >= 0 && dst_x + width <= dst_desc.width);
			assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

			src_format = src_desc.pixel_format.GetFormat();
			dst_format = dst_desc.pixel_format.GetFormat();
		}

		/// the dispatch function
		template<typename DST_TYPE>
		void operator()(DST_TYPE x)
		{
			PixelFormat pf = PixelFormat::GetPixelFormat<DST_TYPE>();
			if (pf.GetFormat() == dst_format)
				boost::mpl::for_each<PixelTypes>(CopyPixelMetaFunc2<DST_TYPE>(this));
		}

	public:

		/// copy function
		template<typename DST_TYPE, typename SRC_TYPE>
		void CopyPixels(bool central_symetry)
		{
			// normal copy
			if (!central_symetry)
			{
				if (boost::is_same<DST_TYPE, SRC_TYPE>::value)
				{
					for (int l = 0; l < height; ++l) // optimized version using memcopy, if there is no conversion to do
					{
						using TYPE = DST_TYPE; // same types fro both src and dst

						TYPE const * s = ImageTools::GetPixelAddress<TYPE>(src_desc, src_x, src_y + l);
						TYPE       * d = ImageTools::GetPixelAddress<TYPE>(dst_desc, dst_x, dst_y + l);
						memcpy(d, s, width * sizeof(TYPE));
					}
				}
				else
				{
					for (int l = 0; l < height; ++l)
					{
						SRC_TYPE const * s = ImageTools::GetPixelAddress<SRC_TYPE>(src_desc, src_x, src_y + l);
						DST_TYPE       * d = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, dst_x, dst_y + l);
						for (int c = 0; c < width; ++c)
							PixelConverter::Convert(d[c], s[c]);
					}
				}
			}
			// copy with central symetry
			else
			{
				for (int l = 0; l < height; ++l)
				{
					SRC_TYPE const * s = ImageTools::GetPixelAddress<SRC_TYPE>(src_desc, src_x, src_y + l);
					DST_TYPE       * d = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, dst_x, dst_y + height - 1 - l);
					for (int c = 0; c < width; ++c)
						PixelConverter::Convert(d[width - 1 - c], s[c]);
				}
			}
		}

	public:

		/// the well known format for source pixels
		int src_format;
		/// the well known format for destination pixels
		int dst_format;
		/// the parameters for copy
		ImageDescription src_desc;
		ImageDescription dst_desc;
		int src_x;
		int src_y;
		int dst_x;
		int dst_y;
		int width; 
		int height;
		// whether a central symetry is to be applyed
		bool central_symetry;
	};

	//
	// CopyPixelMetaFunc2 : used to find SRC_TYPE and start the copy
	// (DST_TYPE is already well known)
	//

	template<typename DST_TYPE> 
	class CopyPixelMetaFunc2
	{
	public:

		/// constructor
		CopyPixelMetaFunc2(CopyPixelMetaFunc * in_params) : params(in_params) {}

		/// dispatch function
		template<typename SRC_TYPE>
		void operator()(SRC_TYPE x)
		{
			PixelFormat pf = PixelFormat::GetPixelFormat<SRC_TYPE>();
			if (pf.GetFormat() == params->src_format)
				params->CopyPixels<DST_TYPE, SRC_TYPE>(params->central_symetry);
		}

	public:

		CopyPixelMetaFunc * params;
	};

	void ImageTools::CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height, bool central_symetry)
	{
		CopyPixelMetaFunc copy_func_map(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height, central_symetry);

		boost::mpl::for_each<PixelTypes>(copy_func_map);	// start by detecting DST_TYPE		
	}

	ImageDescription ImageTools::ConvertPixels(ImageDescription const & src_desc, PixelFormat const & final_pixel_format, char * conversion_buffer, bool central_symetry)
	{
		ImageDescription result;

		result.pixel_format = final_pixel_format;
		result.width = src_desc.width;
		result.height = src_desc.height;
		result.data = conversion_buffer;
		result.line_size = result.width * final_pixel_format.GetPixelSize();
		result.pitch_size = result.line_size;
		result.padding_size = 0;

		assert(result.IsValid());

		ImageTools::CopyPixels(src_desc, result, 0, 0, 0, 0, result.width, result.height, central_symetry); // do the conversion + symmetry

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

		// multiple components => not grayscale
		if (pixel_format.component_count != 1)
			return false;

		// a 'luminance' image is a grayscale
		if (pixel_format.component_type == PixelFormat::TYPE_FLOAT)
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

		for (unsigned int i = 0 ; i < color_count ; ++i)
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

	FIBITMAP * ImageTools::LoadImageFromBuffer(Buffer<char> buffer)
	{
		FIBITMAP * result = nullptr;

		if (buffer != nullptr)
		{
			FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
			if (memory != nullptr)
			{
				FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);

				result = ConvertToSupportedType(FreeImage_LoadFromMemory(format, memory, 0), true);

				FreeImage_CloseMemory(memory);
			}
		}	
		return result;	
	}

	FIBITMAP * ImageTools::LoadImageFromFile(char const * filename)
	{
		assert(filename != nullptr);
		return LoadImageFromBuffer(FileTools::LoadFile(filename, false));
	}

	FIMULTIBITMAP * ImageTools::LoadMultiImageFromFile(char const * filename)
	{
		assert(filename != nullptr);

		FIMULTIBITMAP * result = nullptr;

		Buffer<char> buffer = FileTools::LoadFile(filename, false);
		if (buffer != nullptr)
		{
			FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
			if (memory != nullptr)
			{
				FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);

				result = FreeImage_LoadMultiBitmapFromMemory(format, memory, 0);

				FreeImage_CloseMemory(memory);
			}
		}
		return result;
	}

}; // namespace chaos

