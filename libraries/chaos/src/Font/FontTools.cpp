#include "chaos/Chaos.h"


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

	ImageDescription FontTools::GetImageDescription(FT_Bitmap & bitmap)
	{
		ImageDescription result;

		int w = bitmap.width;
		int h = bitmap.rows;

		result.width = w;
		result.height = h;
		result.pixel_format = PixelFormat(PixelFormat::Gray);
		result.line_size = w * sizeof(PixelGray);
		result.pitch_size = bitmap.pitch;
		result.padding_size = result.pitch_size - result.line_size;
		result.data = bitmap.buffer;

		return result;
	}

	/** for an image representing a glyph, ensure alpha is equal to R (should be equal to G and B too) */
	static void MakeAlphaChannelConsistent(ImageDescription & desc)
	{
		// correction of alpha channel
		if (desc.pixel_format == PixelFormat::RGBAFloat)
		{
			int w = desc.width;
			int h = desc.height;

			ImagePixelAccessor<PixelRGBAFloat> accessor(desc);

			for (int j = 0; j < h; ++j)
			{
				PixelRGBAFloat * line = &accessor(0, j);
				for (int i = 0; i < w; ++i)
					line[i].A = line[i].R;
			}
		}
		else if (desc.pixel_format == PixelFormat::BGRA)
		{
			int w = desc.width;
			int h = desc.height;

			ImagePixelAccessor<PixelBGRA> accessor(desc);

			for (int j = 0; j < h; ++j)
			{
				PixelBGRA * line = &accessor(0, j);
				for (int i = 0; i < w; ++i)
					line[i].A = line[i].R;
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

			meta::for_each<PixelTypes>([src_desc, dst_desc](auto value) -> bool
			{
				using pixel_type = typename decltype(value)::type;

				if (dst_desc.pixel_format != PixelFormat::GetPixelFormat<pixel_type>())
					return false;

				int w = dst_desc.width;
				int h = dst_desc.height;

				ImagePixelAccessor<PixelGray> src_acc(src_desc);
				ImagePixelAccessor<pixel_type> dst_acc(dst_desc);

				for (int j = 0; j < h; ++j)
				{
					pixel_type* dst_line = &dst_acc(0, j);
					PixelGray const* src_line = &src_acc(0, h - 1 - j);

					for (int i = 0; i < w; ++i) // glyph is reversed compare to what we want
						PixelConverter::Convert(dst_line[i], src_line[i]);
				}
				return true;
			});

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

	template<typename T>
	static void BlendFontGlyphPixel(T& dst, T const& src)
	{
		if constexpr (std::is_same_v<T, PixelGray> || std::is_same_v<T, PixelGrayFloat>)
		{
			dst = std::max(dst, src);
		}
		else if constexpr (std::is_same_v<T, PixelBGR> || std::is_same_v<T, PixelRGBFloat>)
		{
			dst.R = std::max(dst.R, src.R);
			dst.G = std::max(dst.G, src.G);
			dst.B = std::max(dst.B, src.B);
		}
		else if constexpr (std::is_same_v<T, PixelBGRA> || std::is_same_v<T, PixelRGBAFloat>)
		{
			dst.R = std::max(dst.R, src.R);
			dst.G = std::max(dst.G, src.G);
			dst.B = std::max(dst.B, src.B);
			dst.A = std::max(dst.A, src.A);
		}
		else
		{
			assert(0);
		}
	}

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
			min_x = std::min(min_x, std::min(x1, x2));
			min_y = std::min(min_y, std::min(y1, y2));
			max_x = std::max(max_x, std::max(x1, x2));
			max_y = std::max(max_y, std::max(y1, y2));

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

			meta::for_each<PixelTypes>([&dst_desc, &glyph_cache, min_x, min_y, str](auto value) -> bool
			{
				using pixel_type = typename decltype(value)::type;

				// skip other types
				if (dst_desc.pixel_format != PixelFormat::GetPixelFormat<pixel_type>())
					return false;

				// fill the background with empty data
				ImageTools::FillImageBackground(dst_desc, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

				// copy each character glyph into buffer
				int pitch_size = dst_desc.pitch_size;

				int dst_height = dst_desc.height;

				unsigned char* dst_buffer = (unsigned char*)dst_desc.data;

				int pos_x = 0;
				int pos_y = 0;
				for (int i = 0; str[i] != 0; ++i)
				{
					char c = str[i];

					auto const it = glyph_cache.find(c);
					if (it == glyph_cache.cend())
						continue;

					FontTools::CharacterBitmapGlyph const& record = it->second;

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
					ImagePixelAccessor<pixel_type>  dst_acc(dst_desc);
					ImagePixelAccessor<PixelGray> src_acc(src_desc);

					for (int y = 0; y < h; ++y)
					{
						int delta_x = (pos_x + bl - min_x);
						int delta_y = (pos_y + bt - min_y + y);

						pixel_type* dst_line = &dst_acc(delta_x, dst_height - 1 - delta_y);
						PixelGray const* src_line = &src_acc(0, y);

						for (int x = 0; x < w; ++x) // glyph is reversed compare to what we want
						{
							pixel_type p;
							PixelConverter::Convert(p, src_line[x]);
							BlendFontGlyphPixel(dst_line[x], p);
						}
					}
					// advance the cursor
					pos_x += avx;
					pos_y += avy;
				}
				return true;
			});
		}

		// release the glyphs
		for (auto glyph_entry : glyph_cache)
			FT_Done_Glyph((FT_Glyph)glyph_entry.second.bitmap_glyph);

		return result;
	}

};


