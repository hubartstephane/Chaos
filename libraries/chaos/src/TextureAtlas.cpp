#include <chaos/TextureAtlas.h>
#include <chaos/GeometryFramework.h>
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
    json_entry["name"]   = entry.name;
    json_entry["atlas"]  = entry.atlas;
    json_entry["x"]      = entry.x;
    json_entry["y"]      = entry.y;
    json_entry["width"]  = entry.width;
    json_entry["height"] = entry.height;
  }

  void LoadFromJSON(TextureAtlasEntry & entry, nlohmann::json const & json_entry)
  {
    entry.name   = json_entry["name"].get<std::string>();
    entry.atlas  = json_entry["atlas"];
    entry.x      = json_entry["x"];
    entry.y      = json_entry["y"];
    entry.width  = json_entry["width"];
    entry.height = json_entry["height"];
  }
  
  // ========================================================================
  // TextureAtlasBase implementation
  // ========================================================================

  void TextureAtlasBase::Clear()
  {
    // destroy the output
    for (FIBITMAP * image : bitmaps)
      if (image != nullptr)
        FreeImage_Unload(image);
    bitmaps.clear();
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
    for (size_t i = 0; i < bitmaps.size(); ++i)
    {
      FIBITMAP * bitmap = bitmaps[i];
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
    for (size_t i = 0; (i < bitmaps.size()) && result; ++i)
    {
      FIBITMAP * im = bitmaps[i];
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
      for (size_t i = 0; i < bitmaps.size(); ++i)
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
        bitmaps.push_back(bitmap);
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
    stream << "  name   : " << entry.name   << std::endl;
    stream << "  width  : " << entry.width  << std::endl;
    stream << "  height : " << entry.height << std::endl;
    stream << "  x      : " << entry.x      << std::endl;
    stream << "  y      : " << entry.y      << std::endl;
    stream << "  atlas  : " << entry.atlas  << std::endl;
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
};

