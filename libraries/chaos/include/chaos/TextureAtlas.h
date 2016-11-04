#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObjectVector.h>
#include <chaos/NamedObject.h>

namespace chaos
{
  namespace TextureAtlasx
  {
    class BitmapEntry : public NamedObject
    {
		public:
      /** the atlas in which it is stored in result */
			int bitmap_index {-1};
      /** the top-left corner of the bitmap */
      int x {0};
      /** the top-left corner of the bitmap */
      int y {0};
      /** the size of the bitmap (beware, 2 x padding must be add for correct result) */
      int width {0};
      /** the size of the bitmap (beware, 2 x padding must be add for correct result) */
      int height {0};
    };

    class CharacterEntry : public BitmapEntry
    {
    public:
      FT_Vector advance {0, 0};
      int       bitmap_left {0}; // from 'CharacterMetrics' class
      int       bitmap_top {0};
    };

    class BitmapSet : public NamedObject
    {
      friend class Atlas;

    protected:

      /** destructor protected */
      virtual ~BitmapSet() = default;

    public:

      /** gets an entry by its name */
      BitmapEntry const * GetEntry(char const * name) const;

      /** the bitmap contained in the bitmap set */
      std::vector<BitmapEntry> elements;
    };

    class CharacterSet : public NamedObject
    {
      friend class Atlas;

    protected:

      /** destructor protected */
      virtual ~CharacterSet() = default;

    public:

      /** gets an entry by its name */
      CharacterEntry const * GetEntry(char const * name) const;

      /** the glyph contained in the character set */
      std::vector<CharacterEntry> elements;
    };

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

    class Atlas
    {

			friend class AtlasGenerator;
			friend class AtlasHTMLGenerator;

    public:

      /** the destructor */
      virtual ~Atlas() { Clear(); }
      /** the clearing method */
      void Clear();
      /** Get a bitmap set */
      BitmapSet const * GetBitmapSet(char const * name) const;
      /** Get a character set */
      CharacterSet const * GetCharacterSet(char const * name) const;

			/** get the number of bitmap to hold the atlas */
			size_t GetBitmapCount() const { return bitmaps.size(); }
			/** get the size of bitmaps composing the atlas */
			glm::ivec2 GetAtlasDimension() const;

			/** function to save the results */
			bool SaveAtlas(boost::filesystem::path const & filename) const;
			/** load an atlas from an index file */
			bool LoadAtlas(boost::filesystem::path const & filename);
			/** load an atlas from a json object */
			bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);

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

      /** returns the bitmaps contained in the atlas */
      std::vector<FIBITMAP *> const & GetBitmaps() const { return bitmaps; }
      /** returns the bitmap sets contained in the atlas */
      std::vector<BitmapSet *> const & GetBitmapSets() const { return bitmap_sets; }
      /** returns the character set contained in the atlas */
      std::vector<CharacterSet *> const & GetCharacterSet() const { return character_sets; }

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

			/** split a filename into DIRECTORY, INDEX_FILENAME and BITMAP prefix path */
			void SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const;
			/** get the name of a bitmap */
			boost::filesystem::path GetBitmapFilename(boost::filesystem::path bitmap_filename, int index) const;

			/** function to save bitmaps */
			bool SaveAtlasBitmaps(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;
			/** function to save contents */
			bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const;

    protected:

      /** the bitmaps contained in the atlas */
      std::vector<FIBITMAP *> bitmaps;
      /** the bitmap sets contained in the atlas */
      std::vector<BitmapSet *> bitmap_sets;
      /** the character sets contained in the atlas */
      std::vector<CharacterSet *> character_sets;
    };







    };




		// ==========================================================================


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
    size_t GetAtlasCount() const { return bitmaps.size(); }
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
    std::vector<FIBITMAP *> bitmaps;
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
