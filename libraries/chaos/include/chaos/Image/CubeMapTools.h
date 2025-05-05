namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class CubeMapImageType;
	enum class CubeMapFaceType;
	enum class CubeMapSingleImageLayoutType;

	class CubeMapSingleImageLayout;
	class CubeMapImages;
	class CubeMapTools;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * CubeMapSingleImageLayoutType: supported layout for single image cubemap
	 */

	enum class CubeMapSingleImageLayoutType : int
	{
		Horizontal,
		Vertical
	};

	/**
	 * CubeMapFaceType: the faces of the cube
	 */

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
	 * CubeMapSingleImageLayoutFaceInfo: information for a face sub-image in some layout
	 */

	class CHAOS_API CubeMapSingleImageLayoutFaceInfo
	{
	public:

		/** the face concerned */
		CubeMapImageType face = CubeMapImageType::ImageLeft;
		/** the position of the face in the whole image */
		glm::ivec2 position = {0, 0};
		/** the transformation to be applyed */
		ImageTransform transform = ImageTransform::NO_TRANSFORM;
	};

	/**
	* CubeMapSingleImageLayout: describes face sub-images, for a single image cube map
	*/

	class CHAOS_API CubeMapSingleImageLayout
	{
	public:

		/** gets a layout by type */
		static CubeMapSingleImageLayout const * GetLayout(CubeMapSingleImageLayoutType layout_type);

		/** returns the face information */
		CubeMapSingleImageLayoutFaceInfo GetFaceInfo(CubeMapImageType image_type) const;

	public:

		/** the number of aligned images horizontally */
		int horizontal_image_count = 0;
		/** the number of aligned images vertically */
		int vertical_image_count = 0;
		/** the information concerning each face */
		std::array<CubeMapSingleImageLayoutFaceInfo, 6> face_info;
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
		CubeMapImages ToSingleImage(CubeMapSingleImageLayoutType layout_type, glm::vec4 const& fill_color, PixelFormatMergeParams const& merge_params = PixelFormatMergeParams()) const;

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

		/** gets the description of one face */
		ImageDescription GetImageFaceDescription(CubeMapImageType image_type) const;
		/** gets information for a face in a single image mode */
		CubeMapSingleImageLayoutFaceInfo GetSingleImageLayoutFaceInfo(CubeMapImageType image_type) const;

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