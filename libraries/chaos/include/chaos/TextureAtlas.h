#pragma once

#include <chaos/StandardHeaders.h>
//#include <chaos/GeometryFramework.h>

namespace chaos
{

  /**
   * TextureAtlasEntry : class to represents an entry inside the atlas
   */

  class TextureAtlasEntry
  {
  public:

    /** the name of the texture */
    std::string name;   
    /** the atlas in which it is stored in result */
    size_t      atlas;
    /** the top-left corner of the texture */
    int         x;
    /** the top-left corner of the texture */
    int         y;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         width;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         height;
  };

  void SaveIntoJSON(TextureAtlasEntry const & entry, nlohmann::json & json_entry);

  void LoadFromJSON(TextureAtlasEntry & entry, nlohmann::json const & json_entry);

  /**
   * TextureAtlasBase : base class for FontAtlas / TextureAtlas
   */

  class TextureAtlasBase
  {
  public:

    /** clear all the textures */
    void Clear();
    /** get the number of atlas */
    size_t GetAtlasCount() const { return atlas_images.size(); }
    /** get the size of bitmap composing the atlas */
    glm::ivec2 GetAtlasDimension() const;

    /** function to save the results */
    bool SaveAtlas(boost::filesystem::path const & filename) const;
    /** load an atlas from an index file */
    bool LoadAtlas(boost::filesystem::path const & filename);
    /** load an atlas from a json object */
    bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);

  protected:

    /** split a filename into DIRECTORY, INDEX_FILENAME and IMAGE prefix path */
    void TextureAtlasBase::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const;
    /** get the name of an atlas image */
    boost::filesystem::path GetAtlasImageName(boost::filesystem::path image_filename, int index) const;

    /** function to save images */
    bool SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** function to save images */
    bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** save all entries inside a json object */
    virtual void SaveAtlasEntriesInIndex(nlohmann::json & json_entries) const {};

    /** load one entry from json object and insert it into entries array */
    virtual void LoadAtlasEntryFromIndex(nlohmann::json const & json_entry) {};

  public:

    /** all the image for the output */
    std::vector<FIBITMAP *> atlas_images;
  };

  /**
  * TextureAtlasTypedBase : base class for FontAtlas / TextureAtlas
  */

  template<typename ENTRY_TYPE>
  class TextureAtlasTypedBase : public TextureAtlasBase
  {
  public:

    using entry_type = ENTRY_TYPE;

    /** constructor */
    TextureAtlasTypedBase() {}
    /** destructor */
    ~TextureAtlasTypedBase() { Clear(); }

    /** clear all the textures */
    void Clear()
    {
      // super method
      TextureAtlasBase::Clear();
      // destroy the entries
      entries.empty();
    }

  protected:

    virtual void SaveAtlasEntriesInIndex(nlohmann::json & json_entries) const override
    {
      for (size_t i = 0; i < entries.size(); ++i)
      {
        auto json_entry = nlohmann::json();
        SaveIntoJSON(entries[i], json_entry);
        json_entries.push_back(json_entry);
      }
    }

    virtual void LoadAtlasEntryFromIndex(nlohmann::json const & json_entry) override
    {
      entry_type entry;
      LoadFromJSON(entry, json_entry);
      entries.push_back(entry);
    }

  public:

    /** all the textures for the input */
    std::vector<entry_type> entries;
  };

  /**
   * TextureAtlas : the result computation of TextureAtlasGenerator
   */

  class TextureAtlas : public TextureAtlasTypedBase<TextureAtlasEntry>
  {
    friend class TextureAtlasGenerator;
    friend class TextureAtlasHTMLGenerator;

  public:

    /** returns the surface for an atlas */
    float ComputeSurface(size_t atlas_index) const;

    /** display information about all textures */
    void OutputTextureInfo(std::ostream & stream) const;
    /** display information about one texture */
    void OutputTextureInfo(TextureAtlasEntry const & entry, std::ostream & stream) const;

    /** display information about all textures */
    std::string GetTextureInfoString() const;
    /** display information about one texture */
    std::string GetTextureInfoString(TextureAtlasEntry const & entry) const;

  protected:

    /** get a string with the general information */
    std::string GetGeneralInformationString() const;
    /** get a string with the surface occupation of all atlas */
    std::string GetAtlasSpaceOccupationString() const;
    /** get a string with the surface occupation of one atlas */
    std::string GetAtlasSpaceOccupationString(size_t atlas_index) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(std::ostream & stream = std::cout) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(size_t atlas_index, std::ostream & stream = std::cout) const;
    /** display the general information if the atlas */
    void OutputGeneralInformation(std::ostream & stream = std::cout) const;
  };
};
