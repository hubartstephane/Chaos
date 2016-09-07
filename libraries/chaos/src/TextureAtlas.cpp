#include <chaos/TextureAtlas.h>
#include <chaos/HTMLTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/StringTools.h>
#include <chaos/ImageTools.h>

namespace chaos
{
  // ========================================================================
  // TextureAtlasData implementation
  // ========================================================================

  bool TextureAtlasData::AddTextureFilesFromDirectory(boost::filesystem::path const & p)
  {
    // enumerate the source directory
    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(p) ; it != end ; ++it)
    {
      boost::filesystem::path filename = it->path(); 
      if (boost::filesystem::is_regular_file(filename))
        AddTextureFile(filename);                           // this will reject files that are not images .. not an error
    }
    return true;
  }

  bool TextureAtlasData::AddTextureFilesFromDirectory(char const * filename)
  {
    return AddTextureFilesFromDirectory(boost::filesystem::path(filename));
  }

  bool TextureAtlasData::AddTextureFile(boost::filesystem::path const & path)
  {
    return AddTextureFile(path.string().c_str());
  }

  bool TextureAtlasData::AddTextureFile(char const * filename)
  {
    assert(filename != nullptr);

    FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
    if (image != nullptr)
    {
      if (AddImageSource(filename, image))
        return true;
      FreeImage_Unload(image);    
    }
    return false;
  }

  bool TextureAtlasData::AddImageSource(char const * filename, FIBITMAP * image)
  {
    assert(filename != nullptr);
    assert(image    != nullptr);

    TextureAtlasEntry new_texture;

    new_texture.bitmap   = image;
    new_texture.width    = (int)FreeImage_GetWidth(new_texture.bitmap);
    new_texture.height   = (int)FreeImage_GetHeight(new_texture.bitmap);
    new_texture.filename = filename;
    new_texture.x        = 0;
    new_texture.y        = 0;
    new_texture.atlas    = SIZE_MAX;
#if _DEBUG
    new_texture.order_of_insertion = SIZE_MAX;
#endif

    entries.push_back(std::move(new_texture));
    return true;
  }

  bool TextureAtlasData::AddFakeImageSource(char const * filename)
  {
    assert(filename != nullptr);

    int w = 15 * (1 + rand() % 10);
    int h = 15 * (1 + rand() % 10);

    FIBITMAP * image = FreeImage_Allocate(w, h, 32); // allocate an image
    if (image == nullptr)
      return false;

    unsigned char c = 55 + (rand() % 200);

    unsigned char color[] = { c, c, c, 255 }; // B G R A

    FreeImage_FillBackground(image, color, 0); // create a background color

    return AddImageSource(filename, image);
  }

  void TextureAtlasData::ResetResult()
  {
    for (TextureAtlasEntry & entry : entries)
    {
      entry.x     = 0;
      entry.y     = 0;
      entry.atlas = SIZE_MAX;
    }
  }

  float TextureAtlasData::ComputeSurface(size_t atlas_index, int padding) const
  {
    float p      = (float)padding;
    float result = 0.0f;
    for (TextureAtlasEntry const & entry : entries)
    {
      if (entry.atlas != atlas_index && atlas_index != SIZE_MAX)
        continue;
      result += (float)((entry.width + padding) * (entry.height + padding));
    }
    return result;
  }

  void TextureAtlasData::Clear()
  {
    for (TextureAtlasEntry & entry : entries)
      if (entry.bitmap != nullptr)
        FreeImage_Unload(entry.bitmap);
    entries.empty();
  }

  void TextureAtlasData::OutputTextureInfo(std::ostream & stream, int padding) const
  {
    for (TextureAtlasEntry const & entry : entries)
      OutputTextureInfo(entry, stream, padding);
  }

  void TextureAtlasData::OutputTextureInfo(TextureAtlasEntry const & entry, std::ostream & stream, int padding) const
  {
    stream << "Texture " << (&entry - &entries[0]) << std::endl;
    stream << "  filename : " << entry.filename    << std::endl;
    stream << "  width    : " << entry.width       << std::endl;
    stream << "  height   : " << entry.height      << std::endl;
    stream << "  x        : " << entry.x + padding << std::endl;
    stream << "  y        : " << entry.y + padding << std::endl;
    stream << "  atlas    : " << entry.atlas       << std::endl;
  }

  std::string TextureAtlasData::GetTextureInfoString(int padding) const
  {
    std::ostringstream out;
    OutputTextureInfo(out, padding);
    return out.str();
  }

  std::string TextureAtlasData::GetTextureInfoString(TextureAtlasEntry const & entry, int padding) const
  {
    std::ostringstream out;
    OutputTextureInfo(entry, out, padding);
    return out.str();
  }

  // ========================================================================
  // TextureAtlasCreatorBase implementation
  // ========================================================================

  AtlasRectangle TextureAtlasCreatorBase::GetAtlasRectangle() const
  {
    AtlasRectangle result;
    result.x      = 0;
    result.y      = 0;
    result.width  = width;
    result.height = height;
    return result;
  }
  
  size_t TextureAtlasCreatorBase::GetAtlasCount() const
  {
    return 0;
  }

  void TextureAtlasCreatorBase::Clear()
  {
    width  = 0;
    height = 0;
    data   = nullptr;
  }

  AtlasRectangle TextureAtlasCreatorBase::AddPadding(AtlasRectangle const & r) const
  {
    AtlasRectangle result = r;
    result.width  += 2 * padding;
    result.height += 2 * padding;
    return result;
  }

  bool TextureAtlasCreatorBase::ComputeResult(TextureAtlasData & in_data, int in_width, int in_height, int in_padding)    
  {
    Clear();

    width   = in_width;
    height  = in_height;
    padding = in_padding;
    data    = &in_data;

    data->ResetResult();

    // search max texture size
    int max_width  = -1;
    int max_height = -1;

    for (TextureAtlasEntry & entry : data->entries)
    {
      if (max_width < 0 || max_width < entry.width)
        max_width = entry.width;
      if (max_height < 0 || max_height < entry.height)
        max_height = entry.height;
    }

    max_width  += padding * 2;
    max_height += padding * 2;

    // ensure this can be produced inside an atlas with size restriction
    if (max_width > width || max_height > height)
      return false;

    // ensure this can be produced inside an atlas with size_restriction
    if (DoComputeResult())
    {
      bool is_valid = EnsureValid();

      if (is_valid)
      {
        OutputAtlasSpaceOccupation(std::cout);
        data->OutputTextureInfo(std::cout, padding);
        return true;
      }      
      else
        data->OutputTextureInfo(std::cout, padding);
    }    
    return false;
  }


  tinyxml2::XMLDocument * TextureAtlasCreatorBase::OutputToHTMLDocument(TextureAtlasHTMLOutputParams params) const
  {
    tinyxml2::XMLDocument * result = new tinyxml2::XMLDocument();
    OutputToHTMLDocument(result, params); 
    return result;
  }

  bool TextureAtlasCreatorBase::OutputToHTMLFile(boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params) const
  {
    return OutputToHTMLFile(path.string().c_str());
  }

  bool TextureAtlasCreatorBase::OutputToHTMLFile(char const * filename, TextureAtlasHTMLOutputParams params) const
  {
    assert(filename != nullptr);

    bool result = false;

    tinyxml2::XMLDocument * doc = TextureAtlasCreatorBase::OutputToHTMLDocument(params);
    if (doc != nullptr)
    {
      result = (doc->SaveFile(filename) == tinyxml2::XML_SUCCESS);
      delete(doc);
    }
    return result;
  }

  void TextureAtlasCreatorBase::OutputToHTMLDocument(tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params) const
  {
    assert(doc != nullptr);

    HTMLTools html(doc);

    tinyxml2::XMLElement * Header = html.PushElement(doc, "!DOCTYPE HTML");
    tinyxml2::XMLElement * HTML   = html.PushElement(Header, "HTML");
    tinyxml2::XMLElement * BODY   = html.PushElement(HTML, "BODY");
    
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
      tinyxml2::XMLElement * P   = html.PushElement(BODY, "P");
      tinyxml2::XMLElement * PRE = html.PushElement(P, "PRE");

      html.PushText(PRE, GetGeneralInformationString().c_str());
    }

    int id = 0;
    for (size_t i = 0 ; i < GetAtlasCount() ; ++i)
    {
      int w =  MultDimension(width,  scale);
      int h =  MultDimension(height, scale);

      if (params.show_atlas_header)
      {
        tinyxml2::XMLElement * P   = html.PushElement(BODY, "P");
        tinyxml2::XMLElement * PRE = html.PushElement(P, "PRE");
        html.PushText(PRE, GetAtlasSpaceOccupationString(i).c_str());
      }

      tinyxml2::XMLElement * TABLE = html.PushElement(BODY, "TABLE");
      html.PushAttribute(TABLE, "border", 5);
      html.PushAttribute(TABLE, "cellpadding", 10);

      tinyxml2::XMLElement * TR = nullptr;

      size_t count = 0;
      for (size_t j = 0 ; j < data->entries.size() ; ++j)
      {
        TextureAtlasEntry const & entry = data->entries[j];
        if (entry.atlas != i)
          continue;

        if (TR == nullptr)
          TR = html.PushElement(TABLE, "TR");

        // first element of the line
        tinyxml2::XMLElement * TD  = html.PushElement(TR, "TD");
        tinyxml2::XMLElement * PRE = html.PushElement(TD, "PRE");
        html.PushText(PRE, data->GetTextureInfoString(entry, padding).c_str());

        if (count % 5 == 4)
          TR = nullptr;
        ++count;
      }

      if (params.show_textures)
      {
        char const * texture_bgnd = "fill-opacity:1.0;fill:rgb(150,150,150);stroke-width:2;stroke:rgb(0,0,0)";

        tinyxml2::XMLElement * BR  = html.PushElement(BODY, "BR");
        tinyxml2::XMLElement * SVG = html.PushElement(BODY, "SVG");

        html.PushAttribute(SVG, "width", w);
        html.PushAttribute(SVG, "height", h);

        tinyxml2::XMLElement * RECT = html.PushElement(SVG, "RECT");
        html.PushAttribute(RECT, "width", w);
        html.PushAttribute(RECT, "height", h);
        html.PushAttribute(RECT, "style", texture_bgnd);

        for (auto & entry : data->entries)
        {
          ++id;
          if (entry.atlas != i)
            continue;

          int color = 10 + (id % 10) * 10;

          int x = MultDimension(entry.x,      scale);
          int y = MultDimension(entry.y,      scale);
          int w = MultDimension(entry.width,  scale);
          int h = MultDimension(entry.height, scale);
          int p = MultDimension(padding, scale);

          char rect_props[1024];
          sprintf_s(rect_props, 1024, "fill-opacity:0.5;fill:rgb(%d,0,0);stroke-width:1;stroke:rgb(0,0,0)", color);

          tinyxml2::XMLElement * RECT = html.PushElement(SVG, "RECT");
          html.PushAttribute(RECT, "x", x + p);
          html.PushAttribute(RECT, "y", y + p);
          html.PushAttribute(RECT, "width", w);
          html.PushAttribute(RECT, "height", h);
          html.PushAttribute(RECT, "style", rect_props);
        }

        // display the filenames
        if (params.show_textures_names)
        {
          for (auto & entry : data->entries)
          {
            if (entry.atlas != i)
              continue;

            int p = MultDimension(padding, scale);
            int x = MultDimension(entry.x, scale) + MultDimension(entry.width , scale * 0.5f);
            int y = MultDimension(entry.y, scale) + MultDimension(entry.height, scale * 0.5f);

            tinyxml2::XMLElement * TEXT = html.PushElement(SVG, "TEXT");
            html.PushAttribute(TEXT, "text-anchor", "middle");
            html.PushAttribute(TEXT, "x", x + p);
            html.PushAttribute(TEXT, "y", y + p);
            html.PushAttribute(TEXT, "fill", "white");

#if _DEBUG
            html.PushText(TEXT, StringTools::Printf("%s [%d]", entry.filename.c_str(), entry.order_of_insertion).c_str());
#else
            html.PushText(TEXT, entry.filename.c_str());
#endif
          }
        }      
      }
    }
  }

  std::string TextureAtlasCreatorBase::GetAtlasSpaceOccupationString(size_t atlas_index) const
  {
    std::ostringstream stream;
    OutputAtlasSpaceOccupation(atlas_index, stream);
    return stream.str();
  }

  std::string TextureAtlasCreatorBase::GetAtlasSpaceOccupationString() const
  {
    std::ostringstream stream;
    OutputAtlasSpaceOccupation(stream);
    return stream.str();
  }

  void TextureAtlasCreatorBase::OutputAtlasSpaceOccupation(std::ostream & stream) const
  {
    for (size_t i = 0 ; i < GetAtlasCount() ; ++i)
      OutputAtlasSpaceOccupation(i, stream);
  }

  void TextureAtlasCreatorBase::OutputAtlasSpaceOccupation(size_t atlas_index, std::ostream & stream) const
  {
    float atlas_surface = (float)(width * height);

    float atlas_used_surface = data->ComputeSurface(atlas_index, padding);
    float percent            = 100.0f * atlas_used_surface / atlas_surface;

    stream << "Atlas " << atlas_index << std::endl;
    stream << "  occupation : " << percent << "%" << std::endl;
  }

  void TextureAtlasCreatorBase::OutputGeneralInformation(std::ostream & stream) const 
  {
    float atlas_surface   = (float)(width * height);
    float texture_surface = data->ComputeSurface(SIZE_MAX, padding);
    int   min_atlas_count = (int)std::ceil(texture_surface / atlas_surface);

    stream << "Texture surface    : " << texture_surface << std::endl;
    stream << "Atlas surface      : " << atlas_surface   << std::endl;
    stream << "Best atlas count   : " << min_atlas_count << std::endl;
    stream << "Actual atlas count : " << GetAtlasCount() << std::endl;
  }

  std::string TextureAtlasCreatorBase::GetGeneralInformationString() const 
  {
    std::ostringstream stream;
    OutputGeneralInformation(stream);
    return stream.str();
  } 

  bool TextureAtlasCreatorBase::EnsureValid(std::ostream & stream) const
  {
    bool result = true;

    size_t atlas_count = GetAtlasCount();
    size_t count       = data->entries.size();

    AtlasRectangle atlas_rectangle = GetAtlasRectangle();

    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasEntry const & t = data->entries[i];

      if (t.atlas >= atlas_count)
      {
        stream << "Texture atlas value is invalid !!! (index = " << i << ")" << std::endl;
        result = false;
      }

      AtlasRectangle r = AddPadding(t.GetRectangle());
      if (!r.IsFullyInside(atlas_rectangle))
      {
        stream << "Texture should be fully inside any atlas !!! (index = " << i << ")" << std::endl;
        result = false;
      }
    }

    for (size_t i = 0 ; i < count - 1 ; ++i)
    {
      for (size_t j = i + 1 ; j < count ; ++j)
      {
        TextureAtlasEntry const & t1 = data->entries[i];
        TextureAtlasEntry const & t2 = data->entries[j];

        if (t1.atlas != t2.atlas)
          continue;

        AtlasRectangle r1 = AddPadding(t1.GetRectangle());
        AtlasRectangle r2 = AddPadding(t2.GetRectangle());

        if (r1.IsIntersecting(r2))
        {
          stream << "Collision between textures !!! (indices = " << i << " and " << j << ")" << std::endl;
          result = false;
        }
      }
    }

    return result;
  }

  bool TextureAtlasCreatorBase::HasInterctingTexture(size_t atlas_index, AtlasRectangle const & r) const
  {
    AtlasRectangle r1 = AddPadding(r);

    size_t count = data->entries.size();
    for (size_t i = 0 ; i < count  ; ++i)
    {
      TextureAtlasEntry const & entry = data->entries[i];
      if (entry.atlas != atlas_index)
        continue;

      AtlasRectangle r2 = AddPadding(entry.GetRectangle());
      if (r2.IsIntersecting(r1))
        return true;
    }
    return false;
  }

  bool TextureAtlasCreatorBase::DoComputeResult()
  {
    return false;
  }

  size_t TextureAtlasCreatorBase::UpdateTexture(TextureAtlasEntry & new_texture, size_t atlas, int x, int y)
  {
    new_texture.x     = x;
    new_texture.y     = y;
    new_texture.atlas = atlas;

    return (&new_texture - &data->entries[0]);
  }

  std::vector<FIBITMAP *> TextureAtlasCreatorBase::GetAtlasTextures() const
  {
    unsigned char const color[] = {0, 0, 0, 255}; // B G R A

    std::vector<FIBITMAP *> result;

    // generate the images and save them
    size_t atlas_cout = GetAtlasCount();
    for (size_t i = 0 ; i < atlas_cout ; ++i)
    {
      FIBITMAP * image = FreeImage_Allocate(width, height, 32);
      if (image != nullptr)
      {       
        FreeImage_FillBackground(image, color, 0);

        for (TextureAtlasEntry const & entry : data->entries)
        {
          if (entry.atlas != i || entry.bitmap == nullptr)
            continue;
          FreeImage_Paste(image, entry.bitmap, entry.x + padding, entry.y + padding, 255);
        }
      }
      result.push_back(image);
    }
    return result;
  }

  bool TextureAtlasCreatorBase::SaveResults(char const * pattern) const
  {
    assert(pattern != nullptr);

    // generate the images
    std::vector<FIBITMAP *> images = GetAtlasTextures();
    
    // save them
    for (size_t i = 0 ; i < images.size() ; ++i)
    {
      FIBITMAP * im = images[i];
      if (im != nullptr)
      {
        std::string filename = StringTools::Printf("%s_%d.png", pattern, i + 1);
        FreeImage_Save(FIF_PNG, im, filename.c_str(), 0);
        FreeImage_Unload(im);      
      }
    }

    // generate a file for the index (lua format)
    std::string filename = StringTools::Printf("%s_index.txt", pattern);

    std::ofstream stream(filename);
    if (stream)
    {
      stream << "index = {" << std::endl;

      bool first = true;
      for (TextureAtlasEntry const & entry : data->entries)
      {
        if (entry.bitmap == nullptr)
          continue;
        if (!first)
          stream << "  }, {" << std::endl; // close previous
        else
          stream << "  {" << std::endl;

        std::string atlas_filename = StringTools::Printf("%s_%d.png", pattern, entry.atlas + 1);
        boost::filesystem::path atlas_basename = boost::filesystem::path(atlas_filename).filename();

        boost::filesystem::path texture_basename = boost::filesystem::path(entry.filename).filename();

        stream << "    name   = \"" << texture_basename.string() << "\"," << std::endl;
        stream << "    atlas  = \"" << atlas_basename.string()   << "\"," << std::endl;
        stream << "    x      = " << entry.x + padding           << "," << std::endl;
        stream << "    y      = " << entry.y + padding           << "," << std::endl;
        stream << "    width  = " << entry.width                 << "," << std::endl;
        stream << "    height = " << entry.height                << std::endl;

        first = false;
      }
      if (!first)
        stream << "  }" << std::endl; // close previous

      stream << "}" << std::endl;
    }

    return true;
  }

  // ========================================================================
  // TextureAtlasCreator implementation
  // ========================================================================

  size_t TextureAtlasCreator::GetAtlasCount() const
  {
    return atlas_definitions.size();
  }

  void TextureAtlasCreator::Clear()
  {
    TextureAtlasCreatorBase::Clear();
    atlas_definitions.clear();
  }

  float TextureAtlasCreator::GetAdjacentSurface(TextureAtlasEntry const & entry, AtlasDefinition const & atlas, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const
  {
    float result = 0.0f;

    size_t a = u;
    size_t b = v;

    // search the first intersection
    u += dx;
    v += dy;
    while ((u < x_count) && (u != SIZE_MAX) && (v < y_count) && (v != SIZE_MAX))
    {
      size_t index = u * y_count + v;
      if (collision[index])
        break;
      u += dx;
      v += dy;
    }

    u -= dx;
    v -= dy;

    if (u != a || v != b)
    {
      if (dx != 0)
      {
        int x1 = atlas.split_x[a];
        int x2 = atlas.split_x[u];

        result = ((float)std::abs(x1 - x2)) * ((float)(entry.height + 2 * padding));
      }
      else
      {
        int y1 = atlas.split_y[b];
        int y2 = atlas.split_y[v];

        result = ((float)std::abs(y1 - y2)) * ((float)(entry.width + 2 * padding));
      }
    }

    return result;
  }

  float TextureAtlasCreator::FindBestPositionInAtlas(TextureAtlasEntry const & entry, AtlasDefinition const & atlas, int & x, int & y) const
  {
    float result = -1.0f;

    size_t  x_count = atlas.split_x.size();
    size_t  y_count = atlas.split_y.size();
    size_t count = x_count * y_count;

    std::vector<int> collision;
    collision.insert(collision.begin(), count, 1); // by default, we cannot place the texture at any position

    // find collision table
    AtlasRectangle r;
    r.width  = entry.width;
    r.height = entry.height;

    bool any_value = false;
    for (size_t u = 0 ; u < x_count ; ++u)
    {
      int px = atlas.split_x[u];
      if (px + entry.width + 2 * padding > width) // cannot puts the texture at this position (not fully inside the atlas)
        break;
      r.x = px;

      for (size_t v = 0 ; v < y_count ; ++v)
      {
        int py = atlas.split_y[v];
        if (py + entry.height + 2 * padding > height)  // cannot puts the texture at this position (not fully inside the atlas)
          break;
        r.y = py;

        bool ok = (!HasInterctingTexture(&atlas - &atlas_definitions[0], r));

        any_value |= ok;

        if (ok)
          collision[u * y_count + v] = 0;
      }
    }

    if (!any_value)
      return -1.0f; // texture can be set nowhere in that atlas

    // find the best position
    for (size_t u = 0 ; u < x_count ; ++u)
    {
      for (size_t v = 0 ; v < y_count ; ++v)
      {
        size_t index = u * y_count + v;
        if (!collision[index])
        {
          float surf1 = GetAdjacentSurface(entry, atlas, collision, x_count, y_count, u, v, +1,  0);
          float surf2 = GetAdjacentSurface(entry, atlas, collision, x_count, y_count, u, v, -1,  0);
          float surf3 = GetAdjacentSurface(entry, atlas, collision, x_count, y_count, u, v,  0, +1);
          float surf4 = GetAdjacentSurface(entry, atlas, collision, x_count, y_count, u, v,  0, -1);

          float sum_surf = surf1 + surf2 + surf3 + surf4;

          if (sum_surf == 0.0f) // perfect fit
          {
            x = atlas.split_x[u];
            y = atlas.split_y[v];
            return 0.0f;
          }

          if (result < 0.0f || sum_surf < result)
          {
            x = atlas.split_x[u];
            y = atlas.split_y[v];
            result = sum_surf;          
          }
        }
      }
    }

    return result;
  }

  void TextureAtlasCreator::InsertOrdered(std::vector<int> & v, int value)
  {
    auto it = std::lower_bound(v.begin(), v.end(), value);
    if ((it != v.end()) && (*it == value))
      return;
    v.insert(it, value);
  }

  void TextureAtlasCreator::InsertTextureInAtlas(TextureAtlasEntry & entry, AtlasDefinition & atlas, int x, int y)
  {
    entry.atlas = &atlas - &atlas_definitions[0];
    entry.x     = x;
    entry.y     = y;

    InsertOrdered(atlas.split_x, x);
    InsertOrdered(atlas.split_x, x + entry.width + 2 * padding);

    InsertOrdered(atlas.split_y, y);
    InsertOrdered(atlas.split_y, y + entry.height + 2 * padding);
  }

  bool TextureAtlasCreator::DoComputeResult()
  {
    size_t count = data->entries.size();

    float p = (float)padding;

    // create an indirection list for texture sorted by surface
    std::vector<size_t> textures_indirection_table = CreateTextureIndirectionTable([this, p](size_t i1, size_t i2){

      TextureAtlasEntry const & t1 = data->entries[i1];
      TextureAtlasEntry const & t2 = data->entries[i2];

      if ((t1.height + p) * (t1.width + p) > (t2.height + p) * (t2.width + p))
        return true;
      return false;
    });


    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasEntry & new_texture = data->entries[textures_indirection_table[i]];

#if _DEBUG
      new_texture.order_of_insertion = i;
#endif

      size_t best_atlas_index =  SIZE_MAX;
      int    best_x           =  0;
      int    best_y           =  0;
      float  best_score       = -1.0f;

      for (size_t j = 0 ; j < atlas_definitions.size() ; ++j)
      {
        int   x, y;
        float score = FindBestPositionInAtlas(new_texture, atlas_definitions[j], x, y);

        if (score < 0.0f)
          continue; // cannot insert the texture in this atlas

        if (score < best_score || best_score < 0) // new best position found
        {
          best_score       = score;
          best_x           = x;
          best_y           = y;
          best_atlas_index = j;
        }

        if (score == 0.0f) // no need to search any more in any atlases
          break;
      }

      if (best_atlas_index == SIZE_MAX) // not enough size in any existing atlas. create a new one
      {
        AtlasDefinition def;
        def.split_x.push_back(0);
        def.split_x.push_back(width + 2 * padding);
        def.split_y.push_back(0);
        def.split_y.push_back(height + 2 * padding);

        best_atlas_index = atlas_definitions.size();
        best_x           = 0;
        best_y           = 0;

        atlas_definitions.push_back(def);
      }

      InsertTextureInAtlas(new_texture, atlas_definitions[best_atlas_index], best_x, best_y);
    }

    return true;
  }

  bool TextureAtlasCreator::CreateAtlasFromDirectory(char const * src_dir, char const * dst_pattern, int atlas_width, int atlas_height, int atlas_padding)
  {
    assert(src_dir     != nullptr);
    assert(dst_pattern != nullptr);

    TextureAtlasData data;
    data.AddTextureFilesFromDirectory(src_dir);

    // create the atlas files
    chaos::TextureAtlasCreator atlas_creator;
  
    if (atlas_creator.ComputeResult(data, atlas_width, atlas_height, atlas_padding))
    {
      boost::filesystem::path result_path = dst_pattern;
      boost::filesystem::create_directories(result_path.parent_path());
      return atlas_creator.SaveResults(dst_pattern);
    }

    return false;
  }
};

