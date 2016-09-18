#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

#if 0

  /**
   * FontAtlasEntry : class to represents an entry inside the font atlas
   */

  class FontAtlasEntry
  {
  public:

    /** the name of the texture */
    std::string filename;   
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

  /**
   * FontAtlas 
   */

  class FontAtlas
  {

  public:

    /** constructor */
    FontAtlas() {}
    /** destructor */
    ~FontAtlas(){ Clear(); }

    /** clear all the textures */
    void Clear();

    /** function to save the results */
    bool SaveAtlas(boost::filesystem::path const & filename) const;

    /** load an atlas from an index file */
    bool LoadAtlas(boost::filesystem::path const & filename);
    /** load an atlas from a json object */
    bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);

    /** get the number of atlas */
    size_t GetAtlasCount() const { return atlas_images.size(); }
    /** get the size of bitmap composing the atlas */
    glm::ivec2 GetAtlasDimension() const;

  protected:

    /** function to save images */
    bool SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** function to save images */
    bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;

  public:

    /** all the textures for the input */
    std::vector<FontAtlasEntry> entries;
    /** all the image for the output */
    std::vector<FIBITMAP *> atlas_images;
  };
#endif
};
