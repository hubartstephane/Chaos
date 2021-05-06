#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace BitmapAtlas
	{
		class FontInfoInputParams;
		class ObjectBaseInput;
		class BitmapAnimationInfoInput;
		class BitmapInfoInput;
		class CharacterInfoInput;
		class FontInfoInput;
		class AddFilesToFolderData;
		class FolderInfoInput;
		class AtlasInput;

	}; // namespace BitmapAtlas

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{
	namespace BitmapAtlas
	{
		/**
		* FontInfoInputParams : when inserting FontInfoInput into AtlasInput, some glyphs are rendered into bitmaps. This controls the process
		*/

		class FontInfoInputParams
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

		bool SaveIntoJSON(nlohmann::json& json_entry, FontInfoInputParams const& src);

		bool LoadFromJSON(nlohmann::json const& json_entry, FontInfoInputParams& dst);

		/**
		* ObjectBaseInput : base object for inputs
		*/

		class ObjectBaseInput : public NamedObject
		{
		public:

			/** register bitmap */
			void RegisterResource(FIBITMAP * bitmap, bool release);
			/** register multi bitmap */
			void RegisterResource(FIMULTIBITMAP * multi_bitmap, bool release);
			/** register font */
			void RegisterResource(FT_Library library, bool release);
			/** register face */
			void RegisterResource(FT_Face face, bool release);

		public:

			/** the atlas the input belongs to */
			class AtlasInput * atlas_input = nullptr;
		};

		/**
		* BitmapAnimationInfoInput : Describe an animation for a BitmapInfoInput
		*/

		class BitmapAnimationInfoInput : public Object
		{

		public:

			/** the animation description */
			ImageAnimationDescription animation_description;
			/** the child frames of animated image (GIF) */
			std::vector<class BitmapInfoInput *> child_frames;
		};

		/**
		* BitmapInfoInput : Will produced a BitmapInfo in the final Atlas
		*/

		class BitmapInfoInput : public ObjectBaseInput
		{
		public:

			/** the description of the bitmap */
			ImageDescription description;

			/** whether the bitmap is part of an animation */
			shared_ptr<BitmapAnimationInfoInput> animation_info;

			/** a pointer on the destination info associated */
			BitmapInfo * bitmap_output_info = nullptr;
			/** a pointer on the destination info associated */
			CharacterInfo * character_output_info = nullptr;
		};

		/**
		* CharacterInfoInput : an info in FontInfoInput. Will produced a CharacterInfo in the final Atlas
		*/

		class CharacterInfoInput : public BitmapInfoInput
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
		* FontInfoInput : this info will produced in the final Atlas a FontInfo (a set of glyphs generated from FreeType)
		*/

		class FontInfoInput : public FontInfoTemplate<CharacterInfoInput, ObjectBaseInput, std::unique_ptr<boost::mpl::_1>>
		{

		};

		/**
		* AddFilesToFolderData : structure used when inserting multiple files in a FolderInfoInput
		*/

		class AddFilesToFolderData
		{
		public:

			/** constructor */
			AddFilesToFolderData() = default;
			/** constructor */
			AddFilesToFolderData(AddFilesToFolderData const & src) = default;
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
		* FolderInfoInput :  this info will produced in the final Atlas a FolderInfo
		*/

		class FolderInfoInput : public FolderInfoTemplate<BitmapInfoInput, FontInfoInput, FolderInfoInput, ObjectBaseInput, std::unique_ptr<boost::mpl::_1>>
		{
			friend class AtlasInput;
			friend class AtlasGenerator;

		public:

			/** constructor */
			FolderInfoInput() = default;

			/** insert a Folder set inside the input */
			FolderInfoInput * AddFolder(char const * name, TagType tag);

			/** insert multiple bitmap before computation */
			bool AddBitmapFilesFromDirectory(FilePathParam const & path, bool recursive);

			/** insert a bitmap before computation */
			BitmapInfoInput * AddBitmap(FilePathParam const & path, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_animated_bitmap, char const * name, TagType tag);

			/** Add a character set */
			FontInfoInput * AddFont(				
				FilePathParam const& path,
				FT_Library library,
				bool release_library,
				char const * name,
				TagType tag,
				FontInfoInputParams const & params = FontInfoInputParams());

			/** Add a character set */
			FontInfoInput * AddFont(
				FT_Face face,
				bool release_face,
				char const * name,
				TagType tag,
				FontInfoInputParams const & params = FontInfoInputParams());

		protected:

			/** internal method to add a bitmap from file (and searching manifest) */
			BitmapInfoInput * AddBitmapFileImpl(FilePathParam const & path, char const * name, TagType tag, AddFilesToFolderData& add_data);
            /** internal method to add a bitmap whose manifest (or not) is known */
            BitmapInfoInput * AddBitmapFileWithManifestImpl(FilePathParam const& path, char const* name, TagType tag, nlohmann::json const* json_manifest, std::vector<FIBITMAP*> * images);
			/** internal method to add a bitmap or a multi bitmap */
			BitmapInfoInput * AddBitmapImpl(std::vector<FIBITMAP *> pages, char const * name, TagType tag, ImageAnimationDescription const * animation_description);

			/** internal method to insert a font file (and searching manifest)*/
			FontInfoInput* AddFontFileImpl(FilePathParam const& path, FT_Library library, bool release_library, char const* name, TagType tag, FontInfoInputParams const& params, AddFilesToFolderData& add_data);
			/** internal method to add a font whose manifest (or not) is known */
			FontInfoInput* AddFontFileWithManifestImpl(FilePathParam const& path, FT_Library library, bool release_library, char const* name, TagType tag, FontInfoInputParams const& params, nlohmann::json const* json_manifest);

			/** internal method to add a character set */
			FontInfoInput * AddFontImpl(
				FT_Library library,
				FT_Face face,
				bool release_library,
				bool release_face,
				char const * name,
				TagType tag,
				FontInfoInputParams const & params);
		};

		/**
		* AtlasInput : this hold the bitmaps / glyphs used for Atlas generation
		*/

		class AtlasInput : public AtlasBaseTemplate<BitmapInfoInput, FontInfoInput, FolderInfoInput, Object>
		{
			friend class ObjectBaseInput;
			friend class AtlasGenerator;		

		public:

			/** constructor */
			AtlasInput();
			/** the clear method */
			virtual void Clear() override;

			/** insert a Folder set inside the input */
			FolderInfoInput * AddFolder(char const * name, TagType tag);

			/** insert multiple bitmap before computation */
			bool AddBitmapFilesFromDirectory(FilePathParam const & path, bool recursive);

			/** insert an image from a file */
			BitmapInfoInput * AddBitmap(FilePathParam const & path, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_bitmap, char const * name, TagType tag);

			/** Add a character set */
			FontInfoInput * AddFont(
				FilePathParam const& path,
				FT_Library library,
				bool release_library,
				char const * name,
				TagType tag,
				FontInfoInputParams const & params = FontInfoInputParams());

			/** Add a character set */
			FontInfoInput * AddFont(
				FT_Face face,
				bool release_face,
				char const * name,
				TagType tag,
				FontInfoInputParams const & params = FontInfoInputParams());

		protected:

			/** register bitmap */
			void RegisterResource(FIBITMAP * bitmap, bool release);
			/** register multi bitmap */
			void RegisterResource(FIMULTIBITMAP * multi_bitmap, bool release);
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

	}; // namespace BitmapAtlas

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
