#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/NamedObject.h>
#include <chaos/FilePath.h>
#include <chaos/ImageTools.h>

namespace chaos
{

	namespace BitmapAtlas
	{
		/**
		* BitmapGridAnimationInfo : some bitmaps represent a uniform grid of individual animation frames
		*/

		class BitmapGridAnimationInfo
		{
		public:

			/** the size of the grid */
			glm::ivec2 grid_size;
			/** the last images that are not filled */
			int skip_lasts = 0;

		public:

			/** parsing the the name to extract the grid numbers */
			static bool ParseFromName(char const * name, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
			/** utility method */
			static bool ParseFromNameReadGridX(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
			/** utility method */
			static bool ParseFromNameReadGridY(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
			/** utility method */
			static bool ParseFromNameReadGridSkip(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
			/** utility method */
			static bool ParseDigitReverse(char const * str, int & start);
		};

		/**
		* ObjectBase : base object 
		*/

		class ObjectBase : public NamedObject
		{

		};

		/**
		* BitmapInfo : represents an Base Bitmap info in the atlas
		*/

		class BitmapInfo : public ObjectBase
		{
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
		};

		/**
		* CharacterInfo : represents a Character info in the atlas. Contained in a FontInfo. It is a BitmapInfo with additionnal information
		*/

		class CharacterInfo : public BitmapInfo
		{
		public:
			FT_Vector advance{ 0, 0 };
			int       bitmap_left = 0; // from 'CharacterMetrics' class
			int       bitmap_top = 0;
		};

		/**
		* FolderInfo : contains bitmpas, font and other folders
		*/

		class FolderInfo : public ObjectBase
		{
		public:

			/** destroy the content of the folder */
			void Clear();

			/** gets a bitmap info by its name */
			BitmapInfo const * GetBitmapInfo(char const * name) const;
			/** gets a bitmap info by its tag */
			BitmapInfo const * GetBitmapInfo(TagType tag) const;

			/** gets a font info by its name */
			FontInfo const * GetFontInfo(char const * name) const;
			/** gets a font info by its tag */
			FontInfo const * GetFontInfo(TagType tag) const;

			/** gets a folder info by its name */
			FolderInfo const * GetFolderInfo(char const * name) const;
			/** gets a folder info by its tag */
			FolderInfo const * GetFolderInfo(TagType tag) const;

		public:

			/** the sub folders contained in this folder */
			std::vector<std::unique_ptr<FolderInfo>> folders;
			/** the bitmaps contained in this folder */
			std::vector<BitmapInfo> bitmaps;
			/** the fonts contained in this folder */
			std::vector<FontInfo> fonts;
		};










		/**
		* BitmapSet : this is a named group of Bitmaps (BitmapInfo)
		*/

		class BitmapSet : public NamedObject
		{
			friend class AtlasBase;

		public:

			/** gets an info by its name */
			BitmapInfo const * GetInfo(char const * name) const;
			/** gets an info by its tag */
			BitmapInfo const * GetInfo(TagType tag) const;

			/** the bitmap contained in the bitmap set */
			std::vector<BitmapInfo> elements;
		};

		/**
		* FontInfo : this is a named group of Characters (CharacterInfo)
		*/

		class FontInfo : public NamedObject
		{
			friend class AtlasBase;

		public:

			/** gets an info by its name */
			CharacterInfo const * GetInfo(char const * name) const;
			/** gets an info by its tag */
			CharacterInfo const * GetInfo(TagType tag) const;

			/** the max bitmap size in the set */
			int max_character_width = 0;
			/** the max bitmap size in the set */
			int max_character_height = 0;
			/** the face ascender */
			int ascender = 0;
			/** the face descender */
			int descender = 0;
			/** the maximum height of a glyph */
			int face_height = 0;
			/** the glyph contained in the character set */
			std::vector<CharacterInfo> elements;
		};

		/**
		* Some JSON utility functions
		*/

		void SaveIntoJSON(NamedObject const & info, nlohmann::json & json_entry);

		void LoadFromJSON(NamedObject & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(BitmapInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(BitmapInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(CharacterInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(CharacterInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(BitmapSet const & info, nlohmann::json & json_entry);

		void LoadFromJSON(BitmapSet & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(FontInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(FontInfo & info, nlohmann::json const & json_entry);

		/**
		* AtlasBase : base class for Atlas and TextureArrayAtlas
		*/

		class AtlasBase : public ReferencedObject
		{

			friend class AtlasGenerator;
			friend class AtlasHTMLGenerator;
			friend class TextureArrayAtlas;

		public:

			/** the destructor */
			virtual ~AtlasBase() = default;
			/** the clearing method */
			virtual void Clear();

			/** gets a bitmap info by its name */
			BitmapInfo const * GetBitmapInfo(char const * name) const;
			/** gets a bitmap info by its tag */
			BitmapInfo const * GetBitmapInfo(TagType tag) const;

			/** gets a font info by its name */
			FontInfo const * GetFontInfo(char const * name) const;
			/** gets a font info by its tag */
			FontInfo const * GetFontInfo(TagType tag) const;

			/** gets a folder info by its name */
			FolderInfo const * GetFolderInfo(char const * name) const;
			/** gets a folder info by its tag */
			FolderInfo const * GetFolderInfo(TagType tag) const;

			/** get the number of bitmap to hold the atlas */
			size_t GetBitmapCount() const { return atlas_count; }
			/** get the size of bitmaps composing the atlas */
			glm::ivec2 GetAtlasDimension() const { return dimension; }

			/** returns the used surface for a bitmap */
			float ComputeSurface(int bitmap_index) const;

			/** display information */
			void OutputInfo(std::ostream & stream) const;
			/** display information about one named element */
			static void OutputInfo(NamedObject const & info, std::ostream & stream);
			/** display information about one bitmap info */
			static void OutputInfo(BitmapInfo const & info, std::ostream & stream);
			/** display information about one character info */
			static void OutputInfo(CharacterInfo const & info, std::ostream & stream);

			/** display information */
			std::string GetInfoString() const;
			/** display information about one named object */
			static std::string GetInfoString(NamedObject const & info);
			/** display information about one bitmap info */
			static std::string GetInfoString(BitmapInfo const & info);
			/** display information about one character info */
			static std::string GetInfoString(CharacterInfo const & info);

			/** returns the bitmap sets contained in the atlas */
			std::vector<std::unique_ptr<BitmapSet>> const & GetBitmapSets() const { return bitmap_sets; }
			/** returns the character set contained in the atlas */
			std::vector<std::unique_ptr<FontInfo>> const & GetFontInfos() const { return font_infos; }

		protected:

			/** get a string with the general information */
			std::string GetGeneralInformationString() const;
			/** get a string with the surface occupation of all atlas */
			std::string GetAtlasSpaceOccupationString() const;
			/** get a string with the surface occupation of one atlas */
			std::string GetAtlasSpaceOccupationString(int bitmap_index) const;
			/** display the surface occupation of all atlas */
			void OutputAtlasSpaceOccupation(std::ostream & stream = std::cout) const;
			/** display the surface occupation of all atlas */
			void OutputAtlasSpaceOccupation(int bitmap_index, std::ostream & stream = std::cout) const;
			/** display the general information if the atlas */
			void OutputGeneralInformation(std::ostream & stream = std::cout) const;

		protected:

			/** atlas count */
			int atlas_count = 0;
			/** atlas dimension */
			glm::ivec2 dimension = glm::ivec2(0, 0);
			/** the root folder */
			FolderInfo root_folder;
		};

		/**
		* Atlas : a group of bitmap and characters, ordered in named set (BitmapSet & FontInfo)
		*/

		class Atlas : public AtlasBase
		{
			friend class AtlasGenerator;

		public:

			/** the clearing method */
			virtual void Clear() override;

			/** get the number of bitmap to hold the atlas */
			size_t GetBitmapCount() const { return bitmaps.size(); }

			/** load an atlas from an index file */
			bool LoadAtlas(FilePathParam const & path);
			/** function to save the results */
			bool SaveAtlas(FilePathParam const & path) const;

			/** returns the bitmaps contained in the atlas */
			std::vector<bitmap_ptr> const & GetBitmaps() const { return bitmaps; }

		protected:

			/** load an atlas from a json object */
			bool LoadAtlas(nlohmann::json const & json, boost::filesystem::path const & src_dir);
			/** function to save bitmaps */
			bool SaveAtlasBitmaps(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;
			/** function to save contents */
			bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;
			/** split a filename into DIRECTORY, INDEX_FILENAME and BITMAP prefix path */
			void SplitFilename(FilePathParam const & path, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const;
			/** get the name of a bitmap */
			boost::filesystem::path GetBitmapFilename(FREE_IMAGE_FORMAT image_format, boost::filesystem::path bitmap_filename, int index) const;

		protected:

			/** the bitmaps contained in the atlas */
			std::vector<bitmap_ptr> bitmaps;
		};
	}; // namespace BitmapAtlas
}; // namespace chaos
