#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>

namespace chaos
{
  namespace BitmapAtlas
  {

    /**
    * FIBITMAPDeleter : deleter for FIBITMAP
    */

    struct FIBITMAPDeleter
    {
      void operator ()(FIBITMAP * bitmap) { FreeImage_Unload(bitmap); }
    };

    using unique_bitmap_ptr = std::unique_ptr<FIBITMAP, FIBITMAPDeleter>;

    /**
    * BitmapEntry : represents an Base Bitmap entry in the atlas. Contained in a BitmapSet
    */

    class BitmapEntry : public NamedObject
    {
    public:
      /** the atlas in which it is stored in result */
      int bitmap_index{ -1 };
      /** the top-left corner of the bitmap */
      int x{ 0 };
      /** the top-left corner of the bitmap */
      int y{ 0 };
      /** the size of the bitmap (beware, 2 x padding must be add for correct result) */
      int width{ 0 };
      /** the size of the bitmap (beware, 2 x padding must be add for correct result) */
      int height{ 0 };
    };

    /**
    * CharacterEntry : represents a Character entry in the atlas. Contained in a CharacterSet. It is a BitmapEntry with additionnal information
    */

    class CharacterEntry : public BitmapEntry
    {
    public:
      FT_Vector advance{ 0, 0 };
      int       bitmap_left{ 0 }; // from 'CharacterMetrics' class
      int       bitmap_top{ 0 };
    };

    /**
    * BitmapSet : this is a named group of Bitmaps (BitmapEntry)
    */

    class BitmapSet : public NamedObject
    {
      friend class AtlasBase;

    public:

      /** gets an entry by its name */
      BitmapEntry const * GetEntry(char const * name) const;
      /** gets an entry by its tag */
      BitmapEntry const * GetEntry(TagType tag) const;

      /** the bitmap contained in the bitmap set */
      std::vector<BitmapEntry> elements;
    };

    /**
    * CharacterSet : this is a named group of Characters (CharacterEntry)
    */

    class CharacterSet : public NamedObject
    {
      friend class AtlasBase;

    public:

      /** gets an entry by its name */
      CharacterEntry const * GetEntry(char const * name) const;
      /** gets an entry by its tag */
      CharacterEntry const * GetEntry(TagType tag) const;

      /** the max bitmap size in the set */
      int max_character_width{ 0 };
      /** the max bitmap size in the set */
      int max_character_height{ 0 };
      /** the face ascender */
      int ascender{ 0 };
      /** the face descender */
      int descender{ 0 };
      /** the maximum height of a glyph */
      int face_height{ 0 };
      /** the glyph contained in the character set */
      std::vector<CharacterEntry> elements;
    };

    /**
    * Some JSON utility functions
    */

    void SaveIntoJSON(NamedObject const & entry, nlohmann::json & json_entry);

    void LoadFromJSON(NamedObject & entry, nlohmann::json const & json_entry);

    void SaveIntoJSON(BitmapEntry const & entry, nlohmann::json & json_entry);

    void LoadFromJSON(BitmapEntry & entry, nlohmann::json const & json_entry);

    void SaveIntoJSON(CharacterEntry const & entry, nlohmann::json & json_entry);

    void LoadFromJSON(CharacterEntry & entry, nlohmann::json const & json_entry);

    void SaveIntoJSON(BitmapSet const & entry, nlohmann::json & json_entry);

    void LoadFromJSON(BitmapSet & entry, nlohmann::json const & json_entry);

    void SaveIntoJSON(CharacterSet const & entry, nlohmann::json & json_entry);

    void LoadFromJSON(CharacterSet & entry, nlohmann::json const & json_entry);

    /**
    * AtlasBase : base class for Atlas and TextureArrayAtlas
    */

    class AtlasBase
    {

      friend class AtlasGenerator;
      friend class AtlasHTMLGenerator;
      friend class TextureArrayAtlas;

    public:

      /** the destructor */
      virtual ~AtlasBase() = default;
      /** the clearing method */
      virtual void Clear();
      /** Get a bitmap set */
      BitmapSet const * GetBitmapSet(char const * name) const;
      /** Get a bitmap set */
      BitmapSet const * GetBitmapSet(TagType tag) const;
      /** Get a character set */
      CharacterSet const * GetCharacterSet(char const * name) const;
      /** Get a character set */
      CharacterSet const * GetCharacterSet(TagType tag) const;

      /** get the number of bitmap to hold the atlas */
      size_t GetBitmapCount() const { return atlas_count; }
      /** get the size of bitmaps composing the atlas */
      glm::ivec2 GetAtlasDimension() const { return dimension; }

      /** returns the used surface for a bitmap */
      float ComputeSurface(int bitmap_index) const;

      /** display information */
      void OutputInfo(std::ostream & stream) const;
      /** display information about one named element */
      static void OutputInfo(NamedObject const & entry, std::ostream & stream);
      /** display information about one bitmap entry */
      static void OutputInfo(BitmapEntry const & entry, std::ostream & stream);
      /** display information about one character entry */
      static void OutputInfo(CharacterEntry const & entry, std::ostream & stream);

      /** display information */
      std::string GetInfoString() const;
      /** display information about one named object */
      static std::string GetInfoString(NamedObject const & entry);
      /** display information about one bitmap entry */
      static std::string GetInfoString(BitmapEntry const & entry);
      /** display information about one character entry */
      static std::string GetInfoString(CharacterEntry const & entry);

      /** returns the bitmap sets contained in the atlas */
      std::vector<std::unique_ptr<BitmapSet>> const & GetBitmapSets() const { return bitmap_sets; }
      /** returns the character set contained in the atlas */
      std::vector<std::unique_ptr<CharacterSet>> const & GetCharacterSets() const { return character_sets; }

    protected:

      /** get a string with the general information */
      std::string GetGeneralInformationString() const;
      /** get a string with the surface occupation of all atlas */
      std::string GetAtlasSpaceOccupationString() const;
      /** get a string with the surface occupation of one atlas */
      std::string GetAtlasSpaceOccupationString(int bitmap_index) const;
      /** display the surface occupation of all atlas */
      void OutputAtlasSpaceOccupation(std::ostream & stream = std::cout) const;
      /** display the surface occupation of all atlas */
      void OutputAtlasSpaceOccupation(int bitmap_index, std::ostream & stream = std::cout) const;
      /** display the general information if the atlas */
      void OutputGeneralInformation(std::ostream & stream = std::cout) const;

    protected:

      /** atlas count */
      int atlas_count{ 0 };
      /** atlas dimension */
      glm::ivec2 dimension{ 0,0 };
      /** the bitmap sets contained in the atlas */
      std::vector<std::unique_ptr<BitmapSet>> bitmap_sets;
      /** the character sets contained in the atlas */
      std::vector<std::unique_ptr<CharacterSet>> character_sets;
    };

    /**
    * Atlas : a group of bitmap and characters, ordered in named set (BitmapSet & CharacterSet)
    */

    class Atlas : public AtlasBase
    {
      friend class AtlasGenerator;

    public:

      /** the clearing method */
      virtual void Clear() override;

      /** get the number of bitmap to hold the atlas */
      size_t GetBitmapCount() const { return bitmaps.size(); }

      /** load an atlas from an index file */
      bool LoadAtlas(boost::filesystem::path const & filename);
      /** function to save the results */
      bool SaveAtlas(boost::filesystem::path const & filename) const;

      /** returns the bitmaps contained in the atlas */
      std::vector<unique_bitmap_ptr> const & GetBitmaps() const { return bitmaps; }

    protected:

      /** load an atlas from a json object */
      bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);
      /** function to save bitmaps */
      bool SaveAtlasBitmaps(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;
      /** function to save contents */
      bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;
      /** split a filename into DIRECTORY, INDEX_FILENAME and BITMAP prefix path */
      void SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const;
      /** get the name of a bitmap */
      boost::filesystem::path GetBitmapFilename(FREE_IMAGE_FORMAT image_format, boost::filesystem::path bitmap_filename, int index) const;

    protected:

      /** the bitmaps contained in the atlas */
      std::vector<unique_bitmap_ptr> bitmaps;
    };
  };
};
