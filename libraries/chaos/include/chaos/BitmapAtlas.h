#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/NamedObject.h>
#include <chaos/FilePath.h>
#include <chaos/ImageTools.h>

namespace chaos
{
	namespace ParticleTextGenerator 
	{
		class GeneratorData;
	};

	namespace BitmapAtlas
	{
		/**
		* BitmapGridAnimationInfo : some bitmaps represent a uniform grid of individual animation frames
		*/

		class BitmapGridAnimationInfo
		{
		public:

			/** the size of the grid */
			glm::ivec2 grid_size = glm::ivec2(0, 0);
			/** the last images that are not filled */
			int skip_lasts = 0;

		public:

			/** returns whether the animation is valid */
			bool IsEmpty() const { return (grid_size.x <= 0) || (grid_size.y <= 0); }

			/** parsing the the name to extract the grid numbers */
			static bool ParseFromName(char const * name, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);

		protected:

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
		 * BitmapAnimationInfo : represents animation data inside a bitmap/character
		 */

		class BitmapAnimationInfo : public ReferencedObject
		{
		public:

			/** get the frame of a linear animation */
			void GetFrame(size_t index) {}
			/** get the frame from a grid animation */
			void GetFrame(glm::ivec2 const & index) {}

			/** get the number of frames */
			size_t GetFrameCount() const
			{
				if (!grid_data.IsEmpty())
					return (size_t)(grid_data.grid_size.x * grid_data.grid_size.y) - grid_data.skip_lasts;
				return (size_t)child_frame_count;
			}

		public:

			/** if the animation is stored inside a grid */
			BitmapGridAnimationInfo grid_data;
			/** number of child frame (directly following the BitmapInfo */
			int child_frame_count = 0;
		};

		/**
		* BitmapInfoBase : represents an Base Bitmap info in the atlas
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
			/** whether the bitmap is a sub frame of a parent */
			bool child_bitmap = false;

			/** whether the bitmap is part of an animation */
			boost::intrusive_ptr<BitmapAnimationInfo> animation_info;
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
		* FontInfoTemplate : a base template for FontInfo and FontInfoInput
		*/

		template<typename CHARACTER_INFO_TYPE, typename PARENT_CLASS, typename META_WRAPPER_TYPE>
		class FontInfoTemplate : public PARENT_CLASS
		{
		public:

			typedef CHARACTER_INFO_TYPE character_type;
			typedef META_WRAPPER_TYPE meta_wrapper_type;
			typedef typename boost::mpl::apply<meta_wrapper_type, character_type>::type character_stored_type;

			/** gets an info by its name */
			character_type const * GetCharacterInfo(char const * name) const
			{
				return NamedObject::FindNamedObject(elements, name);
			}
			/** gets an info by its tag */
			character_type const * GetCharacterInfo(TagType tag) const
			{
				return NamedObject::FindNamedObject(elements, tag);
			}

		public:

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

			/** the glyph contained in the character info */
			std::vector<character_stored_type> elements;
		};

		/**
		* FontInfo : this is a named group of Characters (CharacterInfo)
		*/

		class FontInfo : public FontInfoTemplate<CharacterInfo, NamedObject, boost::mpl::identity<boost::mpl::_1>>
		{

		};

		/**
		* FolderInfoTemplate : a base template for FolderInfo and FolderInfoInput
		*/

		template<typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE, typename PARENT_CLASS, typename META_WRAPPER_TYPE>
		class FolderInfoTemplate : public PARENT_CLASS
		{
		public:

			typedef BITMAP_INFO_TYPE bitmap_type;
			typedef FONT_INFO_TYPE font_type;
			typedef FOLDER_INFO_TYPE folder_type;
			typedef typename font_type::character_type character_type;

			typedef META_WRAPPER_TYPE meta_wrapper_type;
			typedef typename boost::mpl::apply<meta_wrapper_type, bitmap_type>::type bitmap_stored_type;
			typedef typename boost::mpl::apply<meta_wrapper_type, font_type>::type font_stored_type;

#define CHAOS_EMPTY_TOKEN
#define CHAOS_IMPL_GETINFO(result_type, funcname, vector_name, param_type, constness)\
			result_type constness * funcname(param_type name, bool recursive = false) constness\
			{\
				result_type constness * result = FindNamedObject(vector_name, name);\
				if (result != nullptr)\
					return result;\
				size_t count = folders.size();\
				for (size_t i = 0; (i < count) && (result == nullptr); ++i)\
					result = folders[i]->funcname(name, recursive);\
				return result;\
			}
			CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, char const *, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, char const *, const);
			CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, TagType, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(bitmap_type, GetBitmapInfo, bitmaps, TagType, const);

			CHAOS_IMPL_GETINFO(font_type, GetFontInfo, fonts, char const *, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(font_type, GetFontInfo, fonts, char const *, const);
			CHAOS_IMPL_GETINFO(font_type, GetFontInfo, fonts, TagType, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(font_type, GetFontInfo, fonts, TagType, const);

			CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, char const *, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, char const *, const);
			CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, TagType, CHAOS_EMPTY_TOKEN);
			CHAOS_IMPL_GETINFO(folder_type, GetFolderInfo, folders, TagType, const);
#undef CHAOS_IMPL_GETINFO
#undef CHAOS_EMPTY_TOKEN

			/** clear the content of the folder */
			void Clear()
			{
				bitmaps.clear();
				fonts.clear();
				folders.clear();
			}

			/** get all entries from the root folder */
			void CollectEntries(std::vector<bitmap_type> * bitmap_result, std::vector<character_type> * character_result, bool convert_font_into_bitmap, bool recursive) const
			{
				assert(!convert_font_into_bitmap || character_result == nullptr); // not clear what expected if there is a font result + conversion
				assert(!convert_font_into_bitmap || bitmap_result != nullptr); // cannot convert without a result vector
				// early exit
				if (bitmap_result == nullptr && character_result == nullptr)
					return;
				// collect bitmaps
				if (bitmap_result != nullptr)
				{
					size_t bitmap_count = bitmaps.size();
					bitmap_result->reserve(bitmap_result->size() + bitmap_count);

					for (size_t i = 0; i < bitmap_count; ++i)
						bitmap_result->push_back(bitmaps[i]);
				}
				// collect fonts
				if (character_result != nullptr || (bitmap_result != nullptr && convert_font_into_bitmap))
				{
					size_t font_count = fonts.size();
					for (size_t i = 0; i < font_count; ++i)
					{
						font_type const & font_info = fonts[i];

						size_t character_count = font_info.elements.size();
						if (!convert_font_into_bitmap)
						{
							character_result->reserve(character_result->size() + character_count);
							for (size_t j = 0; j < character_count; ++j)		
								character_result->push_back(font_info.elements[j]);
						}
						else
						{
							bitmap_result->reserve(bitmap_result->size() + character_count);
							for (size_t j = 0; j < character_count; ++j)		
								bitmap_result->push_back(font_info.elements[j]);
						}
					}
				}
				// recursion
				if (recursive)
				{
					size_t folder_count = folders.size();
					for (size_t i = 0; i < folder_count; ++i)
						folders[i]->CollectEntries(bitmap_result, character_result, convert_font_into_bitmap, recursive);
				}
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
		* FolderInfo : contains bitmpas, font and other folders
		*/

		class FolderInfo : public FolderInfoTemplate<BitmapInfo, FontInfo, FolderInfo, ObjectBase, boost::mpl::identity<boost::mpl::_1>>
		{

		};

		/**
		* AtlasBaseTemplate : a base template for AtlasBase and AtlasInput
		*/
		template<typename BITMAP_INFO_TYPE, typename FONT_INFO_TYPE, typename FOLDER_INFO_TYPE, typename PARENT_CLASS>
		class AtlasBaseTemplate : public PARENT_CLASS
		{
		public:

			typedef BITMAP_INFO_TYPE bitmap_type;
			typedef FONT_INFO_TYPE   font_type;
			typedef FOLDER_INFO_TYPE folder_type;
			typedef typename font_type::character_type character_type;

			friend class ParticleTextGenerator::GeneratorData;

			/** clear method */
			virtual void Clear()
			{
				root_folder.Clear();
			}

			/** get the root folder */
			folder_type * GetRootFolder()
			{
				return &root_folder;
			}
			/** get the root folder */
			folder_type const * GetRootFolder() const
			{
				return &root_folder;
			}

			/** gets a bitmap info by its name */
			bitmap_type * GetBitmapInfo(char const * name, bool recursive = false)
			{
				return root_folder.GetBitmapInfo(name, recursive);
			}
			bitmap_type const * GetBitmapInfo(char const * name, bool recursive = false) const
			{
				return root_folder.GetBitmapInfo(name, recursive);
			}
			/** gets a bitmap info by its tag */
			bitmap_type * GetBitmapInfo(TagType tag, bool recursive = false)
			{
				return root_folder.GetBitmapInfo(tag, recursive);
			}
			bitmap_type const * GetBitmapInfo(TagType tag, bool recursive = false) const
			{
				return root_folder.GetBitmapInfo(tag, recursive);
			}
			/** gets a font info by its name */
			font_type * GetFontInfo(char const * name, bool recursive = false)
			{
				return root_folder.GetFontInfo(name, recursive);
			}
			font_type const * GetFontInfo(char const * name, bool recursive = false) const
			{
				return root_folder.GetFontInfo(name, recursive);
			}
			/** gets a font info by its tag */
			font_type * GetFontInfo(TagType tag, bool recursive = false)
			{
				return root_folder.GetFontInfo(tag, recursive);
			}
			font_type const * GetFontInfo(TagType tag, bool recursive = false) const
			{
				return root_folder.GetFontInfo(tag, recursive);
			}
			/** gets a folder info by its name */
			folder_type * GetFolderInfo(char const * name, bool recursive = false)
			{
				return root_folder.GetFolderInfo(name, recursive);
			}
			folder_type const * GetFolderInfo(char const * name, bool recursive = false) const
			{
				return root_folder.GetFolderInfo(name, recursive);
			}
			/** gets a folder info by its tag */
			folder_type * GetFolderInfo(TagType tag, bool recursive = false)
			{
				return root_folder.GetFolderInfo(tag, recursive);
			}
			folder_type const * GetFolderInfo(TagType tag, bool recursive = false) const
			{
				return root_folder.GetFolderInfo(tag, recursive);
			}

		protected:

			/** the root folder */
			folder_type root_folder;
		};

		/**
		* AtlasBase : base class for Atlas and TextureArrayAtlas
		*/

		class AtlasBase : public  AtlasBaseTemplate<BitmapInfo, FontInfo, FolderInfo, ReferencedObject>
		{

			friend class AtlasGenerator;
			friend class AtlasHTMLGenerator;
			friend class TextureArrayAtlas;

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

			/** display information */
			void OutputInfo(std::ostream & stream) const;

			/** display information */
			std::string GetInfoString() const;
			/** display information about one named object */
			static std::string GetInfoString(NamedObject const & info);
			/** display information about one bitmap info */
			static std::string GetInfoString(BitmapInfo const & info);
			/** display information about one character info */
			static std::string GetInfoString(CharacterInfo const & info);

			/** get all entries from the root folder */
			void CollectEntries(std::vector<BitmapInfo> * bitmap_result, std::vector<CharacterInfo> * character_result, bool convert_font_into_bitmap, bool recursive) const;

		protected:

			/** utility function to get the surface used in a bitmap */
			float DoComputeSurface(int bitmap_index, FolderInfo const * folder_info) const;

			/** display information */
			void DoOutputInfo(FolderInfo const & folder_info, std::ostream & stream, int indent = 0) const;
			/** display information about one named element */
			static void DoOutputInfo(NamedObject const & info, std::ostream & stream, int indent = 0);
			/** display information about one bitmap info */
			static void DoOutputInfo(BitmapInfo const & info, std::ostream & stream, int indent = 0);
			/** display information about one character info */
			static void DoOutputInfo(CharacterInfo const & info, std::ostream & stream, int indent = 0);
			/** display information about one font info */
			static void DoOutputInfo(FontInfo const & info, std::ostream & stream, int indent = 0);

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

		/**
		* Some JSON utility functions
		*/

		void SaveIntoJSON(BitmapAnimationInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(BitmapAnimationInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(BitmapInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(BitmapInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(CharacterInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(CharacterInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(FolderInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(FolderInfo & info, nlohmann::json const & json_entry);

		void SaveIntoJSON(FontInfo const & info, nlohmann::json & json_entry);

		void LoadFromJSON(FontInfo & info, nlohmann::json const & json_entry);

	}; // namespace BitmapAtlas

}; // namespace chaos
