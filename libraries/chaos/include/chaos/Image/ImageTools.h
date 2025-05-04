namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImageTransform;

	class FIBITMAPDeleter;
	class FIMULTIBITMAPDeleter;

	using bitmap_ptr = std::unique_ptr<FIBITMAP, FIBITMAPDeleter>;
	using multibitmap_ptr = std::unique_ptr<FIMULTIBITMAP, FIMULTIBITMAPDeleter>;

	class ImageTools;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ImageLog, "Image")

	enum class CHAOS_API ImageTransform : int
	{
		NO_TRANSFORM = 0,
		CENTRAL_SYMETRY = 1
	};

	/**
	* FIBITMAPDeleter : deleter for FIBITMAP
	*/

	class CHAOS_API FIBITMAPDeleter
	{
	public:

		/** constructor */
		FIBITMAPDeleter(bool in_release = true) :
			release(in_release) {}
		/** the destruction method */
		void operator ()(FIBITMAP* bitmap)
		{
			if (release)
				FreeImage_Unload(bitmap);
		}

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

	/**
	* FIBITMAPDeleter : deleter for FIBITMAP
	*/

	class CHAOS_API FIMULTIBITMAPDeleter
	{
	public:

		/** constructor */
		FIMULTIBITMAPDeleter(bool in_release = true) :
			release(in_release) {}
		/** the destruction method */
		void operator ()(FIMULTIBITMAP* multi_bitmap)
		{
			if (release)
				FreeImage_CloseMultiBitmap(multi_bitmap);
		}

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

	/**
	* ImageTools : deserve to load some images
	*/

	class CHAOS_API ImageTools
	{

	public:

		/** fill the color of the background */
		static void FillImageBackground(ImageDescription& image_description, glm::vec4 const& color);

		/** generate a free image corresponding to a given pixel format */
		static FIBITMAP* GenFreeImage(PixelFormat const& pixel_format, int width, int height);
		/** generate a free image from a description */
		static FIBITMAP* GenFreeImage(ImageDescription const& src_desc);
		/** generate a free image from a texture */
		static FIBITMAP* GenFreeImage(GLuint texture_id, GLint level);

		/** get the free image description frm a type */
		static FREE_IMAGE_TYPE GetFreeImageType(PixelFormat const& pixel_format, int* bpp = nullptr);
		/** the free image prefered file format for a given PixelFormat */
		static FREE_IMAGE_FORMAT GetFreeImageFormat(PixelFormat const& pixel_format);

		/** load an image from a buffer */
		static FIBITMAP* LoadImageFromBuffer(Buffer<char> buffer);
		/** load an image from file (use our own implementation instead of FreeImage_LoadFromFile to provide our own error management) */
		static FIBITMAP* LoadImageFromFile(FilePathParam const& path);

		/** load multiple image from a buffer (animated gif) */
		static std::vector<FIBITMAP*> LoadMultipleImagesFromBuffer(Buffer<char> buffer, ImageAnimationDescription* anim_description = nullptr);
		/** load multiple image from a file (animated gif) */
		static std::vector<FIBITMAP*> LoadMultipleImagesFromFile(FilePathParam const& path, ImageAnimationDescription* anim_description = nullptr);
		/** extract from a multi bitmap all pages (this is a 'duplication' due to library limitation) */
		static std::vector<FIBITMAP*> GetMultiImagePages(FIMULTIBITMAP* multi_bitmap, ImageAnimationDescription* anim_description = nullptr);

		/** try to read animation meta data from an image */
		static bool GetImageAnimDescription(FIBITMAP* image, ImageAnimationDescription& result);

		/** returns true whether the image is paletted, 8 bits and its color are grays */
		static bool IsGrayscaleImage(FIBITMAP* image, bool* alpha_needed = nullptr);
		/** transform input image into a supported format whenever possible (may destroy the input in case of failure) */
		static FIBITMAP* ConvertToSupportedType(FIBITMAP* src, bool can_delete_src);

		/** get the image information from a FreeImage */
		static ImageDescription GetImageDescription(FIBITMAP* image);
		/** get pixel format corresponding to an image */
		static PixelFormat GetPixelFormat(FIBITMAP* image);

		/** copy pixels */
		static void CopyPixels(ImageDescription const& src_desc, ImageDescription& dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height, ImageTransform image_transform = ImageTransform::NO_TRANSFORM);
		/** convert image into another pixel format + central symetry if required */
		static ImageDescription ConvertPixels(ImageDescription const& src_desc, PixelFormat const& pixel_format, char* conversion_buffer, ImageTransform image_transform = ImageTransform::NO_TRANSFORM);

		/** create a ImageTexture with DWORD alignment requirements with a given buffer */
		static ImageDescription GetImageDescriptionForAlignedTexture(PixelFormat const& pixel_format, int width, int height, char* buffer);
		/** compute the memory requirement for a texture with DWORD aligned rows texture */
		static int GetMemoryRequirementForAlignedTexture(PixelFormat const& pixel_format, int width, int height);

		/** Generate an image from lambda */
		template<typename T, typename GENERATOR>
		static FIBITMAP* GenFreeImage(int width, int height, GENERATOR const& generator)
		{
			FIBITMAP* result = GenFreeImage(PixelFormat::GetPixelFormat<T>(), width, height);
			if (result != nullptr)
			{
				ImageDescription image_desc = GetImageDescription(result);
				generator(image_desc);
			}
			return result;
		}

		// Read a FIFTag with a conversion (not all type handled)
		template<typename T>
		static bool ReadFIFTag(FITAG* tag, T& result)
		{
			// early exit
			if (tag == nullptr)
				return false;
			// get the value
			void const* value_ptr = FreeImage_GetTagValue(tag);
			if (value_ptr == nullptr)
				return false;
			// get the type
			FREE_IMAGE_MDTYPE type = FreeImage_GetTagType(tag);
			// try to make a conversion
#define CHAOS_IMAGETOOLS_READTAG(enumtype, cpptype)\
if (type == enumtype)\
{\
cpptype value = *((cpptype *)value_ptr);\
result = static_cast<T>(value);\
return true;\
}
			CHAOS_IMAGETOOLS_READTAG(FIDT_BYTE, std::uint8_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_SHORT, std::uint16_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_LONG, std::uint32_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_LONG8, std::uint64_t);

			CHAOS_IMAGETOOLS_READTAG(FIDT_SBYTE, std::int8_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_SSHORT, std::int16_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_SLONG, std::int32_t);
			CHAOS_IMAGETOOLS_READTAG(FIDT_SLONG8, std::int64_t);

			CHAOS_IMAGETOOLS_READTAG(FIDT_FLOAT, float);
			CHAOS_IMAGETOOLS_READTAG(FIDT_DOUBLE, double);
#undef CHAOS_IMAGETOOLS_READTAG
			return false;
		}

		// read a FIFTag with a conversion (and a default value)
		template<typename T>
		static bool ReadFIFTag(FITAG* tag, T& result, T default_value)
		{
			if (ReadFIFTag(tag, result))
				return true;
			result = default_value;
			return false;
		}

		// Read some meta data
		template<typename T>
		static bool ReadMetaData(FIBITMAP* image, FREE_IMAGE_MDMODEL model, char const* name, T& result, T default_value = T())
		{
			FITAG* tag = nullptr;
			if (!FreeImage_GetMetadata(model, image, name, &tag))
				return false;
			if (tag == nullptr)
				return false;
			return ReadFIFTag(tag, result, default_value);
		}

#if _WIN32
		/** conversion between HBITMAP & FIBITMAP */
		static FIBITMAP* ConvertFromHBITMAP(HBITMAP hBitmap);
		/** conversion between HBITMAP & FIBITMAP */
		static HBITMAP ConvertToHBITMAP(FIBITMAP* bitmap, bool flip_vertical = true);
#endif // #if _WIN32

	};

#endif

}; // namespace chaos