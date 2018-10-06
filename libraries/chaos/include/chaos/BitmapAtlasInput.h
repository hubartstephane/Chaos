#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/NamedObject.h>
#include <chaos/FontTools.h>
#include <chaos/ImageDescription.h>
#include <chaos/PixelFormat.h>
#include <chaos/FilePath.h>

namespace chaos
{
	namespace BitmapAtlas
	{

		/**
		* FontInfoInputBaseParams : base data for font generating
		*/

		class FontInfoInputBaseParams
		{
		public:

			/** the characters to generate / the characters in the font bitmap */
			std::string characters;
		};

		/**
		* FontInfoInputParams : when inserting FontInfoInput into AtlasInput, some glyphs are rendered into bitmaps. This controls the process
		*/

		class FontInfoInputParams : public FontInfoInputBaseParams
		{
		public:

			/** width of the glyph */
			int max_character_width = 32;
			/** height of the glyph */
			int max_character_height = 32;
		};

		/**
		* FontInfoBitmapParams : some fonts can be represent by a grid of characters
		*/

		class FontInfoBitmapParams : public FontInfoInputBaseParams
		{
		public:

			/** number of uniform grid cells for the bitmaps */
			glm::ivec2 size = glm::ivec2(0, 0);
		};

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

		class BitmapAnimationInfoInput : public ReferencedObject
		{

		public:

			/** if the animation is stored inside a grid */
			BitmapGridAnimationInfo grid_data;
			/** the child frames of animated image (GIF) */
			std::vector<class BitmapInfoInput *> child_frames;
			/** the animation description */
			ImageAnimationDescription animation_description;
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
			boost::intrusive_ptr<BitmapAnimationInfoInput> animation_info;

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
			BitmapInfoInput * AddBitmap(FilePathParam const & path, char const * name, TagType tag, BitmapGridAnimationInfo const * grid_animation_info = nullptr);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag, BitmapGridAnimationInfo const * grid_animation_info = nullptr);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_animated_bitmap, char const * name, TagType tag, BitmapGridAnimationInfo const * grid_animation_info = nullptr);

			/** Add a character set */
			FontInfoInput * AddFont(				
				char const * font_name,
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

			/** internal method to add a bitmap or a multi bitmap */
			BitmapInfoInput * AddBitmapImpl(std::vector<FIBITMAP *> pages, char const * name, TagType tag, BitmapGridAnimationInfo const * grid_animation_info = nullptr, ImageAnimationDescription const * anim_desc = nullptr);

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

		class AtlasInput : public AtlasBaseTemplate<BitmapInfoInput, FontInfoInput, FolderInfoInput, ReferencedObject>
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

			/** insert a bitmap before computation */
			BitmapInfoInput * AddBitmap(FilePathParam const & path, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag);
			/** insert an image inside the atlas */
			BitmapInfoInput * AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_bitmap, char const * name, TagType tag);

			/** Add a character set */
			FontInfoInput * AddFont(
				char const * font_name,
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
