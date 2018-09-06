#include <chaos/BitmapAtlasHTMLGenerator.h>
#include <chaos/MathTools.h>
#include <chaos/StringTools.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		// ========================================================================
		// BitmapAtlasHTMLGenerator function
		// ========================================================================

		tinyxml2::XMLDocument * AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const & atlas, AtlasHTMLOutputParams params)
		{
			tinyxml2::XMLDocument * result = new tinyxml2::XMLDocument();
			if (result != nullptr)
				OutputToHTMLDocument(atlas, result, params);
			return result;
		}

		bool AtlasHTMLGenerator::OutputToHTMLFile(Atlas const & atlas, FilePathParam const & path, AtlasHTMLOutputParams params)
		{
			bool result = false;

			tinyxml2::XMLDocument * doc = OutputToHTMLDocument(atlas, params);
			if (doc != nullptr)
			{
				boost::filesystem::path const & resolved_path = path.GetResolvedPath();

				result = (doc->SaveFile(resolved_path.string().c_str()) == tinyxml2::XML_SUCCESS);
				delete(doc);
			}
			return result;
		}

		void AtlasHTMLGenerator::OutputElementsToHTMLDocument(char const * folder_path, FolderInfo const * folder_info, XMLTools & html, tinyxml2::XMLElement * TABLE, tinyxml2::XMLElement * &TR, int bitmap_index, int & count)
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
				FolderInfo const * child_folder_info = folder_info->folders[i].get();
				if (child_folder_info == nullptr)
					continue;

				std::string path = StringTools::Printf("%s%s/", 
					folder_path, 
					(child_folder_info->name.length() == 0)? "??" : child_folder_info->name.c_str()
				);
				OutputElementsToHTMLDocument(path.c_str(), child_folder_info, html, TABLE, TR, bitmap_index, count);
			}
		}

		template<typename T>
		void AtlasHTMLGenerator::OutputElementsToHTMLDocument(char const * folder_path, std::vector<T> const & elements, XMLTools & html, tinyxml2::XMLElement * TABLE, tinyxml2::XMLElement * &TR, int bitmap_index, int & count)
		{
			for (auto const & info : elements) // all elements 
			{
				// keep only entries of corresponding bitmap 
				if (info.bitmap_index != bitmap_index)
					continue;

				// create a TR element if necessary
				if (TR == nullptr)
					TR = html.PushElement(TABLE, "TR");

				// output the element and its info
				tinyxml2::XMLElement * TD = html.PushElement(TR, "TD");
				tinyxml2::XMLElement * PRE = html.PushElement(TD, "PRE");
				html.PushText(PRE, "Folder : ");
				html.PushText(PRE, folder_path);
				html.PushText(PRE, "\nInfo :\n");
				html.PushText(PRE, Atlas::GetInfoString(info).c_str());

				// reset TR every 5 elements
				if (count % 5 == 4)
					TR = nullptr;
				++count;
			}
		}

		void AtlasHTMLGenerator::OutputBitmapsToHTMLDocument(FolderInfo const * folder_info, XMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
		{
			if (folder_info == nullptr)
				return;
			// draw the bitmaps and the characters
			OutputBitmapsToHTMLDocument(folder_info->bitmaps, html, SVG, bitmap_index, scale);
			for (size_t i = 0 ; i < folder_info->fonts.size() ; ++i)
				OutputBitmapsToHTMLDocument(folder_info->fonts[i].elements, html, SVG, bitmap_index, scale);
			// recursively iterate over sub folders
			for (size_t i = 0; i < folder_info->folders.size(); ++i)
				OutputBitmapsToHTMLDocument(folder_info->folders[i].get(), html, SVG, bitmap_index, scale);
		}

		template<typename T>
		void AtlasHTMLGenerator::OutputBitmapsToHTMLDocument(std::vector<T> const & elements, XMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
		{
			for (auto const & info : elements) // all elements of FontInfo or BitmapSet (i.e CharacterInfo or BitmapInfo)
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

				tinyxml2::XMLElement * RECT = html.PushElement(SVG, "RECT");
				html.PushAttribute(RECT, "x", x);
				html.PushAttribute(RECT, "y", y);
				html.PushAttribute(RECT, "width", w);
				html.PushAttribute(RECT, "height", h);
				html.PushAttribute(RECT, "style", rect_props);
			}
		}

		void AtlasHTMLGenerator::OutputBitmapFilenamesToHTMLDocument(FolderInfo const * folder_info, XMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
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
		void AtlasHTMLGenerator::OutputBitmapFilenamesToHTMLDocument(std::vector<T> const & elements, XMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
		{
			for (auto const & info : elements) // all elements of FontInfo or BitmapSet (i.e CharacterInfo or BitmapInfo)
			{
				// keep only entries of corresponding bitmap 
				if (info.bitmap_index != bitmap_index)
					continue;

				int x = MathTools::CastAndMul<int>(info.x, scale) + MathTools::CastAndMul<int>(info.width, scale * 0.5f);
				int y = MathTools::CastAndMul<int>(info.y, scale) + MathTools::CastAndMul<int>(info.height, scale * 0.5f);

				tinyxml2::XMLElement * TEXT = html.PushElement(SVG, "TEXT");
				html.PushAttribute(TEXT, "text-anchor", "middle");
				html.PushAttribute(TEXT, "x", x);
				html.PushAttribute(TEXT, "y", y);
				html.PushAttribute(TEXT, "fill", "white");

				html.PushText(TEXT, info.name.c_str());
			}
		}

		void AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const & atlas, tinyxml2::XMLDocument * doc, AtlasHTMLOutputParams params)
		{
			assert(doc != nullptr);

			XMLTools html(doc);

			tinyxml2::XMLElement * Header = html.PushElement(doc, "!DOCTYPE HTML");
			tinyxml2::XMLElement * HTML = html.PushElement(Header, "HTML");
			tinyxml2::XMLElement * BODY = html.PushElement(HTML, "BODY");

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
				tinyxml2::XMLElement * META = html.PushElement(BODY, "META");
				html.PushAttribute(META, "http-equiv", "refresh");
				html.PushAttribute(META, "content", 2);
			}

			if (params.show_header)
			{
				tinyxml2::XMLElement * P = html.PushElement(BODY, "P");
				tinyxml2::XMLElement * PRE = html.PushElement(P, "PRE");

				html.PushText(PRE, atlas.GetGeneralInformationString().c_str());
			}

			// output ordered per bitmaps
			for (int i = 0; i < atlas.atlas_count; ++i)
			{
				int w = MathTools::CastAndMul<int>(width, scale);
				int h = MathTools::CastAndMul<int>(height, scale);

				if (params.show_atlas_header)
				{
					tinyxml2::XMLElement * P = html.PushElement(BODY, "P");
					tinyxml2::XMLElement * PRE = html.PushElement(P, "PRE");
					html.PushText(PRE, atlas.GetAtlasSpaceOccupationString(i).c_str());
				}

				tinyxml2::XMLElement * TABLE = html.PushElement(BODY, "TABLE");
				html.PushAttribute(TABLE, "border", 5);
				html.PushAttribute(TABLE, "cellpadding", 10);

				tinyxml2::XMLElement * TR = nullptr;

				// enumerate recursively all fonts and bitmaps from a folder
				int count = 0;
				OutputElementsToHTMLDocument("/",  &atlas.root_folder, html, TABLE, TR, i, count);

				if (params.show_textures)
				{
					char const * texture_bgnd = "fill-opacity:1.0;fill:rgb(150,150,150);stroke-width:2;stroke:rgb(0,0,0)";

					tinyxml2::XMLElement * BR = html.PushElement(BODY, "BR");
					tinyxml2::XMLElement * SVG = html.PushElement(BODY, "SVG");

					html.PushAttribute(SVG, "width", w);
					html.PushAttribute(SVG, "height", h);

					tinyxml2::XMLElement * RECT = html.PushElement(SVG, "RECT");
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
	}; // namespace BitmapAtlas
}; // namespace chaos

