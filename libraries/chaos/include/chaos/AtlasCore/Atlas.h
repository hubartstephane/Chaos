namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class AtlasBitmapLayout;
	class AtlasCharacterLayout;
	class AtlasBitmapAnimationInfo;
	class AtlasBitmapInfo;
	class AtlasCharacterInfo;
	class AtlasFontInfo;
	class AtlasFolderInfo;
	class AtlasBase;
	class Atlas;

	template<typename PARENT_CLASS, typename CHARACTER_INFO_TYPE, typename META_WRAPPER_TYPE>
	class AtlasFontInfoTemplate;

	template<typename PARENT_CLASS, typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE, typename META_WRAPPER_TYPE>
	class AtlasFolderInfoTemplate;

	template<typename PARENT_CLASS, typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE>
	class AtlasBaseTemplate;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		CHAOS_DEFINE_LOG(BitmapAtlasLog, "BitmapAtlas")

	/**
	* AtlasBitmapLayout : were the bitmap lies in the atlas
	*/

	class CHAOS_API AtlasBitmapLayout
	{
	public:

		/** returns whether the bitmap index is valid */
		bool IsValid() const { return (bitmap_index >= 0); }
		/** get the texcoordinates for that bitmap */
		class ParticleTexcoords GetTexcoords() const;
		/** give a size for the bitmap depending on whether src.x.y == 0, and applying aspect ratio */
		glm::vec2 ApplyRatioToSize(glm::vec2 src) const;

	public:

		/** the atlas in which it is stored in result */
		int bitmap_index = -1;
		/** the top-left corner of the bitmap */
		int x = 0;
		/** the top-left corner of the bitmap */
		int y = 0;
		/** the size of the bitmap (beware, 2 x padding must be add for correct result) */
		int width = 0;
		/** the size of the bitmap (beware, 2 x padding must be add for correct result) */
		int height = 0;

		/** the bottom-left texture coordinate */
		glm::vec2 bottomleft_texcoord = glm::vec2(0.0f, 0.0f);
		/** the top-right texture coordinate */
		glm::vec2 topright_texcoord = glm::vec2(0.0f, 0.0f);
	};

	/**
	* AtlasCharacterLayout : were the character lies in the atlas
	*/

	class CHAOS_API AtlasCharacterLayout : public AtlasBitmapLayout
	{
	public:
		FT_Vector advance{ 0, 0 };
		int       bitmap_left = 0; // from 'CharacterMetrics' class
		int       bitmap_top = 0;
	};

	/**
	 * AtlasBitmapAnimationInfo : represents animation data inside a bitmap/character
	 */

	class CHAOS_API AtlasBitmapAnimationInfo : public Object, public ImageAnimationDescription // XXX: inheritance order is important
	{
	};

	/**
	* AtlasBitmapInfo : represents an Bitmap info in the atlas
	*/

	class CHAOS_API AtlasBitmapInfo : public AtlasBitmapLayout, public NamedInterface
	{
	public:

		/** returns true whether the bitmap contains an animation of any type */
		bool HasAnimation() const;
		/** returns true whether the bitmap contains an animation of type frame (i.e GIF) */
		bool HasFrameAnimation() const;
		/** returns true whether the bitmap contains an animation of type grid */
		bool HasGridAnimation() const;

		/** returns the animation layout with a time */
		AtlasBitmapLayout GetAnimationLayoutFromTime(float time, WrapMode mode = WrapMode::NONE) const;
		/** returns the layout for one linear frame of the animation */
		AtlasBitmapLayout GetAnimationLayout(int index, WrapMode mode = WrapMode::NONE) const;
		/** returns the layout for one grid frame of the animation */
		AtlasBitmapLayout GetAnimationLayout(glm::ivec2 const& grid_index, WrapMode mode = WrapMode::NONE) const;
		/** returns the number of frames in the animation */
		int GetAnimationImageCount() const;
		/** returns the duration of a frame in seconds */
		float GetFrameDuration() const;
		/** returns the duration of the whole animation in seconds */
		float GetAnimationDuration() const;

	protected:

		/** utility method */
		AtlasBitmapLayout DoGetFrameAnimationLayout(int index, WrapMode mode) const;
		/** utility method */
		AtlasBitmapLayout DoGetGridAnimationLayout(glm::ivec2 grid_index, WrapMode mode) const;
		/** get the effective WrapMode to use for the request */
		WrapMode GetEffectiveRequestWrapMode(WrapMode src) const;

	public:

		/** whether the bitmap is part of an animation */
		shared_ptr<AtlasBitmapAnimationInfo> animation_info;
	};

	/**
	* AtlasCharacterInfo : represents a Character info in the atlas. Contained in a AtlasFontInfo. It is a AtlasBitmapInfo with additionnal information
	*/

	class CHAOS_API AtlasCharacterInfo : public AtlasCharacterLayout, public NamedInterface
	{
	};

	/**
	* AtlasFontInfoTemplate : a base template for AtlasFontInfo and AtlasFontInfoInput
	*/

	template<typename PARENT_CLASS, typename CHARACTER_INFO_TYPE, typename META_WRAPPER_TYPE>
	class AtlasFontInfoTemplate : public PARENT_CLASS
	{
	public:

		using character_type = CHARACTER_INFO_TYPE;
		using meta_wrapper_type = META_WRAPPER_TYPE;
		using character_stored_type = typename boost::mpl::apply<meta_wrapper_type, character_type>::type;

		/** gets an info by name/tag */
		character_type const* GetAtlasCharacterInfo(ObjectRequest request) const
		{
			return request.FindObject(elements);
		}

	public:

		/** the max bitmap size in the set */
		int glyph_width = 0;
		/** the max bitmap size in the set */
		int glyph_height = 0;
		/** the face ascender */
		int ascender = 0;
		/** the face descender */
		int descender = 0;
		/** the maximum height of a glyph */
		int face_height = 0;

		/** the glyph contained in the character info */
		std::vector<character_stored_type> elements;
	};

	/**
	* AtlasFontInfo : this is a named group of Characters (AtlasCharacterInfo)
	*/

	class CHAOS_API AtlasFontInfo : public AtlasFontInfoTemplate<NamedInterface, AtlasCharacterInfo, boost::mpl::identity<boost::mpl::_1>>
	{
	};

	/**
	* AtlasFolderInfoTemplate : a base template for AtlasFolderInfo and AtlasFolderInfoInput
	*/

	template<typename PARENT_CLASS, typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE, typename META_WRAPPER_TYPE>
	class AtlasFolderInfoTemplate : public PARENT_CLASS
	{
	public:

		using bitmap_type = BITMAP_INFO_TYPE;
		using font_type = FONT_INFO_TYPE;
		using folder_type = FOLDER_INFO_TYPE;
		using character_type = typename font_type::character_type;

		using meta_wrapper_type = META_WRAPPER_TYPE;
		using bitmap_stored_type = typename boost::mpl::apply<meta_wrapper_type, bitmap_type>::type;
		using font_stored_type = typename boost::mpl::apply<meta_wrapper_type, font_type>::type;

#define CHAOS_IMPL_GETINFO(result_type, funcname, vector_name, constness)\
			result_type constness * funcname(ObjectRequest request, bool recursive = false) constness\
			{\
				result_type constness * result = request.FindObject(vector_name);\
				if (result != nullptr)\
					return result;\
				size_t count = folders.size();\
				for (size_t i = 0; (i < count) && (result == nullptr); ++i)\
					result = folders[i]->funcname(request, recursive);\
				return result;\
			}
		CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, BOOST_PP_EMPTY());
		CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, const);

		CHAOS_IMPL_GETINFO(font_type, GetAtlasFontInfo, fonts, BOOST_PP_EMPTY());
		CHAOS_IMPL_GETINFO(font_type, GetAtlasFontInfo, fonts, const);

		CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, BOOST_PP_EMPTY());
		CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, const);
#undef CHAOS_IMPL_GETINFO

		/** clear the content of the folder */
		void Clear()
		{
			bitmaps.clear();
			fonts.clear();
			folders.clear();
		}

	public:

		/** the sub folders contained in this folder */
		std::vector<std::unique_ptr<folder_type>> folders;
		/** the bitmaps contained in this folder */
		std::vector<bitmap_stored_type> bitmaps;
		/** the fonts contained in this folder */
		std::vector<font_stored_type> fonts;
	};

	/**
	* AtlasFolderInfo : contains bitmpas, font and other folders
	*/

	class CHAOS_API AtlasFolderInfo : public AtlasFolderInfoTemplate<NoCopy<NamedInterface>, AtlasBitmapInfo, AtlasFontInfo, AtlasFolderInfo, boost::mpl::identity<boost::mpl::_1>>
	{
	public:

		/** get all entries from the root folder */
		template<typename T, typename ...PARAMS>
		void CollectEntries(T& result, bool recursive, PARAMS && ...params)
		{
			DoCollectEntries(result, std::forward<PARAMS>(params)...);
			if (recursive)
			{
				size_t folder_count = folders.size();
				for (size_t i = 0; i < folder_count; ++i)
					folders[i]->CollectEntries(result, recursive, std::forward<PARAMS>(params)...);
			}
		}

	protected:

		void DoCollectEntries(std::vector<AtlasBitmapInfo>& result);
		/** get all entries from the root folder */
		void DoCollectEntries(std::vector<AtlasCharacterInfo>& result);
		/** get all entries from the root folder */
		void DoCollectEntries(std::vector<AtlasBitmapLayout>& result, bool collect_bitmaps = true, bool collect_characters = true);
	};

	/**
	* AtlasBaseTemplate : a base template for AtlasBase and AtlasInput
	*/
	template<typename PARENT_CLASS, typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE>
	class AtlasBaseTemplate : public PARENT_CLASS
	{
	public:

		using bitmap_type = BITMAP_INFO_TYPE;
		using font_type = FONT_INFO_TYPE;
		using folder_type = FOLDER_INFO_TYPE;
		using character_type = typename font_type::character_type;

		friend class ParticleTextGenerator::GeneratorData;

		/** clear method */
		virtual void Clear()
		{
			root_folder.Clear();
		}

		/** get the root folder */
		folder_type* GetRootFolder()
		{
			return &root_folder;
		}
		/** get the root folder */
		folder_type const* GetRootFolder() const
		{
			return &root_folder;
		}

		/** gets a bitmap info by its name/tag */
		bitmap_type* GetBitmapInfo(ObjectRequest request, bool recursive = false)
		{
			return root_folder.GetBitmapInfo(request, recursive);
		}
		bitmap_type const* GetBitmapInfo(ObjectRequest request, bool recursive = false) const
		{
			return root_folder.GetBitmapInfo(request, recursive);
		}
		/** gets a font info by its name/tag */
		font_type* GetAtlasFontInfo(ObjectRequest request, bool recursive = false)
		{
			return root_folder.GetAtlasFontInfo(request, recursive);
		}
		font_type const* GetAtlasFontInfo(ObjectRequest request, bool recursive = false) const
		{
			return root_folder.GetAtlasFontInfo(request, recursive);
		}
		/** gets a folder info by its name/tag */
		folder_type* GetFolderInfo(ObjectRequest request, bool recursive = false)
		{
			return root_folder.GetFolderInfo(request, recursive);
		}
		folder_type const* GetFolderInfo(ObjectRequest request, bool recursive = false) const
		{
			return root_folder.GetFolderInfo(request, recursive);
		}

	protected:

		/** the root folder */
		folder_type root_folder;
	};

	/**
	* AtlasBase : base class for Atlas and GPUAtlas
	*/

	class CHAOS_API AtlasBase : public AtlasBaseTemplate<Object, AtlasBitmapInfo, AtlasFontInfo, AtlasFolderInfo>
	{

		friend class AtlasGenerator;
		friend class AtlasHTMLGenerator;
		friend class GPUAtlasGenerator;

	public:

		/** the destructor */
		virtual ~AtlasBase() = default;
		/** the clearing method */
		virtual void Clear();

		/** get the number of bitmap to hold the atlas */
		size_t GetBitmapCount() const { return atlas_count; }
		/** get the size of bitmaps composing the atlas */
		glm::ivec2 GetAtlasDimension() const { return dimension; }

		/** returns the used surface for a bitmap */
		float ComputeSurface(int bitmap_index) const;

		/** get all entries from the root folder */
		template<typename T, typename ...PARAMS>
		void CollectEntries(T& result, bool recursive, PARAMS && ...params)
		{
			root_folder.CollectEntries(result, recursive, std::forward<PARAMS>(params)...);
		}

	protected:

		/** utility function to get the surface used in a bitmap */
		float DoComputeSurface(int bitmap_index, AtlasFolderInfo const* folder_info) const;

	protected:

		/** atlas count */
		int atlas_count = 0;
		/** atlas dimension */
		glm::ivec2 dimension = glm::ivec2(0, 0);
	};

	/**
	* Atlas : a group of bitmap and characters, ordered in named set (BitmapSet & AtlasFontInfo)
	*/

	class CHAOS_API Atlas : public AtlasBase
	{
		friend class AtlasGenerator;

	public:

		/** the clearing method */
		virtual void Clear() override;

		/** get the number of bitmap to hold the atlas */
		size_t GetBitmapCount() const { return bitmaps.size(); }

		/** load an atlas from an index file */
		bool LoadAtlas(FilePathParam const& path);
		/** function to save the results */
		bool SaveAtlas(FilePathParam const& path) const;

		/** returns the bitmaps contained in the atlas */
		std::vector<bitmap_ptr> const& GetBitmaps() const { return bitmaps; }

	protected:

		/** load an atlas from a json object */
		bool LoadAtlas(nlohmann::json const* json, boost::filesystem::path const& src_dir);
		/** function to save bitmaps */
		bool SaveAtlasBitmaps(boost::filesystem::path const& target_dir, boost::filesystem::path const& index_filename, boost::filesystem::path const& bitmap_filename) const;
		/** function to save contents */
		bool SaveAtlasIndex(boost::filesystem::path const& target_dir, boost::filesystem::path const& index_filename, boost::filesystem::path const& bitmap_filename) const;
		/** split a filename into DIRECTORY, INDEX_FILENAME and BITMAP prefix path */
		void SplitFilename(FilePathParam const& path, boost::filesystem::path& target_dir, boost::filesystem::path& index_filename, boost::filesystem::path& bitmap_filename) const;
		/** get the name of a bitmap */
		boost::filesystem::path GetBitmapFilename(FREE_IMAGE_FORMAT image_format, boost::filesystem::path bitmap_filename, int index) const;

	protected:

		/** the bitmaps contained in the atlas */
		std::vector<bitmap_ptr> bitmaps;
	};

	/**
	* Some JSON utility functions
	*/

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapAnimationInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapAnimationInfo& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapLayout const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapLayout& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasCharacterLayout const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasCharacterLayout& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapInfo& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasCharacterInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasCharacterInfo& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasFolderInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasFolderInfo& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasFontInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasFontInfo& dst);

#endif

}; // namespace chaos