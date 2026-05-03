namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUTextureArraySliceRegistryEntry;
	class GPUTextureArraySliceRegistry;
	class GPUTextureArraySliceGenerator;
	class GPUTextureArraySliceGenerator_Image;

	class GPUTextureArrayGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUTextureArraySliceRegistryEntry : an entry in the registry of slices
	*/

	class CHAOS_API GPUTextureArraySliceRegistryEntry
	{
	public:

		/** the image description */
		ImageDescription description;
		/** a user data that may be used later for memory releasing */
		void* user_data = nullptr;
	};

	/**
	* GPUTextureArraySliceRegistry : utility function that serves to registry a slice into a vector
	*/

	class CHAOS_API GPUTextureArraySliceRegistry
	{
		friend class GPUTextureArrayGenerator;

	public:

		/** the method to insert one slice */
		bool InsertSlice(ImageDescription const& description, void* user_data = nullptr);
		/** gets the number of slices inserted */
		size_t GetSliceCount() const { return slices.size(); }

	protected:

		/** test whether the description is valid */
		bool IsImageSliceValid(ImageDescription const& description) const;

		/** the array that contains all slices */
		std::vector<GPUTextureArraySliceRegistryEntry> slices;
	};

	/**
	* GPUTextureArraySliceGenerator : base class for slice generators
	*/

	class CHAOS_API GPUTextureArraySliceGenerator : public Object
	{
	public:

		/** the method to override to add all slice we want */
		virtual void RegisterSlices(GPUTextureArraySliceRegistry& slice_registry) {}
		/** the method to override to release all slices */
		virtual void ReleaseSlices(GPUTextureArraySliceRegistryEntry* slices, size_t count) {}
		/** called just before the insertion into the registry */
		virtual bool PreRegister() { return true; }

	};

	/**
	* GPUTextureArraySliceGenerator_Image : a generator that use a FBITMAP, or a path
	*/

	class CHAOS_API GPUTextureArraySliceGenerator_Image : public GPUTextureArraySliceGenerator
	{
	public:

		/** constructor */
		GPUTextureArraySliceGenerator_Image(FilePathParam const& in_image_path) :
			image_path(in_image_path.GetResolvedPath()) {}

		GPUTextureArraySliceGenerator_Image(FIBITMAP* in_image, bool in_release_image) :
			image(in_image),
			release_image(in_release_image)
		{
			assert(image != nullptr);
		}

		/** destructor */
		virtual ~GPUTextureArraySliceGenerator_Image();

		/** override */
		virtual void RegisterSlices(GPUTextureArraySliceRegistry& slice_registry) override;
		/** override */
		virtual bool PreRegister() override;

	protected:

		/** path of the resource file */
		boost::filesystem::path image_path;
		/** the image */
		FIBITMAP* image = nullptr;
		/** whether the image has to be released */
		bool release_image = false;
	};

	/**
	* GPUTextureArrayGenerator : an helper class that is used to generate texture array    GL_TEXTURE_1D_ARRAY,    GL_TEXTURE_2D_ARRAY or    GL_TEXTURE_CUBE_ARRAY
	*/

	class CHAOS_API GPUTextureArrayGenerator : public GPUDeviceResourceInterface
	{
	public:

		class SliceInfo
		{
		public:

			/** first slice allocated for the generator */
			int first_slice = 0;
			/** number of slice used by the generator */
			int slice_count = 0;
		};

		class GeneratorEntry
		{
		public:
			/** the generator that will request some slices */
			shared_ptr<GPUTextureArraySliceGenerator> generator;
			/** optional where the slices are allocated */
			SliceInfo* slice_info = nullptr;
		};

		/** constructor */
		GPUTextureArrayGenerator(GPUDevice * in_device);
		/** destructor */
		virtual ~GPUTextureArrayGenerator();

		/** the insertion method (returns the slice considered) */
		bool AddGenerator(GPUTextureArraySliceGenerator* generator, SliceInfo* slice_info = nullptr);
		/** clean all generators */
		void Clean();
		/** generate the texture array */
		GPUTexture* GenTextureObject(PixelFormatMergeParams const& merge_params = {}, GenTextureParameters const& parameters = {});

	protected:

		/** internal method to generate the texture array */
		GPUTexture* GenTextureObjectHelper(GPUTextureArraySliceRegistry& slice_registry, PixelFormat const& pixel_format, int width, int height, GenTextureParameters const& parameters) const;

	protected:

		/** the registered element to generate */
		std::vector<GeneratorEntry> generators;
	};

#endif

}; // namespace chaos