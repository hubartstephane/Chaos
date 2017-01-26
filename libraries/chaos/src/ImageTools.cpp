#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>


namespace chaos
{

	ImageDescription::ImageDescription(void * in_data, int in_width, int in_height, int in_component_type, int in_component_count, int in_padding):
		data(in_data),
		width(in_width),
		height(in_height),
		component_type(in_component_type),
		component_count(in_component_count),
		padding_size(in_padding)
	{
		line_size  = width * GetPixelSize();
		pitch_size = line_size + padding_size;  

		assert(IsValid());
	}

	int ImageDescription::GetBPP() const
	{
		if (!IsValid())
			return 0;
		if (component_type == TYPE_FLOAT)
			return 0;
		return component_count * 8;
	}

	bool ImageDescription::IsEmpty() const 
	{ 
		return (width == 0 || height == 0 || data == nullptr); 
	}

	int ImageDescription::GetComponentSize() const
	{
		if (component_type == TYPE_UNSIGNED_CHAR)
			return sizeof(unsigned char);
		if (component_type == TYPE_FLOAT)
			return sizeof(float);
		return 0;	
	}

	int ImageDescription::GetPixelSize() const
	{
		return GetComponentSize() * component_count;	
	}

	bool ImageDescription::IsPixelFormatValid() const
	{
		if (component_type != TYPE_UNSIGNED_CHAR && component_type != TYPE_FLOAT)
			return false;
		if (component_count != 1 && component_count != 3 && component_count != 4)
			return false;
		return true;
	}

	bool ImageDescription::ArePixelFormatSame(ImageDescription const & other) const
	{
		return (component_type == other.component_type) && (component_count == other.component_count);	
	}

	bool ImageDescription::IsValid() const
	{
		// size cannot be negative
		if (width < 0 || height < 0)
			return false;
		// should not have a buffer if the image is empty
		if (width == 0 || height == 0)
			if (data != nullptr)
				return false;
		// if image is empty, image size should be 0
		if (data == nullptr)
			if (width != 0 || height != 0)
				return false;
		// the format should be well known
		if (!IsPixelFormatValid())
			return false;
		// test data consistency
		if (line_size != width * GetPixelSize())
			return false;
		if (pitch_size < 0 || padding_size < 0)
			return false;
		if (pitch_size != padding_size + line_size)
			return false;

		return true;		
	}

	ImageDescription ImageDescription::GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const
	{ 
		ImageDescription result;

		// test whether we are a valid description
		if (!IsValid())
			return result;
		// want empty image : return null image 
		if (wanted_width < 0 || wanted_height < 0)
			return result;
		// wrong parameters : return null image
		if (x < 0 || x + wanted_width > width)
			return result;
		if (y < 0 || y + wanted_height > height)
			return result;

		int pixel_size = GetPixelSize();

		result.width           = wanted_width;
		result.height          = wanted_height;
		result.component_type  = component_type;
		result.component_count = component_count;
		result.pitch_size      = pitch_size;
		result.line_size       = wanted_width * pixel_size;
		result.padding_size    = result.pitch_size - result.line_size;

		if (wanted_width != 0 && wanted_height != 0 && !IsEmpty())
		{
			int offset = 0;    
			offset += (x * pixel_size);
			offset += (y * pitch_size); 		
			result.data = ((char*)data) + offset;
		}	

		return result;
	}

	ImageDescription ImageTools::GetImageDescription(FIBITMAP * image)
	{
		assert(image != nullptr);

		ImageDescription result;

		// test whether we can handle that format
		int component_count = 0;
		int component_type  = 0;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(image);
		if (image_type == FIT_BITMAP)
		{			
			int bpp = FreeImage_GetBPP(image); // ignore other format than 8, 24 and 32 bpp
			if (bpp != 8 && bpp != 24 && bpp != 32)
				return result;			

			component_type  = ImageDescription::TYPE_UNSIGNED_CHAR;
			component_count = bpp / 8;
		}
		else if (image_type == FIT_FLOAT || image_type == FIT_RGBF || image_type == FIT_RGBAF) // floating points format are accepted
		{
			component_type  = ImageDescription::TYPE_FLOAT;

			if (image_type == FIT_FLOAT)
				component_count = 1;
			else if (image_type == FIT_RGBF)
				component_count = 3;
			else if (image_type == FIT_RGBAF)
				component_count = 4;
		}
		else
			return result;

		// fullfill missing members
		result.width           = FreeImage_GetWidth(image); // FREEIMAGE does use constness correctly
		result.height          = FreeImage_GetHeight(image);
		result.component_type  = component_type;
		result.component_count = component_count;
		result.line_size       = FreeImage_GetLine(image);
		result.pitch_size      = FreeImage_GetPitch(image);
		result.padding_size    = result.pitch_size - result.line_size;
		result.data            = FreeImage_GetBits(image);

		// test whether the result is valid (line_size & pitch come from FreeImage)
		if (!result.IsValid())
			result = ImageDescription();

		return result;
	}

	class ImageToolsCopyPixelsImpl
	{
	public:

		template<typename T>
		static void CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
		{
			for (int l = 0 ; l < height ; ++l)
			{
				T const * s = ImageTools::GetPixelAddress<T>(src_desc, src_x, src_y + l);
				T       * d = ImageTools::GetPixelAddress<T>(dst_desc, dst_x, dst_y + l);
				memcpy(d, s, width * sizeof(T));
			}		
		}	

		template<typename T>
		static void CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
		{
			for (int l = 0 ; l < height ; ++l)
			{
				T const * s = ImageTools::GetPixelAddress<T>(src_desc, src_x, src_y + l);
				T       * d = ImageTools::GetPixelAddress<T>(dst_desc, dst_x, dst_y + height - 1 - l);
				for (int c = 0 ; c < width ; ++c)
					d[width - 1 - c] = s[c];
			}		
		}	
	};

	void ImageTools::CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
	{
		assert(src_desc.IsValid());
		assert(dst_desc.IsValid());
		assert(src_desc.ArePixelFormatSame(dst_desc));

		assert(width  >= 0);
		assert(height >= 0);
		assert(src_x >= 0 && src_x + width  <= src_desc.width);
		assert(src_y >= 0 && src_y + height <= src_desc.height);
		assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
		assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

		if (src_desc.component_type == ImageDescription::TYPE_UNSIGNED_CHAR)
		{
			if (src_desc.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelGray>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGB>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGBA>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
		else if (src_desc.component_type == ImageDescription::TYPE_FLOAT)
		{
			if (src_desc.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelGrayFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGBFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGBAFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
	}

	void ImageTools::CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
	{
		assert(src_desc.IsValid());
		assert(dst_desc.IsValid());
		assert(src_desc.ArePixelFormatSame(dst_desc));

		assert(width  >= 0);
		assert(height >= 0);
		assert(src_x >= 0 && src_x + width  <= src_desc.width);
		assert(src_y >= 0 && src_y + height <= src_desc.height);
		assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
		assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

		if (src_desc.component_type == ImageDescription::TYPE_UNSIGNED_CHAR)
		{
			if (src_desc.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelGray>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGB>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGBA>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
		else if (src_desc.component_type == ImageDescription::TYPE_FLOAT)
		{
			if (src_desc.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelGrayFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGBFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_desc.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGBAFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
	}

	bool ImageTools::IsGrayscaleImage(FIBITMAP * image)
	{
		assert(image != nullptr);

		// ignore unhandled message
		ImageDescription desc = ImageTools::GetImageDescription(image);
		if (!desc.IsValid())
			return false;

		// multiple components => not grayscale
		if (desc.component_count != 1)
			return false;

		// a 'luminance' image is a grayscale
		if (desc.component_type == ImageDescription::TYPE_FLOAT)
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
		}
		return true;
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

				result = FreeImage_LoadFromMemory(format, memory, 0);

				if (FreeImage_GetImageType(result) == FIT_BITMAP && FreeImage_GetBPP(result) == 8)
				{
					if (!IsGrayscaleImage(result)) // don't want a palette any more
					{
						FIBITMAP * other = FreeImage_ConvertTo32Bits(result);
						FreeImage_Unload(result);
						result = other;           // this code is good even if the conversion fails 
					}
				}
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
