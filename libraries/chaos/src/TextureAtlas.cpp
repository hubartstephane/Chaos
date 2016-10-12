#include <chaos/TextureAtlas.h>
#include <chaos/HTMLTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/StringTools.h>
#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>


namespace chaos
{

  // ========================================================================
  // TextureAtlasEntry functions
  // ========================================================================

  void SaveIntoJSON(TextureAtlasEntry const & entry, nlohmann::json & json_entry)
  {
    json_entry["filename"] = entry.filename;
    json_entry["atlas"]    = entry.atlas;
    json_entry["x"]        = entry.x;
    json_entry["y"]        = entry.y;
    json_entry["width"]    = entry.width;
    json_entry["height"]   = entry.height;
  }

  void LoadFromJSON(TextureAtlasEntry & entry, nlohmann::json const & json_entry)
  {
    entry.filename = json_entry["filename"].get<std::string>();
    entry.atlas    = json_entry["atlas"];
    entry.x        = json_entry["x"];
    entry.y        = json_entry["y"];
    entry.width    = json_entry["width"];
    entry.height   = json_entry["height"];
  }

  // ========================================================================
  // Independant function
  // ========================================================================

  /** multiply an integer with a float (two conversions) */
  static int MultDimension(int a, float b)
  {
    return (int)(((float)a) * b);
  }

  // ========================================================================
  // TextureAtlasInputBase implementation
  // ========================================================================

  bool TextureAtlasInputBase::AddTextureFilesFromDirectory(boost::filesystem::path const & p)
  {
    // enumerate the source directory
    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(p) ; it != end ; ++it)
      AddTextureFile(it->path(), true);                           // this will reject files that are not images .. not an error
    return true;
  }

  bool TextureAtlasInputBase::AddTextureFile(boost::filesystem::path const & path, bool release_bitmap)
  {
    if (boost::filesystem::is_regular_file(path))
      return AddTextureFile(path.string().c_str(), release_bitmap);
    return false;
  }

  bool TextureAtlasInputBase::AddTextureFile(char const * filename, bool release_bitmap)
  {
    assert(filename != nullptr);

    FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
    if (image != nullptr)
    {
      if (AddImageSource(boost::filesystem::path(filename).filename().string().c_str(), image, release_bitmap))
        return true;
      FreeImage_Unload(image);    
    }
    return false;
  }

  bool TextureAtlasInputBase::AddFakeImageSource(char const * filename)
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

    return AddImageSource(filename, image, false);
  }

  // ========================================================================
  // TextureAtlasInput implementation
  // ========================================================================

  bool TextureAtlasInput::AddImageSource(char const * filename, FIBITMAP * image, bool release_bitmap)
  {
    assert(filename != nullptr);
    assert(image    != nullptr);

    TextureAtlasInputEntry new_entry;

    new_entry.bitmap         = image;
    new_entry.width          = (int)FreeImage_GetWidth(new_entry.bitmap);
    new_entry.height         = (int)FreeImage_GetHeight(new_entry.bitmap);
    new_entry.bpp            = (int)FreeImage_GetBPP(new_entry.bitmap);
    new_entry.filename       = filename;
    new_entry.release_bitmap = release_bitmap;

    entries.push_back(std::move(new_entry)); // move for std::string copy
    return true;
  }

  void TextureAtlasInput::Clear()
  {
    // destroy the entries
    for (TextureAtlasInputEntry & input_entry : entries)
      if (input_entry.bitmap != nullptr)
        if (input_entry.release_bitmap)
          FreeImage_Unload(input_entry.bitmap);
    entries.empty();
  }

  // ========================================================================
  // TextureAtlasBase implementation
  // ========================================================================

  void TextureAtlasBase::Clear()
  {
    // destroy the output
    for (FIBITMAP * image : atlas_images)
      if (image != nullptr)
        FreeImage_Unload(image);
    atlas_images.clear();
  }

  void TextureAtlasBase::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const
  {
    // decompose INDEX and IMAGES filename
    target_dir = filename.parent_path();
    index_filename = filename;
    image_filename = filename.filename();

    if (!index_filename.has_extension())
      index_filename.replace_extension(".json");    // by default, INDEX file has extension JSON
    else
      image_filename.replace_extension(); // for moment, IMAGES files should not have any extension
  }

  boost::filesystem::path TextureAtlasBase::GetAtlasImageName(boost::filesystem::path image_filename, int index) const
  {
    char buffer[30]; // big far enough
    sprintf_s(buffer, "_%d.png", index);
    return image_filename.concat(buffer);
  }

  glm::ivec2 TextureAtlasBase::GetAtlasDimension() const
  {
    for (size_t i = 0; i < atlas_images.size(); ++i)
    {
      FIBITMAP * bitmap = atlas_images[i];
      if (bitmap != nullptr)
      {
        int width = (int)FreeImage_GetWidth(bitmap);
        int height = (int)FreeImage_GetHeight(bitmap);
        return glm::ivec2(width, height);
      }
    }
    return glm::ivec2(0, 0);
  }

  bool TextureAtlasBase::SaveAtlas(boost::filesystem::path const & filename) const
  {
    // decompose the filename
    boost::filesystem::path target_dir;
    boost::filesystem::path index_filename;
    boost::filesystem::path image_filename;
    SplitFilename(filename, target_dir, index_filename, image_filename);

    // create a target directory if necessary   
    if (!boost::filesystem::is_directory(target_dir))
      if (!boost::filesystem::create_directories(target_dir))
        return false;

    // save the atlas
    return SaveAtlasImages(target_dir, index_filename, image_filename) && SaveAtlasIndex(target_dir, index_filename, image_filename);
  }

  bool TextureAtlasBase::SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const
  {
    bool result = true;
    // save them
    for (size_t i = 0; (i < atlas_images.size()) && result; ++i)
    {
      FIBITMAP * im = atlas_images[i];
      if (im != nullptr)
      {
        boost::filesystem::path dst_filename = target_dir / GetAtlasImageName(image_filename, i);

        result = (FreeImage_Save(FIF_PNG, im, dst_filename.string().c_str(), 0) != 0);
      }
    }
    return result;
  }

  bool TextureAtlasBase::SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const
  {
    // generate a file for the index (JSON format)
    std::ofstream stream(index_filename.string().c_str());
    if (stream)
    {
      nlohmann::json j;
      // insert the files
      j["images"] = nlohmann::json::array();
      for (size_t i = 0; i < atlas_images.size(); ++i)
        j["images"].push_back(GetAtlasImageName(image_filename, i).string());
      // insert the entries
      j["entries"] = nlohmann::json::array();
      SaveAtlasEntriesInIndex(j["entries"]);
      // format the JSON into string and insert it into stream
      stream << j.dump(4);
      return true;
    }
    return false;
  }

  bool TextureAtlasBase::LoadAtlas(boost::filesystem::path const & filename)
  {
    // decompose the filename
    boost::filesystem::path target_dir;
    boost::filesystem::path index_filename;
    boost::filesystem::path image_filename;
    SplitFilename(filename, target_dir, index_filename, image_filename); // will be ignored during loading, real name is read from .JSON index
                                                                         // load the file into memory
    Buffer<char> buf = FileTools::LoadFile(index_filename, true);
    if (buf == nullptr)
      return false;

    // parse JSON file
    bool result = false;
    try
    {
      nlohmann::json j = nlohmann::json::parse(buf.data);
      result = LoadAtlas(j, target_dir);
    }
    catch (std::exception & e)
    {
      LogTools::Error("TextureAtlasBase::LoadAtlas(...) : error while parsing JSON file [%s] : %s", index_filename.string().c_str(), e.what());
    }
    return result;
  }

  bool TextureAtlasBase::LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & target_dir)
  {
    bool result = true;

    // clean the object
    Clear();

    try
    {
      // load the files
      nlohmann::json const & json_files = j["images"];
      for (auto const json_filename : json_files)
      {
        std::string const & filename = json_filename;

        FIBITMAP * bitmap = ImageTools::LoadImageFromFile((target_dir / filename).string().c_str());
        if (bitmap == nullptr)
        {
          result = false;
          break;
        }
        atlas_images.push_back(bitmap);
      }
      // load the entries
      if (result)
      {
        nlohmann::json const & json_entries = j["entries"];
        for (auto const json_entry : json_entries)
          LoadAtlasEntryFromIndex(json_entry);
      }
    }
    catch (std::exception & e)
    {
      LogTools::Error("TextureAtlasBase::LoadAtlas(...) : error while parsing JSON file : %s", e.what());
    }

    // in case of failure, reset the whole atlas once more
    if (!result)
      Clear();

    return result;
  }

  // ========================================================================
  // TextureAtlas implementation
  // ========================================================================

  float TextureAtlas::ComputeSurface(size_t atlas_index) const
  {
    float result = 0.0f;
    for (TextureAtlasEntry const & output_entry : entries)
    {
      if (output_entry.atlas != atlas_index && atlas_index != SIZE_MAX)
        continue;
      result += (float)(output_entry.width * output_entry.height);
    }
    return result;
  }

  void TextureAtlas::OutputTextureInfo(std::ostream & stream) const
  {
    for (TextureAtlasEntry const & entry : entries)
      OutputTextureInfo(entry, stream);
  }

  void TextureAtlas::OutputTextureInfo(TextureAtlasEntry const & entry, std::ostream & stream) const
  {
    stream << "Texture " << (&entry - &entries[0]) << std::endl;
    stream << "  filename : " << entry.filename    << std::endl;
    stream << "  width    : " << entry.width       << std::endl;
    stream << "  height   : " << entry.height      << std::endl;
    stream << "  x        : " << entry.x           << std::endl;
    stream << "  y        : " << entry.y           << std::endl;
    stream << "  atlas    : " << entry.atlas       << std::endl;
  }

  std::string TextureAtlas::GetTextureInfoString() const
  {
    std::ostringstream out;
    OutputTextureInfo(out);
    return out.str();
  }

  std::string TextureAtlas::GetTextureInfoString(TextureAtlasEntry const & entry) const
  {
    std::ostringstream out;
    OutputTextureInfo(entry, out);
    return out.str();
  }

  tinyxml2::XMLDocument * TextureAtlas::OutputToHTMLDocument(TextureAtlasHTMLOutputParams params) const
  {
    tinyxml2::XMLDocument * result = new tinyxml2::XMLDocument();
    OutputToHTMLDocument(result, params); 
    return result;
  }

  bool TextureAtlas::OutputToHTMLFile(boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params) const
  {
    return OutputToHTMLFile(path.string().c_str());
  }

  bool TextureAtlas::OutputToHTMLFile(char const * filename, TextureAtlasHTMLOutputParams params) const
  {
    assert(filename != nullptr);

    bool result = false;

    tinyxml2::XMLDocument * doc = OutputToHTMLDocument(params);
    if (doc != nullptr)
    {
      result = (doc->SaveFile(filename) == tinyxml2::XML_SUCCESS);
      delete(doc);
    }
    return result;
  }
  

  void TextureAtlas::OutputToHTMLDocument(tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params) const
  {
    assert(doc != nullptr);

    HTMLTools html(doc);

    tinyxml2::XMLElement * Header = html.PushElement(doc, "!DOCTYPE HTML");
    tinyxml2::XMLElement * HTML   = html.PushElement(Header, "HTML");
    tinyxml2::XMLElement * BODY   = html.PushElement(HTML, "BODY");
    
    glm::ivec2 atlas_dimension = GetAtlasDimension();

    int width  = atlas_dimension.x;
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
      for (size_t j = 0 ; j < entries.size() ; ++j)
      {
        TextureAtlasEntry const & entry = entries[j];
        if (entry.atlas != i)
          continue;

        if (TR == nullptr)
          TR = html.PushElement(TABLE, "TR");

        // first element of the line
        tinyxml2::XMLElement * TD  = html.PushElement(TR, "TD");
        tinyxml2::XMLElement * PRE = html.PushElement(TD, "PRE");
        html.PushText(PRE, GetTextureInfoString(entry).c_str());

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

        for (auto & entry : entries)
        {
          ++id;
          if (entry.atlas != i)
            continue;

          int color = 10 + (id % 10) * 10;

          int x = MultDimension(entry.x,      scale);
          int y = MultDimension(entry.y,      scale);
          int w = MultDimension(entry.width,  scale);
          int h = MultDimension(entry.height, scale);

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
          for (auto & entry : entries)
          {
            if (entry.atlas != i)
              continue;

            int x = MultDimension(entry.x, scale) + MultDimension(entry.width , scale * 0.5f);
            int y = MultDimension(entry.y, scale) + MultDimension(entry.height, scale * 0.5f);

            tinyxml2::XMLElement * TEXT = html.PushElement(SVG, "TEXT");
            html.PushAttribute(TEXT, "text-anchor", "middle");
            html.PushAttribute(TEXT, "x", x);
            html.PushAttribute(TEXT, "y", y);
            html.PushAttribute(TEXT, "fill", "white");

            html.PushText(TEXT, entry.filename.c_str());
          }
        }      
      }
    }
  }

  std::string TextureAtlas::GetAtlasSpaceOccupationString(size_t atlas_index) const
  {
    std::ostringstream stream;
    OutputAtlasSpaceOccupation(atlas_index, stream);
    return stream.str();
  }

  std::string TextureAtlas::GetAtlasSpaceOccupationString() const
  {
    std::ostringstream stream;
    OutputAtlasSpaceOccupation(stream);
    return stream.str();
  }

  void TextureAtlas::OutputAtlasSpaceOccupation(std::ostream & stream) const
  {
    for (size_t i = 0 ; i < GetAtlasCount() ; ++i)
      OutputAtlasSpaceOccupation(i, stream);
  }

  void TextureAtlas::OutputAtlasSpaceOccupation(size_t atlas_index, std::ostream & stream) const
  {
    glm::ivec2 atlas_size = GetAtlasDimension();

    float atlas_surface = (float)(atlas_size.x * atlas_size.y);

    float atlas_used_surface = ComputeSurface(atlas_index);
    float percent            = 100.0f * atlas_used_surface / atlas_surface;

    stream << "Atlas " << atlas_index << std::endl;
    stream << "  occupation : " << percent << "%" << std::endl;
  }

  void TextureAtlas::OutputGeneralInformation(std::ostream & stream) const
  {
    glm::ivec2 atlas_size = GetAtlasDimension();

    float atlas_surface   = (float)(atlas_size.x * atlas_size.y);
    float texture_surface = ComputeSurface(SIZE_MAX);
    int   min_atlas_count = (int)std::ceil(texture_surface / atlas_surface);

    stream << "Texture surface    : " << texture_surface << std::endl;
    stream << "Atlas surface      : " << atlas_surface   << std::endl;
    stream << "Best atlas count   : " << min_atlas_count << std::endl;
    stream << "Actual atlas count : " << GetAtlasCount() << std::endl;
  }

  std::string TextureAtlas::GetGeneralInformationString() const
  {
    std::ostringstream stream;
    OutputGeneralInformation(stream);
    return stream.str();
  } 

  // ========================================================================
  // TextureAtlasGenerator implementation
  // ========================================================================

  AtlasRectangle TextureAtlasGenerator::GetAtlasRectangle() const
  {
    AtlasRectangle result;
    result.x      = 0;
    result.y      = 0;
    result.width  = params.atlas_width;
    result.height = params.atlas_height;
    return result;
  }

  AtlasRectangle TextureAtlasGenerator::GetRectangle(TextureAtlasEntry const & entry) const
  {
    AtlasRectangle result;
    result.x      = entry.x;
    result.y      = entry.y;
    result.width  = entry.width;
    result.height = entry.height;
    return result; 
  }
  
  AtlasRectangle TextureAtlasGenerator::AddPadding(AtlasRectangle const & r) const
  {
    AtlasRectangle result = r;
    result.x      -= params.atlas_padding;
    result.y      -= params.atlas_padding;
    result.width  += 2 * params.atlas_padding;
    result.height += 2 * params.atlas_padding;
    return result;
  }

  bool TextureAtlasGenerator::ComputeResult(TextureAtlasInput & in_input, TextureAtlas & in_output, TextureAtlasGeneratorParams const & in_params)
  {
    // clear generator from previous usage
    Clear();

    // store arguments inside
    params = in_params;
    input  = &in_input;
    output = &in_output;

    // prepare the result to receive new compuation
    output->Clear(); 

    // ensure BPP is valid
    if (params.atlas_bpp != 0 && params.atlas_bpp != 8 && params.atlas_bpp != 24 && params.atlas_bpp != 32)
      return false;

    // search max texture size
    int max_width  = -1;
    int max_height = -1;
    int max_bpp    = -1;

    for (TextureAtlasInputEntry & input_entry : input->entries)
    {
      if (max_width < 0 || max_width < input_entry.width)
        max_width = input_entry.width;
      if (max_height < 0 || max_height < input_entry.height)
        max_height = input_entry.height;
      if (max_bpp < 0 || max_bpp < input_entry.bpp)
        max_bpp = input_entry.bpp;
    }

    max_width  += params.atlas_padding * 2;
    max_height += params.atlas_padding * 2;

    // compute final atlas size
    if (params.atlas_width <= 0 || params.atlas_width < max_width)
      params.atlas_width = max_width;
    if (params.atlas_height <= 0 || params.atlas_height < max_height)
      params.atlas_height = max_height;

    if (params.force_power_of_2)
    {
      params.atlas_width  = MathTools::GetNearestPowerOf2(params.atlas_width);
      params.atlas_height = MathTools::GetNearestPowerOf2(params.atlas_height);
    }

    // test the validity of size
    if (params.atlas_max_width > 0 && params.atlas_max_width < params.atlas_width)
      return false;
    if (params.atlas_max_height > 0 && params.atlas_max_height < params.atlas_height)
      return false;

    // if necessary match BPP to textures in input
    if (params.atlas_bpp <= 0)
    {
      if (max_bpp != 8 && max_bpp != 24 && max_bpp != 32) // 16 BPP is unused
        max_bpp = 32;
      params.atlas_bpp = max_bpp;
    }
    
    // generate an entry in output for each entry in input
    size_t count = input->entries.size();

    output->entries.reserve(count);
    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasInputEntry const & input_entry = input->entries[i];

      TextureAtlasEntry output_entry;
      output_entry.filename = input_entry.filename;
      output_entry.atlas    = SIZE_MAX;
      output_entry.x        = 0;
      output_entry.y        = 0;
      output_entry.width    = input_entry.width;
      output_entry.height   = input_entry.height;

      output->entries.push_back(output_entry);
    }
    // ensure this can be produced inside an atlas with size_restriction
    if (DoComputeResult())
    {
      if (EnsureValidResults())
      {
        output->atlas_images = GenerateAtlasTextures();
#if _DEBUG
        output->OutputAtlasSpaceOccupation(std::cout);
        output->OutputTextureInfo(std::cout);
#endif
        return true;
      }   
#if _DEBUG
      else
        output->OutputTextureInfo(std::cout);
#endif
    }    
    return false;
  }


  bool TextureAtlasGenerator::EnsureValidResults(std::ostream & stream) const
  {
    bool result = true;

    size_t atlas_count = GetAtlasCount();
    size_t count       = output->entries.size();

    AtlasRectangle atlas_rectangle = GetAtlasRectangle();

    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasEntry const & t = output->entries[i];

      if (t.atlas >= atlas_count)
      {
        stream << "Texture atlas value is invalid !!! (index = " << i << ")" << std::endl;
        result = false;
      }

      AtlasRectangle r = AddPadding(GetRectangle(t));
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
        TextureAtlasEntry const & output_entry_1 = output->entries[i];
        TextureAtlasEntry const & output_entry_2 = output->entries[j];

        if (output_entry_1.atlas != output_entry_2.atlas)
          continue;

        AtlasRectangle r1 = AddPadding(GetRectangle(output_entry_1));
        AtlasRectangle r2 = AddPadding(GetRectangle(output_entry_2));

        if (r1.IsIntersecting(r2))
        {
          stream << "Collision between textures !!! (indices = " << i << " and " << j << ")" << std::endl;
          result = false;
        }
      }
    }

    return result;
  }

  bool TextureAtlasGenerator::HasInterctingTexture(size_t atlas_index, AtlasRectangle const & r) const
  {
    AtlasRectangle r1 = AddPadding(r);

    size_t count = output->entries.size();
    for (size_t i = 0 ; i < count  ; ++i)
    {
      TextureAtlasEntry const & output_entry = output->entries[i];
      if (output_entry.atlas != atlas_index)
        continue;

      AtlasRectangle r2 = AddPadding(GetRectangle(output_entry));
      if (r2.IsIntersecting(r1))
        return true;
    }
    return false;
  }

  std::vector<FIBITMAP *> TextureAtlasGenerator::GenerateAtlasTextures() const
  {
    unsigned char const color[] = {0, 0, 0, 0}; // B G R A

    std::vector<FIBITMAP *> result;

    // generate the images and save them
    size_t atlas_cout = GetAtlasCount();
    for (size_t i = 0 ; i < atlas_cout ; ++i)
    {
      FIBITMAP * image = FreeImage_Allocate(params.atlas_width, params.atlas_height, params.atlas_bpp);
      if (image != nullptr)
      {       
        FreeImage_FillBackground(image, color, 0);

        size_t count = output->entries.size(); // should be same than inputs
        for (size_t j = 0 ; j < count ; ++j)         
        {
          TextureAtlasInputEntry const & input_entry = input->entries[j];
          TextureAtlasEntry      const & output_entry = output->entries[j];

          if (output_entry.atlas != i || input_entry.bitmap == nullptr)
            continue;

          FreeImage_Paste(image, input_entry.bitmap, output_entry.x, output_entry.y, 255);
        }
      }
      result.push_back(image);
    }
    return result;
  }

  size_t TextureAtlasGenerator::GetAtlasCount() const
  {
    return atlas_definitions.size();
  }

  void TextureAtlasGenerator::Clear()
  {
    params = TextureAtlasGeneratorParams();
    input  = nullptr;
    output = nullptr;

    atlas_definitions.clear();
  }

  float TextureAtlasGenerator::GetAdjacentSurface(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const
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
        int x1 = atlas_def.split_x[a];
        int x2 = atlas_def.split_x[u];

        result = ((float)std::abs(x1 - x2)) * ((float)(input_entry.height + 2 * params.atlas_padding));
      }
      else
      {
        int y1 = atlas_def.split_y[b];
        int y2 = atlas_def.split_y[v];

        result = ((float)std::abs(y1 - y2)) * ((float)(input_entry.width + 2 * params.atlas_padding));
      }
    }

    return result;
  }

  float TextureAtlasGenerator::FindBestPositionInAtlas(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, int & x, int & y) const
  {
    float result = -1.0f;

    size_t  x_count = atlas_def.split_x.size();
    size_t  y_count = atlas_def.split_y.size();
    size_t count = x_count * y_count;

    std::vector<int> collision_table;
    collision_table.insert(collision_table.begin(), count, 1); // by default, we cannot place the texture at any position

    // find collision table
    AtlasRectangle r;
    r.width  = input_entry.width;
    r.height = input_entry.height;

    bool any_value = false;
    for (size_t u = 0 ; u < x_count ; ++u)
    {
      int px = atlas_def.split_x[u];
      if (px + input_entry.width + 2 * params.atlas_padding > params.atlas_width) // cannot puts the texture at this position (not fully inside the atlas)
        break;
      r.x = px + params.atlas_padding;

      for (size_t v = 0 ; v < y_count ; ++v)
      {
        int py = atlas_def.split_y[v];
        if (py + input_entry.height + 2 * params.atlas_padding > params.atlas_height)  // cannot puts the texture at this position (not fully inside the atlas)
          break;
        r.y = py + params.atlas_padding;

        bool collision = HasInterctingTexture(&atlas_def - &atlas_definitions[0], r);

        any_value |= !collision;

        if (!collision)
          collision_table[u * y_count + v] = 0;
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
        if (!collision_table[index])
        {
          float surf1 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v, +1,  0);
          float surf2 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v, -1,  0);
          float surf3 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v,  0, +1);
          float surf4 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v,  0, -1);

          float sum_surf = surf1 + surf2 + surf3 + surf4;

          if (sum_surf == 0.0f) // perfect fit
          {
            x = atlas_def.split_x[u];
            y = atlas_def.split_y[v];
            return 0.0f;
          }

          if (result < 0.0f || sum_surf < result)
          {
            x = atlas_def.split_x[u];
            y = atlas_def.split_y[v];
            result = sum_surf;          
          }
        }
      }
    }

    return result;
  }

  void TextureAtlasGenerator::InsertOrdered(std::vector<int> & v, int value)
  {
    auto it = std::lower_bound(v.begin(), v.end(), value);
    if ((it != v.end()) && (*it == value))
      return;
    v.insert(it, value);
  }

  void TextureAtlasGenerator::InsertTextureInAtlas(TextureAtlasEntry & input_entry, AtlasDefinition & atlas_def, int x, int y)
  {
    input_entry.atlas = &atlas_def - &atlas_definitions[0];
    input_entry.x     = x + params.atlas_padding;
    input_entry.y     = y + params.atlas_padding;

    InsertOrdered(atlas_def.split_x, x);
    InsertOrdered(atlas_def.split_x, x + input_entry.width + 2 * params.atlas_padding);

    InsertOrdered(atlas_def.split_y, y);
    InsertOrdered(atlas_def.split_y, y + input_entry.height + 2 * params.atlas_padding);
  }

  bool TextureAtlasGenerator::DoComputeResult()
  {
    size_t count = input->entries.size();

    float p = (float)params.atlas_padding;

    // create an indirection list for texture sorted by surface
    std::vector<size_t> textures_indirection_table = CreateTextureIndirectionTable([this, p](size_t i1, size_t i2){

      TextureAtlasInputEntry const & input_entry_1 = input->entries[i1];
      TextureAtlasInputEntry const & input_entry_2 = input->entries[i2];

      if ((input_entry_1.height + p) * (input_entry_1.width + p) > (input_entry_2.height + p) * (input_entry_2.width + p))
        return true;
      return false;
    });

    for (size_t i = 0 ; i < count ; ++i)
    {
      int entry_index = textures_indirection_table[i];

      TextureAtlasInputEntry & input_entry = input->entries[entry_index];

      size_t best_atlas_index =  SIZE_MAX;
      int    best_x           =  0;
      int    best_y           =  0;
      float  best_score       = -1.0f;

      for (size_t j = 0 ; j < atlas_definitions.size() ; ++j)
      {
        int   x, y;
        float score = FindBestPositionInAtlas(input_entry, atlas_definitions[j], x, y);

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
        def.split_x.push_back(params.atlas_width + 2 * params.atlas_padding);
        def.split_y.push_back(0);
        def.split_y.push_back(params.atlas_height + 2 * params.atlas_padding);

        best_atlas_index = atlas_definitions.size();
        best_x           = 0;
        best_y           = 0;

        atlas_definitions.push_back(def);
      }

      InsertTextureInAtlas(output->entries[entry_index], atlas_definitions[best_atlas_index], best_x, best_y);
    }

    return true;
  }

  bool TextureAtlasGenerator::CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, TextureAtlasGeneratorParams const & in_params)
  {
    // fill the atlas
    TextureAtlasInput input;
    input.AddTextureFilesFromDirectory(src_dir);

    // create the atlas files
    TextureAtlas          atlas;
    TextureAtlasGenerator generator;  
    if (generator.ComputeResult(input, atlas, in_params))
      return atlas.SaveAtlas(filename);
    return false;
  }
};

