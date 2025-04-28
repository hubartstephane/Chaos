namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AtlasFontInfoInputParams;
	class AtlasInputInfoBase;
	class AtlasBitmapAnimationInfoInput;
	class AtlasBitmapInfoInput;
	class AtlasCharacterInfoInput;
	class AtlasFontInfoInput;
	class AddFilesToFolderData;
	class AtlasFolderInfoInput;
	class AtlasInput;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AtlasFontInfoInputParams : when inserting AtlasFontInfoInput into AtlasInput, some glyphs are rendered into bitmaps. This controls the process
	*/

	class CHAOS_API AtlasFontInfoInputParams
	{
	public:

		// XXX : FreeType does not produce glyph of the exact requested size

		/** the characters to generate / the characters in the font bitmap */
		std::string characters;
		/** number of uniform grid cells for the bitmaps */
		glm::ivec2 grid_size = glm::ivec2(0, 0);

		/** width of the glyph */
		int glyph_width = 64;
		/** height of the glyph */
		int glyph_height = 64;

		/** the image processing to apply */
		std::vector<shared_ptr<ImageProcessor>> image_processors;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasFontInfoInputParams const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasFontInfoInputParams& dst);

	/**
	* AtlasInputInfoBase : base object for inputs
	*/

	class CHAOS_API AtlasInputInfoBase : public NamedInterface
	{
	public:

		/** register bitmap */
		void RegisterResource(FIBITMAP* bitmap, bool release);
		/** register multi bitmap */
		void RegisterResource(FIMULTIBITMAP* multi_bitmap, bool release);
		/** register font */
		void RegisterResource(FT_Library library, bool release);
		/** register face */
		void RegisterResource(FT_Face face, bool release);

	public:

		/** the atlas the input belongs to */
		class AtlasInput* atlas_input = nullptr;
	};

	/**
	* AtlasBitmapAnimationInfoInput : Describe an animation for a AtlasBitmapInfoInput
	*/

	class CHAOS_API AtlasBitmapAnimationInfoInput : public Object
	{

	public:

		/** the animation description */
		ImageAnimationDescription animation_description;
		/** the child frames of animated image (GIF) */
		std::vector<class AtlasBitmapInfoInput*> child_frames;
	};

	/**
	* AtlasBitmapInfoInput : Will produced a AtlasBitmapInfo in the final Atlas
	*/

	class CHAOS_API AtlasBitmapInfoInput : public AtlasInputInfoBase
	{
	public:

		/** the description of the bitmap */
		ImageDescription description;

		/** whether the bitmap is part of an animation */
		shared_ptr<AtlasBitmapAnimationInfoInput> animation_info;

		/** a pointer on the destination info associated */
		AtlasBitmapInfo* bitmap_output_info = nullptr;
		/** a pointer on the destination info associated */
		AtlasCharacterInfo* character_output_info = nullptr;
	};

	/**
	* AtlasCharacterInfoInput : an info in AtlasFontInfoInput. Will produced a AtlasCharacterInfo in the final Atlas
	*/

	class CHAOS_API AtlasCharacterInfoInput : public AtlasBitmapInfoInput
	{
	public:

		/** from 'CharacterMetrics' class */
		FT_Vector advance{ 0, 0 };
		/** from 'CharacterMetrics' class */
		int       bitmap_left = 0;
		/** from 'CharacterMetrics' class */
		int       bitmap_top = 0;
	};

	/**
	* AtlasFontInfoInput : this info will produced in the final Atlas a AtlasFontInfo (a set of glyphs generated from FreeType)
	*/

	class CHAOS_API AtlasFontInfoInput : public AtlasFontInfoTemplate<NoCopy<AtlasInputInfoBase>, AtlasCharacterInfoInput, std::unique_ptr<boost::mpl::_1>>
	{

	};

	/**
	* AddFilesToFolderData : structure used when inserting multiple files in a AtlasFolderInfoInput
	*/

	class CHAOS_API AddFilesToFolderData
	{
	public:

		/** constructor */
		AddFilesToFolderData() = default;
		/** constructor */
		AddFilesToFolderData(AddFilesToFolderData const& src) = default;
		/** constructor */
		AddFilesToFolderData(FilePathParam const& in_path);

		/** iterate over the directory and find directories and files */
		void SearchEntriesInDirectory();

	public:

		// the files concerned by the request
		std::vector<boost::filesystem::path> files;
		// the directories concerned by the request
		std::vector<boost::filesystem::path> directories;

		// the directories to ignore due to JSON manifest
		std::vector<boost::filesystem::path> ignore_directories;
		// the files to ignore due to JSON manifest
		std::vector<boost::filesystem::path> ignore_files;

	protected:

		/** the path of the directory */
		boost::filesystem::path directory_path;
		// whether the directory has already be processed
		bool processed_done = false;
	};


	/**
	* AtlasFolderInfoInput :  this info will produced in the final Atlas a AtlasFolderInfo
	*/

	class CHAOS_API AtlasFolderInfoInput : public AtlasFolderInfoTemplate<NoCopy<AtlasInputInfoBase>, AtlasBitmapInfoInput, AtlasFontInfoInput, AtlasFolderInfoInput, std::unique_ptr<boost::mpl::_1>>
	{
		friend class AtlasInput;
		friend class AtlasGenerator;

	public:

		/** constructor */
		AtlasFolderInfoInput() = default;

		/** insert a Folder set inside the input */
		AtlasFolderInfoInput* AddFolder(char const* name, TagType tag);

		/** insert multiple bitmap before computation */
		bool AddBitmapFilesFromDirectory(FilePathParam const& path, bool recursive);

		/** insert a bitmap before computation */
		AtlasBitmapInfoInput* AddBitmap(FilePathParam const& path, char const* name, TagType tag);
		/** insert an image inside the atlas */
		AtlasBitmapInfoInput* AddBitmap(FIBITMAP* bitmap, bool release_bitmap, char const* name, TagType tag);
		/** insert an image inside the atlas */
		AtlasBitmapInfoInput* AddBitmap(FIMULTIBITMAP* animated_bitmap, bool release_animated_bitmap, char const* name, TagType tag);

		/** Add a character set */
		AtlasFontInfoInput* AddFont(
			FilePathParam const& path,
			FT_Library library,
			bool release_library,
			char const* name,
			TagType tag,
			AtlasFontInfoInputParams const& params = AtlasFontInfoInputParams());

		/** Add a character set */
		AtlasFontInfoInput* AddFont(
			FT_Face face,
			bool release_face,
			char const* name,
			TagType tag,
			AtlasFontInfoInputParams const& params = AtlasFontInfoInputParams());

	protected:

		/** internal method to add a bitmap from file (and searching manifest) */
		AtlasBitmapInfoInput* AddBitmapFileImpl(FilePathParam const& path, char const* name, TagType tag, AddFilesToFolderData& add_data);
		/** internal method to add a bitmap whose manifest (or not) is known */
		AtlasBitmapInfoInput* AddBitmapFileWithManifestImpl(FilePathParam const& path, char const* name, TagType tag, nlohmann::json const* json_manifest, std::vector<FIBITMAP*>* images);
		/** internal method to add a bitmap or a multi bitmap */
		AtlasBitmapInfoInput* AddBitmapImpl(std::vector<FIBITMAP*> pages, char const* name, TagType tag, ImageAnimationDescription const* animation_description);

		/** internal method to insert a font file (and searching manifest)*/
		AtlasFontInfoInput* AddFontFileImpl(FilePathParam const& path, FT_Library library, bool release_library, char const* name, TagType tag, AtlasFontInfoInputParams const& params, AddFilesToFolderData& add_data);
		/** internal method to add a font whose manifest (or not) is known */
		AtlasFontInfoInput* AddFontFileWithManifestImpl(FilePathParam const& path, FT_Library library, bool release_library, char const* name, TagType tag, AtlasFontInfoInputParams const& params, nlohmann::json const* json_manifest);

		/** internal method to add a character set */
		AtlasFontInfoInput* AddFontImpl(
			FT_Library library,
			FT_Face face,
			bool release_library,
			bool release_face,
			char const* name,
			TagType tag,
			AtlasFontInfoInputParams const& params);
	};

	/**
	* AtlasInput : this hold the bitmaps / glyphs used for Atlas generation
	*/

	class CHAOS_API AtlasInput : public AtlasBaseTemplate<NoCopy<Object>, AtlasBitmapInfoInput, AtlasFontInfoInput, AtlasFolderInfoInput>
	{
		friend class AtlasInputInfoBase;
		friend class AtlasGenerator;

	public:

		using super = AtlasBaseTemplate<NoCopy<Object>, AtlasBitmapInfoInput, AtlasFontInfoInput, AtlasFolderInfoInput>;

		/** constructor */
		AtlasInput();
		/** the clear method */
		virtual void Clear() override;

		/** insert a Folder set inside the input */
		AtlasFolderInfoInput* AddFolder(char const* name, TagType tag);

		/** insert multiple bitmap before computation */
		bool AddBitmapFilesFromDirectory(FilePathParam const& path, bool recursive);

		/** insert an image from a file */
		AtlasBitmapInfoInput* AddBitmap(FilePathParam const& path, char const* name, TagType tag);
		/** insert an image inside the atlas */
		AtlasBitmapInfoInput* AddBitmap(FIBITMAP* bitmap, bool release_bitmap, char const* name, TagType tag);
		/** insert an image inside the atlas */
		AtlasBitmapInfoInput* AddBitmap(FIMULTIBITMAP* animated_bitmap, bool release_bitmap, char const* name, TagType tag);

		/** Add a character set */
		AtlasFontInfoInput* AddFont(
			FilePathParam const& path,
			FT_Library library,
			bool release_library,
			char const* name,
			TagType tag,
			AtlasFontInfoInputParams const& params = AtlasFontInfoInputParams());

		/** Add a character set */
		AtlasFontInfoInput* AddFont(
			FT_Face face,
			bool release_face,
			char const* name,
			TagType tag,
			AtlasFontInfoInputParams const& params = AtlasFontInfoInputParams());

	protected:

		/** register bitmap */
		void RegisterResource(FIBITMAP* bitmap, bool release);
		/** register multi bitmap */
		void RegisterResource(FIMULTIBITMAP* multi_bitmap, bool release);
		/** register font */
		void RegisterResource(FT_Library library, bool release);
		/** register face */
		void RegisterResource(FT_Face face, bool release);

	protected:

		/** the bitmaps to destroy */
		std::vector<bitmap_ptr> bitmaps;
		/** the multi bitmaps to destroy */
		std::vector<multibitmap_ptr> multi_bitmaps;
		/** the ft_libraries to destroy */
		std::vector<library_ptr> libraries; // XXX : order declaration of 'libraries' and 'faces' is important
		/** the ft_faces to destroy */      //       'faces' have to be destroyed first. So it must be declared last
		std::vector<face_ptr> faces;
	};


#endif

}; // namespace chaos