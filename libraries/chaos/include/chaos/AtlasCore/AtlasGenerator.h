namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AtlasGeneratorParams;
	class AtlasRectangle;
	class AtlasGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AtlasGeneratorParams : parameters used when generating an atlas
	*/

	class CHAOS_API AtlasGeneratorParams
	{
	public:

		/** contructor */
		AtlasGeneratorParams() = default;
		/** copy contructor */
		AtlasGeneratorParams(AtlasGeneratorParams const& src) = default;
		/** contructor with initialization */
		AtlasGeneratorParams(int in_width, int in_height, int in_padding, PixelFormatMergeParams const& in_merge_params) :
			atlas_width(in_width),
			atlas_height(in_height),
			atlas_padding(in_padding),
			merge_params(in_merge_params) {}

		/** whether we have to use power of 2 values */
		bool force_power_of_2 = true;
		/** whether we have to use square bitmap */
		bool force_square = true;
		/** whether each image in the atlas should have extra border that is the duplication of the origin image (usefull for texel interpolation in shaders) */
		bool duplicate_image_border = true;
		/** the width of an atlas bitmap */
		int atlas_width = 0;
		/** the height of an atlas bitmap */
		int atlas_height = 0;
		/** the max width of an atlas bitmap (if resized). 0 = no limit */
		int atlas_max_width = 0;
		/** the max height of an atlas bitmap (if resized). 0 = no limit */
		int atlas_max_height = 0;
		/** some padding for the bitmap : should be even */
		int atlas_padding = 0;
		/** the background color */
		glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		/** parameters for merging different pixel format */
		PixelFormatMergeParams merge_params;
		/** the filters to be applyed to each bitmaps */
		AtlasInputFilterSet const* filters = nullptr;
	};

	/**
	* AtlasRectangle : a class to represents rectangles
	*/

	class CHAOS_API AtlasRectangle
	{
	public:
		/** the top-left corner of the rectangle */
		int x = 0;
		/** the top-left corner of the rectangle */
		int y = 0;
		/** the size of the rectangle */
		int width = 0;
		/** the size of the rectangle */
		int height = 0;
		/** equality test */
		bool operator == (AtlasRectangle const& src) const
		{
			return (x == src.x) && (y == src.y) && (width == src.width) && (height == src.height);
		}
		/** returns true whenever big does fully contains this */
		bool IsFullyInside(AtlasRectangle const& big) const;
		/** returns true whenever there is an intersection between this and big */
		bool IsIntersecting(AtlasRectangle const& big) const;
	};

	/**
	* AtlasGenerator :
	*   each time a AtlasBitmapInfo is inserted, the space is split along 4 axis
	*   this creates a grid of points that serve to new positions for inserting next entries ...
	*   it select the best position as the one that minimize space at left, right, top and bottom
	*/

	class CHAOS_API AtlasGenerator
	{
		/** an definition is a set of vertical and horizontal lines that split the space */
		class AtlasDefinition
		{
		public:
			unsigned int surface_sum = 0;

			std::vector<AtlasRectangle>  collision_rectangles;
			std::vector<glm::ivec2> potential_bottomleft_corners;
		};

		/** an utility class used to reference all entries in input */
		using AtlasBitmapInfoInputVector = std::vector<AtlasBitmapInfoInput*>;

	public:

		/** make destructor virtual */
		virtual ~AtlasGenerator() = default;
		/** compute all AtlasBitmapInfo positions */
		bool ComputeResult(AtlasInput const& in_input, Atlas& in_ouput, AtlasGeneratorParams const& in_params = AtlasGeneratorParams());
		/** returns a vector with all generated bitmaps (to be deallocated after usage) */
		std::vector<bitmap_ptr> GenerateBitmaps(AtlasBitmapInfoInputVector const& entries, PixelFormat const& final_pixel_format) const;
		/** create an atlas from a directory into another directory */
		static bool CreateAtlasFromDirectory(FilePathParam const& bitmaps_dir, FilePathParam const& path, bool recursive, AtlasGeneratorParams const& in_params = AtlasGeneratorParams());

	protected:

		/** clear the results */
		void Clear();
		/** returns the box for the atlas */
		AtlasRectangle GetAtlasRectangle() const;
		/** add padding to a rectangle */
		AtlasRectangle AddPadding(AtlasRectangle const& r) const;
		/** returns the rectangle corresponding to the AtlasBitmapLayout */
		AtlasRectangle GetRectangle(AtlasBitmapLayout const& layout) const;

		/** fill the entries of the atlas from input (collect all input entries) */
		void FillAtlasEntriesFromInput(AtlasBitmapInfoInputVector& result, AtlasFolderInfoInput* folder_info_input, AtlasFolderInfo* folder_info_output);
		/** test whether there is an intersection between each pair of Entries in an atlas */
		bool EnsureValidResults(AtlasBitmapInfoInputVector const& result, std::ostream& stream = std::cout) const;
		/** test whether rectangle intersects with any of the entries */
		bool HasIntersectingInfo(AtlasRectangle const& r, std::vector<AtlasRectangle> const& collision_rectangles) const;

		/** the effective function to do the computation */
		bool DoComputeResult(AtlasBitmapInfoInputVector const& entries);
		/** returns the position (if any) in an atlas withe the best score */
		float FindBestPositionInAtlas(AtlasBitmapInfoInputVector const& entries, AtlasBitmapInfoInput const& info, AtlasDefinition const& atlas_def, glm::ivec2& position) const;
		/** insert a bitmap in an atlas definition */
		void InsertAtlasBitmapLayoutInAtlas(AtlasBitmapLayout& layout, AtlasDefinition& atlas_def, glm::ivec2 const& position);

		/** an utility function that returns an array with 0.. count - 1*/
		static std::vector<size_t> CreateIndexTable(size_t count)
		{
			std::vector<size_t> result;
			result.reserve(count);
			for (size_t i = 0; i < count; ++i)
				result.push_back(i);
			return result;
		}
		/** an utility function to generate sub series from a function */
		template<typename FUNC>
		std::vector<size_t> CreateIndirectionTable(size_t count, FUNC func)
		{
			std::vector<size_t> result = CreateIndexTable(count);
			std::sort(result.begin(), result.end(), func);
			return result;
		}

	protected:

		/** the params for generation */
		AtlasGeneratorParams params;
		/** the input files */
		AtlasInput const* input = nullptr;
		/** the result */
		Atlas* output = nullptr;
		/** all definitions */
		std::vector<AtlasDefinition> atlas_definitions;
	};

	/**
	* JSON serialization
	*/

	/** load from JSON */
	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasGeneratorParams& dst);
	/** save into JSON */
	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasGeneratorParams const& src);

#endif

}; // namespace chaos