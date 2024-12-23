namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FTLibraryDeleter;
	class FTFaceDeleter;

	using library_ptr = std::unique_ptr<struct FT_LibraryRec_, FTLibraryDeleter>; // FT_Library = struct FT_LibraryRec_ *

	using face_ptr = std::unique_ptr<struct FT_FaceRec_, FTFaceDeleter>; // FT_Face = struct FT_FaceRec_

	namespace FontTools
	{
		class CharacterMetrics;
		class CharacterBitmapGlyph;

	}; // namespace FontTools

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(FontLog, "Font")

	/**
	* A smart pointer for FT_Library
	*/

	class CHAOS_API FTLibraryDeleter
	{
	public:

		/** constructor */
		FTLibraryDeleter(bool in_release = true) :
			release(in_release) {}
		/** the destruction method */
		void operator ()(FT_Library library)
		{
			if (release)
				FT_Done_FreeType(library);
		}

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

	/**
	* A smart pointer for FT_Face
	*/

	class CHAOS_API FTFaceDeleter
	{
	public:

		/** constructor */
		FTFaceDeleter(bool in_release = true) :
			release(in_release) {}
		/** the destruction method */
		void operator ()(FT_Face face)
		{
			if (release)
				FT_Done_Face(face);
		}

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

	/**
	* FontTools : provide some static methods to manipulate tools
	*/

	namespace FontTools
	{
		/** Metrics of a character */
		class CHAOS_API CharacterMetrics
		{
		public:

			/** default constructor */
			CharacterMetrics() = default;

			/** constructor with initialization */
			CharacterMetrics(FT_BitmapGlyph in_bitmap_glyph) :
				advance({ in_bitmap_glyph->root.advance.x >> 16, in_bitmap_glyph->root.advance.y >> 16 }),
				width(in_bitmap_glyph->bitmap.width),
				height(in_bitmap_glyph->bitmap.rows),
				bitmap_left(in_bitmap_glyph->left),
				bitmap_top(in_bitmap_glyph->top) {}

			FT_Vector advance{ 0, 0 };
			int       width = 0;
			int       height = 0;
			int       bitmap_left = 0;
			int       bitmap_top = 0;
		};

		/** Bitmap + Metrics of a character */
		class CHAOS_API CharacterBitmapGlyph : public CharacterMetrics
		{
		public:

			/** default constructor */
			CharacterBitmapGlyph() = default;

			/** constructor with initialization */
			CharacterBitmapGlyph(FT_BitmapGlyph in_bitmap_glyph) :
				CharacterMetrics(in_bitmap_glyph),
				bitmap_glyph(in_bitmap_glyph) {}

			/** the glyph */
			FT_BitmapGlyph bitmap_glyph = nullptr;
		};

		/** get an image description from a FT_Bitmap object */
		CHAOS_API ImageDescription GetImageDescription(FT_Bitmap const& bitmap);

		/** generate a bitmap from a glyph slot */
		CHAOS_API FIBITMAP* GenerateImage(FT_GlyphSlot glyph, PixelFormat const& pixel_format = PixelFormat::Gray);
		/** generate a bitmap from a bitmap */
		CHAOS_API FIBITMAP* GenerateImage(FT_Bitmap const & bitmap, PixelFormat const& pixel_format = PixelFormat::Gray);
		/** generate a bitmap from a font an a string */
		CHAOS_API FIBITMAP* GenerateImage(FT_Face face, char const* str, PixelFormat const& pixel_format = PixelFormat::Gray);
		/** generate a bitmap from a font an a character */
		CHAOS_API FIBITMAP* GenerateImage(FT_Face face, uint32_t charcode, PixelFormat const& pixel_format = PixelFormat::Gray);
		/** generate a bitmap glyph from a character */
		CHAOS_API FT_BitmapGlyph GetBitmapGlyph(FT_Face face, uint32_t charcode, bool accept_notfound_glyph);
		/** generate a cache with all glyph required for a string */
		CHAOS_API std::map<uint32_t, CharacterBitmapGlyph> GetGlyphCacheForString(FT_Face face, char const* str);

	}; // namespace FontTools

#endif

}; // namespace chaos