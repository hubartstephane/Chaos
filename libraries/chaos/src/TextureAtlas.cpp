#include <chaos/TextureAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>


namespace chaos
{

  namespace TextureAtlasx
  {
    // ========================================================================
    // BitmapSet functions
    // ========================================================================

    BitmapEntry const * BitmapSet::GetEntry(char const * name) const
    {
      for (auto const & element : elements)
        if (element.name == name)
          return &element;
      return nullptr;
    }

    // ========================================================================
    // CharacterSet functions
    // ========================================================================

		CharacterEntry const * CharacterSet::GetEntry(char const * name) const
    {
      for (auto const & element : elements)
        if (element.name == name)
          return &element;
      return nullptr;
    }

		// ========================================================================
		// JSON functions
		// ========================================================================

		template<typename T>
		void SaveIntoJSON(std::vector<T> const & elements, nlohmann::json & json_entries)
		{
			for (auto const & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(element, json_entry);
				json_entries.push_back(json_entry);
			}
		}

		template<typename T>
		void SaveIntoJSON(std::vector<T*> const & elements, nlohmann::json & json_entries)
		{
			for (auto const * element : elements)
			{
				if (element != nullptr)
					continue;
				auto json_entry = nlohmann::json();
				SaveIntoJSON(*element, json_entry);
				json_entries.push_back(json_entry);
			}
		}

		template<typename T>
		void LoadFromJSON(std::vector<T> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T element;
				LoadFromJSON(element, json_entry);
				elements.push_back(element);
			}					
		}

		template<typename T>
		void LoadFromJSON(std::vector<T*> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T * element = new T;
				if (element == nullptr)
					continue;
				LoadFromJSON(*element, json_entry);
				elements.push_back(element);
			}		
		}

		void SaveIntoJSON(NamedObject const & entry, nlohmann::json & json_entry)
		{
			json_entry["name"] = entry.name;
			json_entry["tag"]  = entry.tag;
		}

		void LoadFromJSON(NamedObject & entry, nlohmann::json const & json_entry)
		{
			entry.name = json_entry["name"].get<std::string>();
			entry.tag  = json_entry["tag"];
		}

		void SaveIntoJSON(BitmapEntry const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["bitmap_index"] = entry.bitmap_index;
			json_entry["x"]            = entry.x;
			json_entry["y"]            = entry.y;
			json_entry["width"]        = entry.width;
			json_entry["height"]       = entry.height;		
		}

		void LoadFromJSON(BitmapEntry & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method

			entry.bitmap_index = json_entry["bitmap_index"];
			entry.x            = json_entry["x"];
			entry.y            = json_entry["y"];
			entry.width        = json_entry["width"];
			entry.height       = json_entry["height"];		
		}

		void SaveIntoJSON(CharacterEntry const & entry, nlohmann::json & json_entry)
		{
			BitmapEntry const & bitmap_entry = entry;
			SaveIntoJSON(bitmap_entry, json_entry); // call 'super' method

			json_entry["advance_x"]   = entry.advance.x;
			json_entry["advance_y"]   = entry.advance.y;
			json_entry["bitmap_left"] = entry.bitmap_left;
			json_entry["bitmap_top"]  = entry.bitmap_top;
		}

		void LoadFromJSON(CharacterEntry & entry, nlohmann::json const & json_entry)
		{
			BitmapEntry & bitmap_entry = entry;
			LoadFromJSON(bitmap_entry, json_entry); // call 'super' method

			entry.advance.x   = json_entry["advance_x"];
			entry.advance.y   = json_entry["advance_y"];
			entry.bitmap_left = json_entry["bitmap_left"];
			entry.bitmap_top  = json_entry["bitmap_top"];
		}

		void SaveIntoJSON(BitmapSet const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["elements"] = nlohmann::json::array();
			SaveIntoJSON(entry.elements, json_entry["elements"]);
		}

		void LoadFromJSON(BitmapSet & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method

			LoadFromJSON(entry.elements, json_entry["elements"]);
		}

		void SaveIntoJSON(CharacterSet const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["elements"] = nlohmann::json::array();
			SaveIntoJSON(entry.elements, json_entry["elements"]);
		}

		void LoadFromJSON(CharacterSet & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method

			LoadFromJSON(entry.elements, json_entry["elements"]);
		}

    // ========================================================================
    // Atlas functions
    // ========================================================================

    void Atlas::Clear()
    {
      // destroy the bitmap sets
      for (BitmapSet * bitmap_set : bitmap_sets)
        delete(bitmap_set);
      bitmap_sets.clear();
      // destroy the character sets
      for (CharacterSet * character_set : character_sets)
        delete(character_set);
			character_sets.clear();
      // destroy the bitmaps
      for (FIBITMAP * bitmap : bitmaps)
        FreeImage_Unload(bitmap);
      bitmaps.clear();
    }

    BitmapSet const * Atlas::GetBitmapSet(char const * name) const
    {
      assert(name != nullptr);
      for (BitmapSet * bitmap_set : bitmap_sets)
        if (bitmap_set->name == name)
          return bitmap_set;
      return nullptr;
    }

		CharacterSet const * Atlas::GetCharacterSet(char const * name) const
    {
      assert(name != nullptr);
      for (CharacterSet * character_set : character_sets)
        if (character_set->name == name)
          return character_set;
      return nullptr;
    }

		void Atlas::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const
		{
			// decompose INDEX and BITMAPS filename
			target_dir      = filename.parent_path();
			index_filename  = filename;
			bitmap_filename = filename.filename();

			if (!index_filename.has_extension())
				index_filename.replace_extension(".json");    // by default, INDEX file has extension JSON
			else
				bitmap_filename.replace_extension(); // for moment, BITMAP files should not have any extension
		}

		boost::filesystem::path Atlas::GetBitmapFilename(boost::filesystem::path bitmap_filename, int index) const
		{
			char buffer[30]; // big far enough
			sprintf_s(buffer, "_%d.png", index);
			return bitmap_filename.concat(buffer);
		}

		glm::ivec2 Atlas::GetAtlasDimension() const
		{
			for (size_t i = 0; i < bitmaps.size(); ++i)
			{
				FIBITMAP * bitmap = bitmaps[i];
				if (bitmap != nullptr)
				{
					int width  = (int)FreeImage_GetWidth(bitmap);
					int height = (int)FreeImage_GetHeight(bitmap);
					return glm::ivec2(width, height);
				}
			}
			return glm::ivec2(0, 0);
		}

		bool Atlas::SaveAtlas(boost::filesystem::path const & filename) const
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(filename, target_dir, index_filename, bitmap_filename);

			// create a target directory if necessary   
			if (!boost::filesystem::is_directory(target_dir))
				if (!boost::filesystem::create_directories(target_dir))
					return false;

			// save the atlas
			return SaveAtlasBitmaps(target_dir, index_filename, bitmap_filename) && SaveAtlasIndex(target_dir, index_filename, bitmap_filename);
		}

		bool Atlas::SaveAtlasBitmaps(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const
		{
			bool result = true;
			// save them
			size_t count = bitmaps.size();
			for (size_t i = 0; (i < count) && result; ++i)
			{
				FIBITMAP * im = bitmaps[i];
				if (im != nullptr)
				{
					boost::filesystem::path dst_filename = target_dir / GetBitmapFilename(bitmap_filename, i);

					result = (FreeImage_Save(FIF_PNG, im, dst_filename.string().c_str(), 0) != 0);
				}
			}
			return result;
		}

		bool Atlas::SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const
		{
			// generate a file for the index (JSON format)
			std::ofstream stream(index_filename.string().c_str());
			if (stream)
			{
				nlohmann::json j;
				// insert the files
				j["bitmaps"] = nlohmann::json::array();
				for (size_t i = 0; i < bitmaps.size(); ++i)
					j["bitmaps"].push_back(GetBitmapFilename(bitmap_filename, i).string());
				// insert the entries
				j["bitmap_sets"] = nlohmann::json::array();
				SaveIntoJSON(bitmap_sets, j["bitmap_sets"]);
				j["character_sets"] = nlohmann::json::array();
				SaveIntoJSON(bitmap_sets, j["character_sets"]);
				// format the JSON into string and insert it into stream
				stream << j.dump(4);
				return true;
			}
			return false;
		}

		bool Atlas::LoadAtlas(boost::filesystem::path const & filename)
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(filename, target_dir, index_filename, bitmap_filename); // will be ignored during loading, real name is read from .JSON index
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
				LogTools::Error("Atlas::LoadAtlas(...) : error while parsing JSON file [%s] : %s", index_filename.string().c_str(), e.what());
			}
			return result;
		}

		bool Atlas::LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & target_dir)
		{
			bool result = true;

			// clean the object
			Clear();

			try
			{
				// load the files
				nlohmann::json const & json_files = j["bitmaps"];
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
					LoadFromJSON(bitmap_sets, j["bitmap_sets"]);
					LoadFromJSON(character_sets, j["character_sets"]);
				}
			}
			catch (std::exception & e)
			{
				LogTools::Error("Atlas::LoadAtlas(...) : error while parsing JSON file : %s", e.what());
			}

			// in case of failure, reset the whole atlas once more
			if (!result)
				Clear();

			return result;
		}

		float Atlas::ComputeSurface(int bitmap_index) const
		{
			float result = 0.0f;
			// surface for the bitmap sets
			for (BitmapSet const * bitmap_set : bitmap_sets)
				if (bitmap_set != nullptr)
					for (BitmapEntry const & entry : bitmap_set->elements)
						if (entry.bitmap_index == bitmap_index || bitmap_index < 0)
							result += (float)(entry.width * entry.height);
			// surface for character sets
			for (CharacterSet const * character_set : character_sets)
				if (character_set != nullptr)
					for (CharacterEntry const & entry : character_set->elements)
						if (entry.bitmap_index == bitmap_index || bitmap_index < 0)
							result += (float)(entry.width * entry.height);
			
			return result;
		}

		void Atlas::OutputInfo(std::ostream & stream) const
		{
			// info for bitmap sets
			for (BitmapSet const * bitmap_set : bitmap_sets)
				if (bitmap_set != nullptr)
					for (BitmapEntry const & entry : bitmap_set->elements)
						OutputInfo(entry, stream);
			// info for character sets
			for (CharacterSet const * character_set : character_sets)
				if (character_set != nullptr)
					for (CharacterEntry const & entry : character_set->elements)
						OutputInfo(entry, stream);
		}

		void Atlas::OutputInfo(NamedObject const & entry, std::ostream & stream)
		{
			stream << "  name         : " << entry.name << std::endl;
			stream << "  tag          : " << entry.tag  << std::endl;		
		}

		void Atlas::OutputInfo(BitmapEntry const & entry, std::ostream & stream)
		{
			NamedObject const & named_entry = entry;
			OutputInfo(named_entry, stream);

			stream << "  bitmap_index : " << entry.bitmap_index << std::endl;
			stream << "  width        : " << entry.width        << std::endl;
			stream << "  height       : " << entry.height       << std::endl;
			stream << "  x            : " << entry.x            << std::endl;
			stream << "  y            : " << entry.y            << std::endl;
		}

		void Atlas::OutputInfo(CharacterEntry const & entry, std::ostream & stream)
		{
			BitmapEntry const & bitmap_entry = entry;
			OutputInfo(bitmap_entry, stream);

			stream << "  advance.x    : " << entry.advance.x   << std::endl;
			stream << "  advance.y    : " << entry.advance.y   << std::endl;
			stream << "  bitmap_left  : " << entry.bitmap_left << std::endl;
			stream << "  bitmap_top   : " << entry.bitmap_top  << std::endl;
		}

		std::string Atlas::GetInfoString() const
		{
			std::ostringstream out;
			OutputInfo(out);
			return out.str();
		}

		std::string Atlas::GetInfoString(NamedObject const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string Atlas::GetInfoString(BitmapEntry const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string Atlas::GetInfoString(CharacterEntry const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string Atlas::GetAtlasSpaceOccupationString(int bitmap_index) const
		{
			std::ostringstream stream;
			OutputAtlasSpaceOccupation(bitmap_index, stream);
			return stream.str();
		}

		std::string Atlas::GetAtlasSpaceOccupationString() const
		{
			std::ostringstream stream;
			OutputAtlasSpaceOccupation(stream);
			return stream.str();
		}

		void Atlas::OutputAtlasSpaceOccupation(std::ostream & stream) const
		{
			size_t count = bitmaps.size();
			for (size_t i = 0 ; i < count ; ++i)
				OutputAtlasSpaceOccupation(i, stream);
		}

		void Atlas::OutputAtlasSpaceOccupation(int bitmap_index, std::ostream & stream) const
		{
			glm::ivec2 atlas_size = GetAtlasDimension();

			float atlas_surface      = (float)(atlas_size.x * atlas_size.y);
			float atlas_used_surface = ComputeSurface(bitmap_index);
			float percent            = 100.0f * atlas_used_surface / atlas_surface;

			stream << "Atlas " << bitmap_index << std::endl;
			stream << "  occupation : " << percent << "%" << std::endl;
		}

		void Atlas::OutputGeneralInformation(std::ostream & stream) const
		{
			glm::ivec2 atlas_size = GetAtlasDimension();

			float atlas_surface   = (float)(atlas_size.x * atlas_size.y);
			float texture_surface = ComputeSurface(-1);
			int   min_atlas_count = (int)std::ceil(texture_surface / atlas_surface);

			stream << "Texture surface    : " << texture_surface << std::endl;
			stream << "Atlas surface      : " << atlas_surface   << std::endl;
			stream << "Best atlas count   : " << min_atlas_count << std::endl;
			stream << "Actual atlas count : " << bitmaps.size() << std::endl;
		}

		std::string Atlas::GetGeneralInformationString() const
		{
			std::ostringstream stream;
			OutputGeneralInformation(stream);
			return stream.str();
		} 























  };









// =================================================================================








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

