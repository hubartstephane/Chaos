#include <chaos/FontTools.h>
#include <chaos/ImageTools.h>


// XXX : for Freetype, a glyph in memory is organised up to down
//   
// ----------------------------------- pen_y
//            |
//            v - bitmap_top
// Line 0       +---+
// Line 1       |   |
// Line 2       +---+
// Line 3       |   |
// Line 4       |   |
// 

namespace chaos
{
	FIBITMAP * FontTools::GenerateImage(FT_GlyphSlot glyph, PixelFormat const & pixel_format)
	{
		assert(glyph != nullptr);

		FIBITMAP * result = nullptr;

		FT_Error Err = FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL); // render the slot into a bitmap
		if (!Err)
			result = GenerateImage(glyph->bitmap, pixel_format); // convert FreeType-bitmap into FreeImage-bitmap
		return result;
	}

	class FillFontImageMetaFunc
	{
	public:

		FillFontImageMetaFunc(ImageDescription & in_dst_desc, ImageDescription & in_src_desc) :
			dst_desc(in_dst_desc),
			src_desc(in_src_desc)
		{}

		/// the dispatch function
		template<typename DST_TYPE>
		void operator()(DST_TYPE dst_color)
		{
			PixelFormat pf = PixelFormat::GetPixelFormat<DST_TYPE>();
			if (pf == dst_desc.pixel_format)
			{
				int w = dst_desc.width;
				int h = dst_desc.height;

				for (int j = 0; j < h; ++j)
				{
					DST_TYPE * d = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, 0, j);
					PixelGray const * s = ImageTools::GetPixelAddress<PixelGray>(src_desc, 0, (h - 1 - j));

					for (int i = 0; i < w; ++i) // glyph is reversed compare to what we want
						PixelConverter::Convert(d[i], s[i]);
				}
			}
		}

	protected:

		ImageDescription dst_desc;
		ImageDescription src_desc;
	};

	ImageDescription FontTools::GetImageDescription(FT_Bitmap & bitmap)
	{
		ImageDescription result;

		int w = bitmap.width;
		int h = bitmap.rows;

		result.width = w;
		result.height = h;
		result.pixel_format = PixelFormat(PixelFormat::FORMAT_GRAY);
		result.line_size = w * sizeof(PixelGray);
		result.pitch_size = bitmap.pitch;
		result.padding_size = result.pitch_size - result.line_size;
		result.data = bitmap.buffer;

		return result;
	}

	void FontTools::MakeAlphaChannelConsistent(ImageDescription & desc)
	{
		// correction of alpha channel
		if (desc.pixel_format == PixelFormat::FORMAT_RGBA_FLOAT)
		{
			int w = desc.width;
			int h = desc.height;

			for (int j = 0; j < h; ++j)
			{
				PixelRGBAFloat * d = ImageTools::GetPixelAddress<PixelRGBAFloat>(desc, 0, j);
				for (int i = 0; i < w; ++i)
					d[i].A = d[i].R;
			}
		}
		else if (desc.pixel_format == PixelFormat::FORMAT_RGBA)
		{
			int w = desc.width;
			int h = desc.height;

			for (int j = 0; j < h; ++j)
			{
				PixelBGRA * d = ImageTools::GetPixelAddress<PixelBGRA>(desc, 0, j);
				for (int i = 0; i < w; ++i)
					d[i].A = d[i].R;
			}
		}
	}

	FIBITMAP * FontTools::GenerateImage(FT_Bitmap & bitmap, PixelFormat const & pixel_format)
	{
		if (!pixel_format.IsValid())
			return nullptr;

		int mode = bitmap.pixel_mode;
		if (mode != FT_PIXEL_MODE_GRAY) // other format not supported yet
			return nullptr;

		int w = bitmap.width;
		int h = bitmap.rows;

		FIBITMAP * result = ImageTools::GenFreeImage(pixel_format, w, h);
		if (result != nullptr)
		{
			ImageDescription src_desc = FontTools::GetImageDescription(bitmap);
			ImageDescription dst_desc = ImageTools::GetImageDescription(result);

			FillFontImageMetaFunc fill_func_map(dst_desc, src_desc);

			boost::mpl::for_each<PixelTypes>(fill_func_map);
			MakeAlphaChannelConsistent(dst_desc);
		}

		return result;
	}


	FT_BitmapGlyph FontTools::GetBitmapGlyph(FT_Face face, char c, bool accept_notfound_glyph)
	{
		FT_Error error = 0;

		// get glyph index
		int glyph_index = FT_Get_Char_Index(face, c);
		if (c != 0 && (glyph_index == 0 && !accept_notfound_glyph))
			return nullptr;

		// load the glyph
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
			return nullptr;

		FT_Glyph glyph;

		// copy the glyph
		error = FT_Get_Glyph(face->glyph, &glyph);
		if (error || glyph == nullptr)
			return nullptr;

		// convert to a bitmap if necessary
		if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
			if (error)
				return nullptr;
		}

		return (FT_BitmapGlyph)glyph;
	}

	std::map<char, FontTools::CharacterBitmapGlyph> FontTools::GetGlyphCacheForString(FT_Face face, char const * str)
	{
		assert(face != nullptr);
		assert(str != nullptr);

		std::map<char, CharacterBitmapGlyph> result;

		// generate the glyph bitmap : for all characters
		for (int i = 0; str[i] != 0; ++i)
		{
			char c = str[i];

			if (result.find(c) != result.end()) // already in cache
				continue;

			FT_BitmapGlyph bitmap_glyph = GetBitmapGlyph(face, c, false);
			if (bitmap_glyph == nullptr)
				continue;

			result.insert(std::make_pair(c, CharacterBitmapGlyph(bitmap_glyph))); // insert into cache
		}

		// generate the glyph bitmap : for 0
		FT_BitmapGlyph bitmap_glyph = GetBitmapGlyph(face, 0, false);
		if (bitmap_glyph != nullptr)
			result.insert(std::make_pair(0, CharacterBitmapGlyph(bitmap_glyph))); // insert into cache

		return result;
	}

	FIBITMAP * FontTools::GenerateImage(FT_Face face, char c, PixelFormat const & pixel_format)
	{
		assert(face != nullptr);

		FIBITMAP * result = nullptr;

		FT_BitmapGlyph bitmap_glyph = GetBitmapGlyph(face, c, true);
		if (bitmap_glyph != nullptr)
		{
			result = GenerateImage(bitmap_glyph->bitmap, pixel_format);
			FT_Done_Glyph((FT_Glyph)bitmap_glyph);
		}
		return result;
	}

	class FillFontStringImageMetaFunc
	{
	public:

		FillFontStringImageMetaFunc(ImageDescription const & in_dst_desc, char const * in_str, std::map<char, FontTools::CharacterBitmapGlyph> const & in_glyph_cache, int in_min_x, int in_min_y) :
			dst_desc(in_dst_desc),
			str(in_str),
			glyph_cache(in_glyph_cache),    
			min_x(in_min_x),
			min_y(in_min_y)
		{}

		/// utility fonctions for blending pixels
		static void BlendPixel(PixelGray & dst, PixelGray const & src)
		{
			dst = max(dst, src);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelBGR & dst, PixelBGR const & src)
		{
			dst.R = max(dst.R, src.R);
			dst.G = max(dst.G, src.G);
			dst.B = max(dst.B, src.B);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelBGRA & dst, PixelBGRA const & src)
		{
			dst.R = max(dst.R, src.R);
			dst.G = max(dst.G, src.G);
			dst.B = max(dst.B, src.B);
			dst.A = max(dst.A, src.A);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelGrayFloat & dst, PixelGrayFloat const & src)
		{
			dst = max(dst, src);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelRGBFloat & dst, PixelRGBFloat const & src)
		{
			dst.R = max(dst.R, src.R);
			dst.G = max(dst.G, src.G);
			dst.B = max(dst.B, src.B);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelRGBAFloat & dst, PixelRGBAFloat const & src)
		{
			dst.R = max(dst.R, src.R);
			dst.G = max(dst.G, src.G);
			dst.B = max(dst.B, src.B);
			dst.A = max(dst.A, src.A);
		}
		/// utility fonctions for blending pixels
		static void BlendPixel(PixelDepthStencil & dst, PixelDepthStencil const & src)
		{
			assert(0); // should never happens
		}

		/// the dispatch function
		template<typename DST_TYPE>
		void operator()(DST_TYPE dst_color)
		{
			PixelFormat pf = PixelFormat::GetPixelFormat<DST_TYPE>();
			if (pf == dst_desc.pixel_format)
			{
				// fill the background with empty data
				ImageTools::FillImageBackground(dst_desc, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

				// copy each character glyph into buffer
				int pitch_size = dst_desc.pitch_size;

				int dst_height = dst_desc.height;

				unsigned char * dst_buffer = (unsigned char *)dst_desc.data;

				int pos_x = 0;
				int pos_y = 0;
				for (int i = 0; str[i] != 0; ++i)
				{
					char c = str[i];

					auto const it = glyph_cache.find(c);
					if (it == glyph_cache.cend())
						continue;

					FontTools::CharacterBitmapGlyph const & record = it->second;

					// get the metrics
					int w = record.width;
					int h = record.height;
					int bl = record.bitmap_left;
					int bt = record.bitmap_top;
					int avx = record.advance.x;
					int avy = record.advance.y;

					bt = -bt;

					ImageDescription src_desc = FontTools::GetImageDescription(record.bitmap_glyph->bitmap);

					// copy the glyph to dest buffer : invert lines 
					for (int y = 0; y < h; ++y)
					{
						int delta_x = (pos_x + bl - min_x);
						int delta_y = (pos_y + bt - min_y + y);

						DST_TYPE * d = ImageTools::GetPixelAddress<DST_TYPE>(dst_desc, delta_x, dst_height - 1 - delta_y);
						PixelGray const * s = ImageTools::GetPixelAddress<PixelGray>(src_desc, 0, y);

						for (int x = 0; x < w; ++x) // glyph is reversed compare to what we want
						{
							DST_TYPE p;
							PixelConverter::Convert(p, s[x]);
							BlendPixel(d[x], p);
						}
#if 0
						DST_TYPE * d = (DST_TYPE*)(dst_buffer +
							sizeof(DST_TYPE) * (pos_x + bl - min_x) +
							(dst_height - 1 - (pos_y + bt - min_y + y)) * pitch_size); // compute destination address

						PixelGray const * s = (PixelGray const *)record.bitmap_glyph->bitmap.buffer + y * w; // compute source address

						for (int x = 0; x < w; ++x) // glyph is reversed compare to what we want
							PixelConverter::Convert(d[x], s[x]);
#endif
					}
					// advance the cursor
					pos_x += avx;
					pos_y += avy;
				}
			}
		}

	protected:

		ImageDescription dst_desc;

		char const * str;

		std::map<char, FontTools::CharacterBitmapGlyph> const & glyph_cache;

		int min_x;
		int min_y;
	};

	FIBITMAP * FontTools::GenerateImage(FT_Face face, char const * str, PixelFormat const & pixel_format)
	{
		assert(face != nullptr);
		assert(str != nullptr);

		FIBITMAP * result = nullptr;
		if (!pixel_format.IsValid())
			return result;

		// generate all required glyph
		std::map<char, CharacterBitmapGlyph> glyph_cache = GetGlyphCacheForString(face, str);

		// STEP 1 : compute required size
		int min_x = std::numeric_limits<int>::max();
		int max_x = std::numeric_limits<int>::min();
		int min_y = std::numeric_limits<int>::max();
		int max_y = std::numeric_limits<int>::min();

		int pos_x = 0;
		int pos_y = 0;
		for (int i = 0; str[i] != 0; ++i)
		{
			char c = str[i];

			auto const it = glyph_cache.find(c);
			if (it == glyph_cache.cend())
				continue;

			CharacterBitmapGlyph const & record = it->second;

			// get the metrics
			int w = record.width;
			int h = record.height;
			int bl = record.bitmap_left;
			int bt = record.bitmap_top;
			int avx = record.advance.x;
			int avy = record.advance.y;

			bt = -bt;

			// compute the position of the 4 corners
			int x1 = pos_x + bl;
			int x2 = pos_x + bl + w;
			int y1 = pos_y + bt;
			int y2 = pos_y + bt + h;

			// update min/max X,Y
			min_x = min(min_x, min(x1, x2));
			min_y = min(min_y, min(y1, y2));
			max_x = max(max_x, max(x1, x2));
			max_y = max(max_y, max(y1, y2));

			// advance the cursor
			pos_x += avx;
			pos_y += avy;
		}

		// STEP 2 : draw the string
		int required_width = max_x - min_x;
		int required_height = max_y - min_y;

		result = ImageTools::GenFreeImage(pixel_format, required_width, required_height);
		if (result != nullptr)
		{
			ImageDescription dst_desc = ImageTools::GetImageDescription(result);

			FillFontStringImageMetaFunc fill_func_map(dst_desc, str, glyph_cache, min_x, min_y);

			boost::mpl::for_each<PixelTypes>(fill_func_map);
		}

		// release the glyphs
		for (auto glyph_entry : glyph_cache)
			FT_Done_Glyph((FT_Glyph)glyph_entry.second.bitmap_glyph);

		return result;
	}

};


