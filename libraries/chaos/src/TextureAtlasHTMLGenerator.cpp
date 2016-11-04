#include <chaos/TextureAtlasHTMLGenerator.h>

namespace chaos
{
  namespace TextureAtlasx
  {
    // ========================================================================
    // Independant function
    // ========================================================================

    /** multiply an integer with a float (two conversions) */
    static int MultDimensionMixedTypes(int a, float b)
    {
      return (int)(((float)a) * b);
    }

    // ========================================================================
    // TextureAtlasHTMLGenerator function
    // ========================================================================

    tinyxml2::XMLDocument * AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const & atlas, AtlasHTMLOutputParams params)
    {
      tinyxml2::XMLDocument * result = new tinyxml2::XMLDocument();
      if (result != nullptr)
        OutputToHTMLDocument(atlas, result, params);
      return result;
    }

    bool AtlasHTMLGenerator::OutputToHTMLFile(Atlas const & atlas, boost::filesystem::path const & path, AtlasHTMLOutputParams params)
    {
      return OutputToHTMLFile(atlas, path.string().c_str());
    }

    bool AtlasHTMLGenerator::OutputToHTMLFile(Atlas const & atlas, char const * filename, AtlasHTMLOutputParams params)
    {
      assert(filename != nullptr);

      bool result = false;

      tinyxml2::XMLDocument * doc = OutputToHTMLDocument(atlas, params);
      if (doc != nullptr)
      {
        result = (doc->SaveFile(filename) == tinyxml2::XML_SUCCESS);
        delete(doc);
      }
      return result;
    }

    template<typename T>
    void AtlasHTMLGenerator::OutputElementsToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * TABLE, tinyxml2::XMLElement * &TR, int bitmap_index, int & count)
    {
      for (auto const * element : elements) // iterate over CharacterSet or BitmapSet
      {
        for (auto const & entry : element->elements) // all elements of CharacterSet or BitmapSet (i.e CharacterEntry or BitmapEntry)
        { 
          // keep only entries of corresponding bitmap 
          if (entry.bitmap_index != bitmap_index)
            continue;

          // create a TR element if necessary
          if (TR == nullptr)
            TR = html.PushElement(TABLE, "TR"); 

          // output the element and its entry
          tinyxml2::XMLElement * TD  = html.PushElement(TR, "TD");
          tinyxml2::XMLElement * PRE = html.PushElement(TD, "PRE");
					html.PushText(PRE, "Set");
          html.PushText(PRE, Atlas::GetInfoString(*element).c_str());
					html.PushText(PRE, "Entry");
          html.PushText(PRE, Atlas::GetInfoString(entry).c_str());

          // reset TR every 5 elements
          if (count % 5 == 4) 
            TR = nullptr;
          ++count;
        }
      }
    }

    template<typename T>
    void AtlasHTMLGenerator::OutputBitmapsToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
    {
      for (auto const * element : elements) // iterate over CharacterSet or BitmapSet
      {
        for (auto const & entry : element->elements) // all elements of CharacterSet or BitmapSet (i.e CharacterEntry or BitmapEntry)
        {
          // keep only entries of corresponding bitmap 
          if (entry.bitmap_index != bitmap_index)
            continue;

          int color = 10 + (rand() % 10) * 10;

          int x = MultDimensionMixedTypes(entry.x, scale);
          int y = MultDimensionMixedTypes(entry.y, scale);
          int w = MultDimensionMixedTypes(entry.width, scale);
          int h = MultDimensionMixedTypes(entry.height, scale);

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
    }

    template<typename T>
    void AtlasHTMLGenerator::OutputBitmapFilenamesToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale)
    {
      for (auto const * element : elements) // iterate over CharacterSet or BitmapSet
      {
        for (auto const & entry : element->elements) // all elements of CharacterSet or BitmapSet (i.e CharacterEntry or BitmapEntry)
        {
          // keep only entries of corresponding bitmap 
          if (entry.bitmap_index != bitmap_index)
            continue;

          int x = MultDimensionMixedTypes(entry.x, scale) + MultDimensionMixedTypes(entry.width, scale * 0.5f);
          int y = MultDimensionMixedTypes(entry.y, scale) + MultDimensionMixedTypes(entry.height, scale * 0.5f);

          tinyxml2::XMLElement * TEXT = html.PushElement(SVG, "TEXT");
          html.PushAttribute(TEXT, "text-anchor", "middle");
          html.PushAttribute(TEXT, "x", x);
          html.PushAttribute(TEXT, "y", y);
          html.PushAttribute(TEXT, "fill", "white");

          html.PushText(TEXT, entry.name.c_str());
        }
      }
    }

    void AtlasHTMLGenerator::OutputToHTMLDocument(Atlas const & atlas, tinyxml2::XMLDocument * doc, AtlasHTMLOutputParams params)
    {
      assert(doc != nullptr);

      HTMLTools html(doc);

      tinyxml2::XMLElement * Header = html.PushElement(doc, "!DOCTYPE HTML");
      tinyxml2::XMLElement * HTML   = html.PushElement(Header, "HTML");
      tinyxml2::XMLElement * BODY   = html.PushElement(HTML, "BODY");

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
      for (size_t i = 0; i < atlas.bitmaps.size(); ++i)
      {
        int w = MultDimensionMixedTypes(width, scale);
        int h = MultDimensionMixedTypes(height, scale);

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

        // enumerate all BitmapEntry and CharacterEntry using given bitmap
        int count = 0;
        OutputElementsToHTMLDocument(atlas.bitmap_sets, html, TABLE, TR, i, count);
				OutputElementsToHTMLDocument(atlas.character_sets, html, TABLE, TR, i, count);

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
          OutputBitmapsToHTMLDocument(atlas.bitmap_sets, html, SVG, i, scale);
					OutputBitmapsToHTMLDocument(atlas.character_sets, html, SVG, i, scale);
          // Display the filenames
          if (params.show_textures_names)
          {
						OutputBitmapFilenamesToHTMLDocument(atlas.bitmap_sets, html, SVG, i, scale);
						OutputBitmapFilenamesToHTMLDocument(atlas.character_sets, html, SVG, i, scale);
          }
        }
      }
    }
  };

























  // ========================================================================
  // Independant function
  // ========================================================================

  /** multiply an integer with a float (two conversions) */
  static int MultDimensionMixedTypes(int a, float b)
  {
    return (int)(((float)a) * b);
  }

  // ========================================================================
  // TextureAtlasHTMLGenerator function
  // ========================================================================

  tinyxml2::XMLDocument * TextureAtlasHTMLGenerator::OutputToHTMLDocument(TextureAtlas const & atlas, TextureAtlasHTMLOutputParams params)
  {
    tinyxml2::XMLDocument * result = new tinyxml2::XMLDocument();
    if (result != nullptr)
      OutputToHTMLDocument(atlas, result, params);
    return result;
  }

  bool TextureAtlasHTMLGenerator::OutputToHTMLFile(TextureAtlas const & atlas, boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params)
  {
    return OutputToHTMLFile(atlas, path.string().c_str());
  }

  bool TextureAtlasHTMLGenerator::OutputToHTMLFile(TextureAtlas const & atlas, char const * filename, TextureAtlasHTMLOutputParams params)
  {
    assert(filename != nullptr);

    bool result = false;

    tinyxml2::XMLDocument * doc = OutputToHTMLDocument(atlas, params);
    if (doc != nullptr)
    {
      result = (doc->SaveFile(filename) == tinyxml2::XML_SUCCESS);
      delete(doc);
    }
    return result;
  }


  void TextureAtlasHTMLGenerator::OutputToHTMLDocument(TextureAtlas const & atlas, tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params)
  {
    assert(doc != nullptr);

    HTMLTools html(doc);

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

    int id = 0;
    for (size_t i = 0; i < atlas.GetAtlasCount(); ++i)
    {
      int w = MultDimensionMixedTypes(width, scale);
      int h = MultDimensionMixedTypes(height, scale);

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

      int count = 0;
      for (size_t j = 0; j < atlas.entries.size(); ++j)
      {
        TextureAtlasEntry const & entry = atlas.entries[j];
        if (entry.atlas != i)
          continue;

        if (TR == nullptr)
          TR = html.PushElement(TABLE, "TR");

        // first element of the line
        tinyxml2::XMLElement * TD = html.PushElement(TR, "TD");
        tinyxml2::XMLElement * PRE = html.PushElement(TD, "PRE");
        html.PushText(PRE, atlas.GetTextureInfoString(entry).c_str());

        if (count % 5 == 4)
          TR = nullptr;
        ++count;
      }

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

        for (auto & entry : atlas.entries)
        {
          ++id;
          if (entry.atlas != i)
            continue;

          int color = 10 + (id % 10) * 10;

          int x = MultDimensionMixedTypes(entry.x, scale);
          int y = MultDimensionMixedTypes(entry.y, scale);
          int w = MultDimensionMixedTypes(entry.width, scale);
          int h = MultDimensionMixedTypes(entry.height, scale);

          char rect_props[1024];
          sprintf_s(rect_props, 1024, "fill-opacity:0.5;fill:rgb(%d,0,0);stroke-width:1;stroke:rgb(0,0,0)", color);

          tinyxml2::XMLElement * RECT = html.PushElement(SVG, "RECT");
          html.PushAttribute(RECT, "x", x);
          html.PushAttribute(RECT, "y", y);
          html.PushAttribute(RECT, "width", w);
          html.PushAttribute(RECT, "height", h);
          html.PushAttribute(RECT, "style", rect_props);
        }

        // display the filenames
        if (params.show_textures_names)
        {
          for (auto & entry : atlas.entries)
          {
            if (entry.atlas != i)
              continue;

            int x = MultDimensionMixedTypes(entry.x, scale) + MultDimensionMixedTypes(entry.width, scale * 0.5f);
            int y = MultDimensionMixedTypes(entry.y, scale) + MultDimensionMixedTypes(entry.height, scale * 0.5f);

            tinyxml2::XMLElement * TEXT = html.PushElement(SVG, "TEXT");
            html.PushAttribute(TEXT, "text-anchor", "middle");
            html.PushAttribute(TEXT, "x", x);
            html.PushAttribute(TEXT, "y", y);
            html.PushAttribute(TEXT, "fill", "white");

            html.PushText(TEXT, entry.name.c_str());
          }
        }
      }
    }
  }
};

