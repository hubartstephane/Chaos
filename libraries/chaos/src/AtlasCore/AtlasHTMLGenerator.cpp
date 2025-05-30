#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// AtlasHTMLGenerator function
	// ========================================================================

	tinyxml2::XMLDocument* AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const& atlas, AtlasHTMLOutputParams params) const
	{
		tinyxml2::XMLDocument* result = new tinyxml2::XMLDocument();
		if (result != nullptr)
			OutputToHTMLDocument(atlas, result, params);
		return result;
	}

	bool AtlasHTMLGenerator::OutputToHTMLFile(Atlas const& atlas, FilePathParam const& path, AtlasHTMLOutputParams params) const
	{
		bool result = false;

		tinyxml2::XMLDocument* doc = OutputToHTMLDocument(atlas, params);
		if (doc != nullptr)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();

			result = (doc->SaveFile(resolved_path.string().c_str()) == tinyxml2::XML_SUCCESS);
			delete(doc);
		}
		return result;
	}

	void AtlasHTMLGenerator::OutputElementsToHTMLDocument(char const* folder_path, AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* TABLE, tinyxml2::XMLElement*& TR, int bitmap_index, int& count) const
	{
		if (folder_info == nullptr)
			return;

		// display all bitmaps and characters
		OutputElementsToHTMLDocument(folder_path, folder_info->bitmaps, html, TABLE, TR, bitmap_index, count);
		for (size_t i = 0; i < folder_info->fonts.size(); ++i)
			OutputElementsToHTMLDocument(folder_path, folder_info->fonts[i].elements, html, TABLE, TR, bitmap_index, count);

		// recursively iterate of the folder
		size_t folder_count = folder_info->folders.size();
		for (size_t i = 0; i < folder_count; ++i)
		{
			AtlasFolderInfo const* child_folder_info = folder_info->folders[i].get();
			if (child_folder_info == nullptr)
				continue;

			std::string path = StringTools::Printf("%s%s/",
				folder_path,
				(strlen(child_folder_info->GetName()) == 0) ? "??" : child_folder_info->GetName()
			);
			OutputElementsToHTMLDocument(path.c_str(), child_folder_info, html, TABLE, TR, bitmap_index, count);
		}
	}

	template<typename T>
	void AtlasHTMLGenerator::OutputElementsToHTMLDocument(char const* folder_path, std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* TABLE, tinyxml2::XMLElement*& TR, int bitmap_index, int& count) const
	{
		for (auto const& info : elements) // all elements
		{
			// keep only entries of corresponding bitmap
			if (info.bitmap_index != bitmap_index)
				continue;

			// create a TR element if necessary
			if (TR == nullptr)
				TR = html.PushElement(TABLE, "TR");

			// output the element and its info
			tinyxml2::XMLElement* TD = html.PushElement(TR, "TD");
			tinyxml2::XMLElement* PRE = html.PushElement(TD, "PRE");
			html.PushText(PRE, "Folder : ");
			html.PushText(PRE, folder_path);
			html.PushText(PRE, "\nInfo :\n");
			html.PushText(PRE, GetElementInfoString(info).c_str());

			// reset TR every 5 elements
			if (count % 5 == 4)
				TR = nullptr;
			++count;
		}
	}

	void AtlasHTMLGenerator::OutputBitmapsToHTMLDocument(AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const
	{
		if (folder_info == nullptr)
			return;
		// draw the bitmaps and the characters
		OutputBitmapsToHTMLDocument(folder_info->bitmaps, html, SVG, bitmap_index, scale);
		for (size_t i = 0; i < folder_info->fonts.size(); ++i)
			OutputBitmapsToHTMLDocument(folder_info->fonts[i].elements, html, SVG, bitmap_index, scale);
		// recursively iterate over sub folders
		for (size_t i = 0; i < folder_info->folders.size(); ++i)
			OutputBitmapsToHTMLDocument(folder_info->folders[i].get(), html, SVG, bitmap_index, scale);
	}

	template<typename T>
	void AtlasHTMLGenerator::OutputBitmapsToHTMLDocument(std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const
	{
		for (auto const& info : elements) // all elements of AtlasFontInfo or BitmapSet (i.e AtlasCharacterInfo or AtlasBitmapInfo)
		{
			// keep only entries of corresponding bitmap
			if (info.bitmap_index != bitmap_index)
				continue;

			int color = 10 + (rand() % 10) * 10;

			int x = MathTools::CastAndMul<int>(info.x, scale);
			int y = MathTools::CastAndMul<int>(info.y, scale);
			int w = MathTools::CastAndMul<int>(info.width, scale);
			int h = MathTools::CastAndMul<int>(info.height, scale);

			char rect_props[1024];
			sprintf_s(rect_props, 1024, "fill-opacity:0.5;fill:rgb(%d,0,0);stroke-width:1;stroke:rgb(0,0,0)", color);

			tinyxml2::XMLElement* RECT = html.PushElement(SVG, "RECT");
			html.PushAttribute(RECT, "x", x);
			html.PushAttribute(RECT, "y", y);
			html.PushAttribute(RECT, "width", w);
			html.PushAttribute(RECT, "height", h);
			html.PushAttribute(RECT, "style", &rect_props[0]); // because the rect_props = char[1024]  and not   char const *
		}
	}

	void AtlasHTMLGenerator::OutputBitmapFilenamesToHTMLDocument(AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const
	{
		if (folder_info == nullptr)
			return;

		// draw the bitmaps and the characters
		OutputBitmapFilenamesToHTMLDocument(folder_info->bitmaps, html, SVG, bitmap_index, scale);
		for (size_t i = 0; i < folder_info->fonts.size(); ++i)
			OutputBitmapFilenamesToHTMLDocument(folder_info->fonts[i].elements, html, SVG, bitmap_index, scale);
		// recursively iterate over sub folders
		for (size_t i = 0; i < folder_info->folders.size(); ++i)
			OutputBitmapFilenamesToHTMLDocument(folder_info->folders[i].get(), html, SVG, bitmap_index, scale);
	}

	template<typename T>
	void AtlasHTMLGenerator::OutputBitmapFilenamesToHTMLDocument(std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const
	{
		for (auto const& info : elements) // all elements of AtlasFontInfo or BitmapSet (i.e AtlasCharacterInfo or AtlasBitmapInfo)
		{
			// keep only entries of corresponding bitmap
			if (info.bitmap_index != bitmap_index)
				continue;

			int x = MathTools::CastAndMul<int>(info.x, scale) + MathTools::CastAndMul<int>(info.width, scale * 0.5f);
			int y = MathTools::CastAndMul<int>(info.y, scale) + MathTools::CastAndMul<int>(info.height, scale * 0.5f);

			tinyxml2::XMLElement* TEXT = html.PushElement(SVG, "TEXT");
			html.PushAttribute(TEXT, "text-anchor", "middle");
			html.PushAttribute(TEXT, "x", x);
			html.PushAttribute(TEXT, "y", y);
			html.PushAttribute(TEXT, "fill", "white");

			html.PushText(TEXT, info.GetName());
		}
	}

	void AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const& atlas, tinyxml2::XMLDocument* doc, AtlasHTMLOutputParams params) const
	{
		assert(doc != nullptr);

		XMLTools html(doc);

		tinyxml2::XMLElement* Header = html.PushElement(doc, "!DOCTYPE HTML");
		tinyxml2::XMLElement* HTML = html.PushElement(Header, "HTML");
		tinyxml2::XMLElement* BODY = html.PushElement(HTML, "BODY");

		glm::ivec2 atlas_dimension = atlas.GetAtlasDimension();

		int width = atlas_dimension.x;
		int height = atlas_dimension.y;

		float scale = params.texture_scale;
		if (scale <= 0.0f)
		{
			if (width <= 256 || height <= 256)
				scale = 3.0f;
			else if (width <= 512 || height <= 512)
				scale = 2.0f;
		}

		if (params.auto_refresh)
		{
			tinyxml2::XMLElement* META = html.PushElement(BODY, "META");
			html.PushAttribute(META, "http-equiv", "refresh");
			html.PushAttribute(META, "content", 2);
		}

		if (params.show_header)
		{
			tinyxml2::XMLElement* P = html.PushElement(BODY, "P");
			tinyxml2::XMLElement* PRE = html.PushElement(P, "PRE");

			html.PushText(PRE, GetGeneralInformationString(atlas).c_str());
		}

		// output ordered per bitmaps
		for (int i = 0; i < atlas.atlas_count; ++i)
		{
			int w = MathTools::CastAndMul<int>(width, scale);
			int h = MathTools::CastAndMul<int>(height, scale);

			if (params.show_atlas_header)
			{
				tinyxml2::XMLElement* P = html.PushElement(BODY, "P");
				tinyxml2::XMLElement* PRE = html.PushElement(P, "PRE");
				html.PushText(PRE, GetAtlasSpaceOccupationString(atlas, i).c_str());
			}

			tinyxml2::XMLElement* TABLE = html.PushElement(BODY, "TABLE");
			html.PushAttribute(TABLE, "border", 5);
			html.PushAttribute(TABLE, "cellpadding", 10);

			tinyxml2::XMLElement* TR = nullptr;

			// enumerate recursively all fonts and bitmaps from a folder
			int count = 0;
			OutputElementsToHTMLDocument("/", &atlas.root_folder, html, TABLE, TR, i, count);

			if (params.show_textures)
			{
				char const* texture_bgnd = "fill-opacity:1.0;fill:rgb(150,150,150);stroke-width:2;stroke:rgb(0,0,0)";

				tinyxml2::XMLElement* BR = html.PushElement(BODY, "BR");
				tinyxml2::XMLElement* SVG = html.PushElement(BODY, "SVG");

				html.PushAttribute(SVG, "width", w);
				html.PushAttribute(SVG, "height", h);

				tinyxml2::XMLElement* RECT = html.PushElement(SVG, "RECT");
				html.PushAttribute(RECT, "width", w);
				html.PushAttribute(RECT, "height", h);
				html.PushAttribute(RECT, "style", texture_bgnd);

				// Display the rectangles
				OutputBitmapsToHTMLDocument(&atlas.root_folder, html, SVG, i, scale);
				// Display the filenames
				if (params.show_textures_names)
					OutputBitmapFilenamesToHTMLDocument(&atlas.root_folder, html, SVG, i, scale);
			}
		}
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasFolderInfo const& folder_info, std::ostream& stream, int indent) const
	{
		StreamTools::Whitespaces whitespaces(indent);

		stream << whitespaces << "Folder:" << '\n';
		NamedInterface const& named_info = folder_info;
		DoOutputElementInfo(named_info, stream, indent);

		// output the bitmaps in the folder
		if (folder_info.bitmaps.size() > 0)
		{
			StreamTools::Whitespaces bitmap_stream_indent(indent + INDENT_VALUE);
			stream << bitmap_stream_indent << "Bitmaps:" << '\n';
			for (AtlasBitmapInfo const& bitmap_info : folder_info.bitmaps)
				DoOutputElementInfo(bitmap_info, stream, indent + INDENT_VALUE);
		}
		// output the fonts in the folder
		if (folder_info.fonts.size() > 0)
		{
			StreamTools::Whitespaces font_stream_indent(indent + INDENT_VALUE);
			stream << font_stream_indent << "Fonts:" << '\n';
			for (AtlasFontInfo const& font_info : folder_info.fonts)
				DoOutputElementInfo(font_info, stream, indent + INDENT_VALUE);
		}
		// recursive calls
		if (folder_info.folders.size() > 0)
		{
			StreamTools::Whitespaces folder_stream_indent(indent + INDENT_VALUE);
			stream << folder_stream_indent << "Child Folders:" << '\n';

			size_t count = folder_info.folders.size();
			for (size_t i = 0; i < count; ++i)
				DoOutputElementInfo(*folder_info.folders[i], stream, indent + INDENT_VALUE);
		}
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(NamedInterface const& info, std::ostream& stream, int indent) const
	{
		StreamTools::Whitespaces whitespaces(indent);
		stream << whitespaces << "  name                  : " << info.GetName() << '\n';
		stream << whitespaces << "  tag                   : " << info.GetTag() << '\n';
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasBitmapLayout const& info, std::ostream& stream, int indent) const
	{
		StreamTools::Whitespaces whitespaces(indent);
		stream << whitespaces << "  bitmap_index          : " << info.bitmap_index << '\n';
		stream << whitespaces << "  width                 : " << info.width << '\n';
		stream << whitespaces << "  height                : " << info.height << '\n';
		stream << whitespaces << "  x                     : " << info.x << '\n';
		stream << whitespaces << "  y                     : " << info.y << '\n';
		stream << whitespaces << "  bottomleft_texcoord.x : " << info.bottomleft_texcoord.x << '\n';
		stream << whitespaces << "  bottomleft_texcoord.y : " << info.bottomleft_texcoord.y << '\n';
		stream << whitespaces << "  topright_texcoord.x   : " << info.topright_texcoord.x << '\n';
		stream << whitespaces << "  topright_texcoord.y   : " << info.topright_texcoord.y << '\n';
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasCharacterLayout const& info, std::ostream& stream, int indent) const
	{
		AtlasBitmapLayout const& bitmap_layout = info;
		DoOutputElementInfo(bitmap_layout, stream, indent);

		StreamTools::Whitespaces whitespaces(indent);
		stream << whitespaces << "  advance.x    : " << info.advance.x << '\n';
		stream << whitespaces << "  advance.y    : " << info.advance.y << '\n';
		stream << whitespaces << "  bitmap_left  : " << info.bitmap_left << '\n';
		stream << whitespaces << "  bitmap_top   : " << info.bitmap_top << '\n';
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasBitmapInfo const& info, std::ostream& stream, int indent) const
	{
		NamedInterface const& named_info = info;
		DoOutputElementInfo(named_info, stream, indent);

		AtlasBitmapLayout const& bitmap_layout = info;
		DoOutputElementInfo(bitmap_layout, stream, indent);
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasCharacterInfo const& info, std::ostream& stream, int indent) const
	{
		NamedInterface const& named_info = info;
		DoOutputElementInfo(named_info, stream, indent);

		AtlasCharacterLayout const& character_layout = info;
		DoOutputElementInfo(character_layout, stream, indent);
	}

	void AtlasHTMLGenerator::DoOutputElementInfo(AtlasFontInfo const& info, std::ostream& stream, int indent) const
	{
		NamedInterface const& named_info = info;
		DoOutputElementInfo(named_info, stream, indent);

		StreamTools::Whitespaces whitespaces(indent);
		stream << whitespaces << "  glyph_width  : " << info.glyph_width << '\n';
		stream << whitespaces << "  glyph_height : " << info.glyph_height << '\n';
		stream << whitespaces << "  ascender     : " << info.ascender << '\n';
		stream << whitespaces << "  descender    : " << info.descender << '\n';
		stream << whitespaces << "  face_height  : " << info.face_height << '\n';

		// output the charactars in the fonts
		if (info.elements.size() > 0)
		{
			stream << whitespaces << "Characters:" << '\n';
			for (AtlasCharacterInfo const& character_info : info.elements)
				DoOutputElementInfo(character_info, stream, indent + INDENT_VALUE);
		}
	}

	template<typename T>
	std::string AtlasHTMLGenerator::GetElementInfoString(T const& element) const
	{
		std::ostringstream out;
		DoOutputElementInfo(element, out);
		return out.str();
	}

	std::string AtlasHTMLGenerator::GetAtlasSpaceOccupationString(Atlas const& atlas, int bitmap_index) const
	{
		std::ostringstream stream;
		float atlas_surface = (float)(atlas.dimension.x * atlas.dimension.y);
		float atlas_used_surface = atlas.ComputeSurface(bitmap_index);
		float percent = 100.0f * atlas_used_surface / atlas_surface;

		stream << "Atlas " << bitmap_index << '\n';
		stream << "  occupation : " << percent << "%" << '\n';
		return stream.str();
	}

	std::string AtlasHTMLGenerator::GetGeneralInformationString(Atlas const& atlas) const
	{
		std::ostringstream stream;
		float atlas_surface = (float)(atlas.dimension.x * atlas.dimension.y);
		float full_surface = atlas.ComputeSurface(-1);
		int   min_atlas_count = int(std::ceil(full_surface / atlas_surface));

		stream << "Full used surface  : " << full_surface << '\n';
		stream << "Atlas surface      : " << atlas_surface << '\n';
		stream << "Best atlas count   : " << min_atlas_count << '\n';
		stream << "Actual atlas count : " << atlas.atlas_count << '\n';
		return stream.str();
	}

}; // namespace chaos

