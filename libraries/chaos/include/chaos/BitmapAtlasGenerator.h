#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/NamedObject.h>
#include <chaos/FontTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/PixelFormat.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		/**
		* BitmapEntryInput : an entry in BitmapSetInput. Will produced a BitmapEntry in the final Atlas
		*/

		class BitmapEntryInput : public NamedObject
		{
		public:

			/** the description of the bitmap */
			ImageDescription description;
			/** the bitmap */
			FIBITMAP    * bitmap{ nullptr };
			/** whether the bitmap is to be destroyed at the end */
			bool          release_bitmap{ true };
			/** a pointer on the destination entry associated */
			BitmapEntry * output_entry{ nullptr };
		};

		/**
		* CharacterEntryInput : an entry in CharacterSetInput. Will produced a CharacterEntry in the final Atlas
		*/

		class CharacterEntryInput : public BitmapEntryInput
		{
		public:
			FT_Vector advance{ 0, 0 };
			int       bitmap_left{ 0 }; // from 'CharacterMetrics' class
			int       bitmap_top{ 0 };
		};

		/**
		* BitmapSetInput :  this entry will produced in the final Atlas a BitmapSet
		*/

		class BitmapSetInput : public NamedObject
		{
			friend class AtlasInput;
			friend class AtlasGenerator;

		protected:

			/** constructor is protected */
			BitmapSetInput() = default;
			/** destructor is protected */
			virtual ~BitmapSetInput();

		public:

			/** insert multiple bitmap before computation */
			bool AddBitmapFilesFromDirectory(boost::filesystem::path const & path);
			/** insert a bitmap before computation */
			bool AddBitmapFile(boost::filesystem::path const & path, char const * name);
			/** insert a bitmap before computation */
			bool AddBitmapFile(char const * filename, char const * name);
			/** insert an image inside the atlas */
			bool AddBitmap(char const * name, FIBITMAP * bitmap, bool release_bitmap = true);

		protected:

			/** the bitmaps composing the set */
			std::vector<BitmapEntryInput> elements;
		};

		/**
		* CharacterSetInputParams : when inserting CharacterSetInput into AtlasInput, some glyphs are rendered into bitmaps. This controls the process
		*/

		class CharacterSetInputParams
		{
		public:

			/** width of the glyph */
			int max_character_width{ 32 };
			/** height of the glyph */
			int max_character_height{ 32 };
		};

		/**
		* CharacterSetInput : this entry will produced in the final Atlas a CharacterSet (a set of glyphs generated from FreeType)
		*/

		class CharacterSetInput : public NamedObject
		{
			friend class AtlasInput;
			friend class AtlasGenerator;

		protected:

			/** constructor */
			CharacterSetInput() = default;
			/** destructor */
			virtual ~CharacterSetInput();

		protected:

			/** the Freetype library if appropriate */
			FT_Library library{ nullptr };
			/** the Freetype face if appropriate */
			FT_Face    face{ nullptr };
			/** should the library be released at destruction */
			bool release_library{ true };
			/** should the face be released at destruction */
			bool release_face{ true };
			/** the glyph width */
			int  max_character_width{ 0 };
			/** the glyph height */
			int  max_character_height{ 0 };
			/** the face ascender */
			int  ascender{ 0 };
			/** the face descender */
			int  descender{ 0 };
			/** the maximum size of a character */
			int  face_height{ 0 };
			/** the bitmaps composing the set */
			std::vector<CharacterEntryInput> elements;
		};

		/**
		* AtlasInput : this hold the bitmaps / glyphs used for Atlas generation
		*/

		class AtlasInput
		{
			friend class AtlasGenerator;

		public:

			/** destructor */
			virtual ~AtlasInput() { Clear(); }

			/** clearing method */
			void Clear();

			/** insert a Bitmap set inside the input */
			BitmapSetInput * AddBitmapSet(char const * name);

			/** Add a character set */
			CharacterSetInput * AddCharacterSet(
				char const * name,
				FT_Library library,
				char const * font_name,
				char const * characters = nullptr,
				bool release_library = true,
				CharacterSetInputParams const & params = CharacterSetInputParams());
			/** Add a character set */
			CharacterSetInput * AddCharacterSet(
				char const * name,
				FT_Face face,
				char const * characters = nullptr,
				bool release_face = true,
				CharacterSetInputParams const & params = CharacterSetInputParams());

		protected:

			/** internal method to add a character set */
			CharacterSetInput * AddCharacterSetImpl(
				char const * name,
				FT_Library library,
				FT_Face face,
				char const * characters,
				bool release_library,
				bool release_face,
				CharacterSetInputParams const & params);

		protected:

			/** the bitmap sets */
			std::vector<BitmapSetInput *> bitmap_sets;
			/** the character sets */
			std::vector<CharacterSetInput *> character_sets;
		};

		/**
		* AtlasGeneratorParams : parameters used when generating an atlas
		*/

		class AtlasGeneratorParams
		{
		public:

			/** contructor */
			AtlasGeneratorParams(int in_width = 0, int in_height = 0, int in_padding = 0, PixelFormatMergeParams const & in_merge_params = PixelFormatMergeParams()) :
				atlas_width(in_width),
				atlas_height(in_height),
				atlas_padding(in_padding),
				merge_params(in_merge_params) {}

			/** whether we have to use power of 2 values */
			bool force_power_of_2{ true };
			/** whether we have to use square bitmap */
			bool force_square{ true };
			/** the width of an atlas bitmap */
			int atlas_width{ 0 };
			/** the height of an atlas bitmap */
			int atlas_height{ 0 };
			/** the max width of an atlas bitmap (if resized). 0 = no limit */
			int atlas_max_width{ 0 };
			/** the max height of an atlas bitmap (if resized). 0 = no limit */
			int atlas_max_height{ 0 };
			/** some padding for the bitmap : should be even */
			int atlas_padding{ 0 };
			/** parameters for merging different pixel format */
			PixelFormatMergeParams merge_params;
		};

		/**
		* Rectangle : a class to represents rectangles
		*/

		class Rectangle
		{
		public:
			/** the top-left corner of the rectangle */
			int x;
			/** the top-left corner of the rectangle */
			int y;
			/** the size of the rectangle */
			int width;
			/** the size of the rectangle */
			int height;
			/** equality test */
			bool operator == (Rectangle const & src) const
			{
				return (x == src.x) && (y == src.y) && (width == src.width) && (height == src.height);
			}
			/** returns true whenever big does fully contains this */
			bool IsFullyInside(Rectangle const & big) const;
			/** returns true whenever there is an intersection between this and big */
			bool IsIntersecting(Rectangle const & big) const;
		};

		/**
		* AtlasGenerator :
		*   each time a BitmapEntry is inserted, the space is split along 4 axis
		*   this creates a grid of points that serve to new positions for inserting next entries ...
		*   it select the best position as the one that minimize space at left, right, top and bottom
		*/

		class AtlasGenerator
		{
			/** an definition is a set of vertical and horizontal lines that split the space */
			class AtlasDefinition
			{
			public:
				std::vector<int> split_x;
				std::vector<int> split_y;
			};

			/** an utility class used to reference all entries in input */
			using BitmapEntryInputVector = std::vector<BitmapEntryInput *>;

		public:

			/** make destructor virtual */
			virtual ~AtlasGenerator() = default;
			/** compute all BitmapEntry positions */
			bool ComputeResult(AtlasInput & in_input, Atlas & in_ouput, AtlasGeneratorParams const & in_params = AtlasGeneratorParams());
			/** returns a vector with all generated bitmaps (to be deallocated after usage) */
			std::vector<unique_bitmap_ptr> GenerateBitmaps(BitmapEntryInputVector const & entries, PixelFormat const & final_pixel_format) const;
			/** create an atlas from a directory into another directory */
			static bool CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, AtlasGeneratorParams const & in_params = AtlasGeneratorParams());

		protected:

			/** clear the results */
			void Clear();
			/** returns the box for the atlas */
			Rectangle GetAtlasRectangle() const;
			/** add padding to a rectangle */
			Rectangle AddPadding(Rectangle const & r) const;
			/** returns the rectangle corresponding to the BitmapEntry */
			Rectangle GetRectangle(BitmapEntry const & entry) const;

			/** fill the entries of the atlas from input (collect all input entries) */
			void FillAtlasEntriesFromInput(BitmapEntryInputVector & result);
			/** test whether there is an intersection between each pair of Entries in an atlas */
			bool EnsureValidResults(BitmapEntryInputVector const & result, std::ostream & stream = std::cout) const;
			/** test whether rectangle intersects with any of the entries */
			bool HasIntersectingEntry(BitmapEntryInputVector const & entries, int bitmap_index, Rectangle const & r) const;

			/** the effective function to do the computation */
			bool DoComputeResult(BitmapEntryInputVector const & entries);
			/** an utility function that gets a score for a rectangle */
			float GetAdjacentSurface(BitmapEntryInput const & entry, AtlasDefinition const & atlas_def, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const;
			/** returns the position (if any) in an atlas withe the best score */
			float FindBestPositionInAtlas(BitmapEntryInputVector const & entries, BitmapEntryInput const & entry, AtlasDefinition const & atlas_def, int & x, int & y) const;
			/** insert an integer in a vector. keep it ordered */
			void InsertOrdered(std::vector<int> & v, int value);
			/** insert a bitmap in an atlas definition */
			void InsertBitmapInAtlas(BitmapEntry & entry, AtlasDefinition & atlas_def, int x, int y);

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
			AtlasInput  * input{ nullptr };
			/** the result */
			Atlas       * output{ nullptr };
			/** all definitions */
			std::vector<AtlasDefinition> atlas_definitions;
		};
	};
};
