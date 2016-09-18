#include <chaos/FontAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/StringTools.h>
#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>


namespace chaos
{

#if 0
  // ========================================================================
  // FontAtlas implementation
  // ========================================================================

  void FontAtlas::Clear()
  {
    // destroy the entries
    entries.empty();

    // destroy the output
    for (FIBITMAP * image : atlas_images)
      if (image != nullptr)
        FreeImage_Unload(image);
    atlas_images.clear();
  }

  void FontAtlas::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const
  {
    // decompose INDEX and IMAGES filename
    target_dir     = filename.parent_path();
    index_filename = filename;
    image_filename = filename.filename();

    if (!index_filename.has_extension())
      index_filename.replace_extension(".json");    // by default, INDEX file has extension JSON
    else
      image_filename.replace_extension(); // for moment, IMAGES files should not have any extension
  }

  bool FontAtlas::LoadAtlas(boost::filesystem::path const & filename)
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
      LogTools::Error("FontAtlas::LoadAtlas(...) : error while parsing JSON file [%s] : %s", index_filename.string().c_str(), e.what());
    }
    return result;
  }

  bool FontAtlas::LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & target_dir)
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
        {
          FontAtlasEntry entry;
            
          entry.filename = json_entry["filename"].get<std::string>();
          entry.atlas    = json_entry["atlas"];
          entry.x        = json_entry["x"];
          entry.y        = json_entry["y"];
          entry.width    = json_entry["width"];
          entry.height   = json_entry["height"];

          entries.push_back(entry);
        }
      }
    }
    catch (std::exception & e)
    {
      LogTools::Error("FontAtlas::LoadAtlas(...) : error while parsing JSON file : %s", e.what());
    }

    // in case of failure, reset the whole atlas once more
    if (!result)
      Clear();

    return result;
  }

  bool FontAtlas::SaveAtlas(boost::filesystem::path const & filename) const
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

  boost::filesystem::path FontAtlas::GetAtlasImageName(boost::filesystem::path image_filename, int index) const
  {
    char buffer[30]; // big far enough
    sprintf_s(buffer, "_%d.png", index);
    return image_filename.concat(buffer);
  }

  bool FontAtlas::SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const
  {
    bool result = true;
    // save them
    for (size_t i = 0 ; (i < atlas_images.size()) && result ; ++i)
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

  bool FontAtlas::SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const
  {
    // generate a file for the index (JSON format)
    std::ofstream stream(index_filename.string().c_str());
    if (stream)
    {
      nlohmann::json j;

      // insert the files
      j["images"] = nlohmann::json::array();
      for (size_t i = 0 ; i < atlas_images.size() ; ++i)
        j["images"].push_back(GetAtlasImageName(image_filename, i).string());

      // insert the entries
      j["entries"] = nlohmann::json::array();
      for (size_t i = 0 ; i < entries.size() ; ++i)
      {
        FontAtlasEntry const & entry = entries[i];

        auto json_entry = nlohmann::json();
        json_entry["filename"] = entry.filename;
        json_entry["atlas"]    = entry.atlas;
        json_entry["x"]        = entry.x;
        json_entry["y"]        = entry.y;
        json_entry["width"]    = entry.width;
        json_entry["height"]   = entry.height;

        j["entries"].push_back(json_entry);
      }

      stream << j.dump(4);

      return true;
    }

    return false;
  }

  glm::ivec2 FontAtlas::GetAtlasDimension() const
  {
    for (size_t i = 0 ; i < atlas_images.size() ; ++i)
    {
      FIBITMAP * bitmap = atlas_images[i];
      if (bitmap != nullptr)
      {
        int width  = (int)FreeImage_GetWidth(bitmap);
        int height = (int)FreeImage_GetHeight(bitmap);
        return glm::ivec2(width, height);
      }       
    }
    return glm::ivec2(0, 0);
  }
#endif
};

