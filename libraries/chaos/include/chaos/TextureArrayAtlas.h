#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/Texture.h>

namespace chaos
{
  namespace TextureArrayAtlas
  {
    class Atlas : public BitmapAtlas::AtlasBase
    {
    public:

      /** the clearing method */
      void Clear();


    public:

      boost::intrusive_ptr<Texture> texture;
    };
  };
#if 0
  namespace BitmapAtlas
  {




    /**
    * Atlas : a group of bitmap and characters, ordered in named set (BitmapSet & CharacterSet)
    */

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
      /** Get a bitmap set */
      BitmapSet const * GetBitmapSet(TagType tag) const;
      /** Get a character set */
      CharacterSet const * GetCharacterSet(char const * name) const;
      /** Get a character set */
      CharacterSet const * GetCharacterSet(TagType tag) const;

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
      void SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const;
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
#endif
};
