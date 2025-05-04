namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class CubeMapImageType;
	enum class CubeMapFaceType;

	class CubeMapSingleDisposition;
	class CubeMapImages;
	class CubeMapTools;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION


	enum class CHAOS_API CubeMapFaceType : int // values are important for OpenGL cubeface transfert
	{
		PositiveX = 0,
		NegativeX = 1,
		PositiveY = 2,
		NegativeY = 3,
		PositiveZ = 4,
		NegativeZ = 5
	};

	/**
	 * CubeMapImageType: the possible different image that compose a cubemap
	 */

	enum class CHAOS_API CubeMapImageType : int
	{

		ImageLeft   = 0,
		ImageRight  = 1,
		ImageTop    = 2,
		ImageBottom = 3,
		ImageFront  = 4,
		ImageBack   = 5,

		ImageSingle = 6 // some cube map may be represented by a single image containing a cross. This cross represents a deployed cube map
	};

	/**
	* CubeMapSingleDisposition : the disposition of each individual face inside the single image
	*/

	class CHAOS_API CubeMapSingleDisposition
	{
	public:

		/** the disposition for an horizontal image */
		static CubeMapSingleDisposition const HorizontalDisposition;
		/** the disposition for an vertical image */
		static CubeMapSingleDisposition const VerticalDisposition;

		/** returns the position according to an image type */
		glm::ivec3 GetPositionAndFlags(CubeMapImageType image_type) const;

		/** the number of aligned images horizontally */
		int image_count_horiz = 0;
		/** the number of aligned images vertically */
		int image_count_vert = 0;

		/** the position for the images */
		glm::ivec3 image_position[6];
	};

	class CHAOS_API CubeMapImages
	{
	public:

		/** constructor */
		CubeMapImages() = default;
		/** copy constructor */
		CubeMapImages(CubeMapImages const& other) = delete;
		/** move constructor */
		CubeMapImages(CubeMapImages&& other) noexcept;
		/** destructor */
		~CubeMapImages();

		/** Get cubemap size */
		int GetCubeMapSize() const;
		/** Get cubemap size for a single image (returns -1 if not a good candidate i.e 4/3) */
		static int GetSingleImageSize(FIBITMAP* image);
		/** Get cubemap size for a multiple image (returns -1 if not a good candidate i.e square) */
		static int GetMultipleImageSize(FIBITMAP* image);

		/** Get cubemap size for a single image (returns -1 if not a good candidate i.e 4/3) */
		static int GetSingleImageSize(ImageDescription const& image_description);
		/** Get cubemap size for a multiple image (returns -1 if not a good candidate i.e square) */
		static int GetMultipleImageSize(ImageDescription const& image_description);

		/** Method to clean the cubemap */
		void Release();
		/** Transform a 6 images box into a one image box */
		CubeMapImages ToMultipleImages() const;
		/** Transform a single image box into a six images box */
		CubeMapImages ToSingleImage(bool bHorizontal, glm::vec4 const& fill_color, PixelFormatMergeParams const& merge_params = PixelFormatMergeParams()) const;

		/** returns true whether the cubemap is empty */
		bool IsEmpty() const;
		/** returns true if the cubemap is contained into a single image */
		bool IsSingleImage() const;
		/** returns true if the cubemap is contained into six images */
		bool IsMultipleImage() const;
		/** returns true whether the 6 images cubemap is completed */
		bool IsMultipleImageComplete() const;
		/** returns true whether it is a single image in horizontal direction */
		bool IsSingleImageHorizontal() const;
		/** returns true whether it is a single image in vertical direction */
		bool IsSingleImageVertical() const;

		/** get the pixel format for the cubemap according to a merge param */
		PixelFormat GetMergedPixelFormat(PixelFormatMergeParams const& merge_params) const;

		/** Gets the description of one face */
		ImageDescription GetImageFaceDescription(CubeMapImageType image_type) const;
		/** Gets position and flags for a single image */
		glm::ivec3 GetPositionAndFlags(CubeMapImageType image_type) const;

		/** Sets an image (verify that it is a coherent call) */
		bool SetImage(CubeMapImageType image_type, FIBITMAP* image, bool release_image);
		/** get the image of any face */
		FIBITMAP* GetImage(CubeMapImageType face) const;

		/** no copy */
		CubeMapImages& operator = (CubeMapImages const& other) = delete;
		/** move operator */
		CubeMapImages& operator = (CubeMapImages&& other) noexcept;

	protected:

		/** compare 2 images and ensure they are compatible */
		static bool AreImageCompatible(FIBITMAP* image1, FIBITMAP* image2);

	protected:

		/** the images for the cubemap */
		std::array<FIBITMAP*, 7> images = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		/** whether the cubemap images are to be released */
		std::array<bool, 7> release_images = { false, false, false, false, false, false, false };
	};


	/**
	* CubeMapTools : deserve to load cubemap files
	*/

	class CHAOS_API CubeMapTools
	{
	public:

		/** load a cubemap from a single file */
		static CubeMapImages LoadSingleCubeMap(FilePathParam const& path);
		/** load a cubemap from a multiple files */
		static CubeMapImages LoadMultipleCubeMap(FilePathParam const& left_image, FilePathParam const& right_image, FilePathParam const& top_image, FilePathParam const& bottom_image, FilePathParam const& front_image, FilePathParam const& back_image);

	protected:

		/** utility function to load a file into one image. Incrementaly test for compatibility with previsous image */
		static bool DoLoadMultipleCubeMap_OneImage(CubeMapImages& cubemap, FilePathParam const& path, CubeMapImageType image_index);
	};

#endif

}; // namespace chaos