namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class SkyBoxImageType;

	class SkyBoxSingleDisposition;
	class SkyBoxImages;
	class SkyBoxTools;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API SkyBoxImageType : int
	{
		IMAGE_LEFT = 0,
		IMAGE_RIGHT = 1,
		IMAGE_TOP = 2,
		IMAGE_BOTTOM = 3,
		IMAGE_FRONT = 4,
		IMAGE_BACK = 5,
		IMAGE_SINGLE = 6,

		IMAGE_FIRST_INDEX = IMAGE_LEFT,
		IMAGE_LAST_INDEX = IMAGE_SINGLE
	};

	/**
	* SkyBoxSingleDisposition : the disposition of each individual face inside the single image
	*/

	class CHAOS_API SkyBoxSingleDisposition
	{
	public:

		/** the disposition for an horizontal image */
		static SkyBoxSingleDisposition const HorizontalDisposition;
		/** the disposition for an vertical image */
		static SkyBoxSingleDisposition const VerticalDisposition;

		/** returns the position according to an image type */
		glm::ivec3 GetPositionAndFlags(SkyBoxImageType image_type) const;

		/** the number of aligned images horizontally */
		int image_count_horiz = 0;
		/** the number of aligned images vertically */
		int image_count_vert = 0;

		/** the position for the images */
		glm::ivec3 image_position[6];
	};

	class CHAOS_API SkyBoxImages
	{
	public:

		/** constructor */
		SkyBoxImages() = default;
		/** copy constructor */
		SkyBoxImages(SkyBoxImages const& other) = delete;
		/** move constructor */
		SkyBoxImages(SkyBoxImages&& other) noexcept;
		/** destructor */
		~SkyBoxImages();

		/** Get skybox size */
		int GetSkyBoxSize() const;
		/** Get skybox size for a single image (returns -1 if not a good candidate i.e 4/3) */
		static int GetSingleImageSize(FIBITMAP* image);
		/** Get skybox size for a multiple image (returns -1 if not a good candidate i.e square) */
		static int GetMultipleImageSize(FIBITMAP* image);

		/** Get skybox size for a single image (returns -1 if not a good candidate i.e 4/3) */
		static int GetSingleImageSize(ImageDescription const& image_description);
		/** Get skybox size for a multiple image (returns -1 if not a good candidate i.e square) */
		static int GetMultipleImageSize(ImageDescription const& image_description);

		/** Method to clean the skybox */
		void Release();
		/** Transform a 6 images box into a one image box */
		SkyBoxImages ToMultipleImages() const;
		/** Transform a single image box into a six images box */
		SkyBoxImages ToSingleImage(bool bHorizontal, glm::vec4 const& fill_color, PixelFormatMergeParams const& merge_params = PixelFormatMergeParams()) const;

		/** returns true whether the skybox is empty */
		bool IsEmpty() const;
		/** returns true if the skybox is contained into a single image */
		bool IsSingleImage() const;
		/** returns true if the skybox is contained into six images */
		bool IsMultipleImage() const;
		/** returns true whether the 6 images skybox is completed */
		bool IsMultipleImageComplete() const;
		/** returns true whether it is a single image in horizontal direction */
		bool IsSingleImageHorizontal() const;
		/** returns true whether it is a single image in vertical direction */
		bool IsSingleImageVertical() const;

		/** get the pixel format for the skybox according to a merge param */
		PixelFormat GetMergedPixelFormat(PixelFormatMergeParams const& merge_params) const;

		/** Gets the description of one face */
		ImageDescription GetImageFaceDescription(SkyBoxImageType image_type) const;
		/** Gets position and flags for a single image */
		glm::ivec3 GetPositionAndFlags(SkyBoxImageType image_type) const;

		/** Sets an image (verify that it is a coherent call) */
		bool SetImage(SkyBoxImageType image_type, FIBITMAP* image, bool release_image);
		/** get the image of any face */
		FIBITMAP* GetImage(SkyBoxImageType face) const;

		/** no copy */
		SkyBoxImages& operator = (SkyBoxImages const& other) = delete;
		/** move operator */
		SkyBoxImages& operator = (SkyBoxImages&& other) noexcept;

	protected:

		/** compare 2 images and ensure they are compatible */
		static bool AreImageCompatible(FIBITMAP* image1, FIBITMAP* image2);

	protected:

		/** the images for the skybox */
		FIBITMAP* images[7] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		/** whether the skybox images are to be released */
		bool release_images[7] = { false, false, false, false, false, false, false };
	};


	/**
	* SkyBoxTools : deserve to load skybox files
	*/

	class CHAOS_API SkyBoxTools
	{
	public:

		/** load a skybox from a single file */
		static SkyBoxImages LoadSingleSkyBox(FilePathParam const& path);
		/** load a skybox from a multiple files */
		static SkyBoxImages LoadMultipleSkyBox(FilePathParam const& left_image, FilePathParam const& right_image, FilePathParam const& top_image, FilePathParam const& bottom_image, FilePathParam const& front_image, FilePathParam const& back_image);

	protected:

		/** utility function to load a file into one image. Incrementaly test for compatibility with previsous image */
		static bool DoLoadMultipleSkyBox_OneImage(SkyBoxImages& skybox, FilePathParam const& path, SkyBoxImageType image_index);
	};

#endif

}; // namespace chaos