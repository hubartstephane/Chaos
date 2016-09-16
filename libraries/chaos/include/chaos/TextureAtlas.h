#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{

  /**
   * TextureAtlasHTMLOutputParams : the parameters for HTML output of atlas 
   */

  class TextureAtlasHTMLOutputParams
  {
  public:

    /** constructor */
    TextureAtlasHTMLOutputParams():
      show_header(true), 
      show_atlas_header(true),
      show_textures(true), 
      show_textures_names(true),
      auto_refresh(true),
      texture_scale(-1.0f){}

    /** show the HTML header */
    bool  show_header;
    /** show the atlas header */
    bool  show_atlas_header;
    /** show the textures */
    bool  show_textures;
    /** show the texture names */
    bool  show_textures_names;
    /** should the document refresh itself ("http-equiv", "refresh") every 2 seconds */
    bool  auto_refresh;
    /** a scale factor to be applied for the rendering of the textures (-1 for auto factor dependent on the atlas size) */
    float texture_scale;
  };

  /**
   * AtlasRectangle : a class to represents rectangles
   */

  class AtlasRectangle
  {
  public:
    /** the top-left corner of the rectangle */
    int x;
    /** the top-left corner of the rectangle */
    int y;
    /** the size of the rectangle */
    int width;
    /** the size of the rectangle */
    int height;
    /** equality test */
    bool operator == (AtlasRectangle const & src) const
    {
      return (x == src.x) && (y == src.y) && (width == src.width) && (height == src.height);
    }
    /** returns true whenever big does fully contains this */
    bool IsFullyInside(AtlasRectangle const & big) const
    {
      if (x < big.x)
        return false;
      if (x + width > big.x + big.width)
        return false;
      if (y < big.y)
        return false;
      if (y + height > big.y + big.height)
        return false;
      return true;
    }
    /** returns true whenever there is an intersection between this and big */
    bool IsIntersecting(AtlasRectangle const & big) const
    {
      if (x >= big.x + big.width)
        return false;
      if (x + width <= big.x)
        return false;
      if (y >= big.y + big.height)
        return false;
      if (y + height <= big.y)
        return false;
      return true;
    }
  };


  /**
  * TextureAtlasInputEntry : class to represents an entry inside the atlas
  */

  class TextureAtlasInputEntry
  {
  public:

    /** the name of the texture */
    std::string filename;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         width;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         height;
    /** the bitmap */
    FIBITMAP  * bitmap;
  };

  /**
   * TextureAtlasEntry : class to represents an entry inside the atlas
   */

  class TextureAtlasEntry
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
   * TextureAtlasInput : the set of textures to be given to an AtlasCreator (input)
   */

  class TextureAtlasInput
  {
    friend class TextureAtlasGenerator;

  public:

    /** constructor */
    TextureAtlasInput() {}
    /** destructor */
    ~TextureAtlasInput(){ Clear(); }

    /** clear all the textures */
    void Clear();
    /** insert multiple texture before computation */
    bool AddTextureFilesFromDirectory(boost::filesystem::path const & path);
    /** insert a texture before computation */
    bool AddTextureFile(boost::filesystem::path const & path);
    /** insert a texture before computation */
    bool AddTextureFile(char const * filename);
    /** insert an image inside the atlas */
    bool AddImageSource(char const * filename, FIBITMAP * image);
    /** insert an image inside the atlas */
    bool AddFakeImageSource(char const * filename);

  protected:

    /** all the textures for the input */
    std::vector<TextureAtlasInputEntry> entries;
  };


  /**
   * TextureAtlas : the result computation of TextureAtlasGenerator
   */

  class TextureAtlas
  {
    friend class TextureAtlasGenerator;

  public:

    /** constructor */
    TextureAtlas() {}
    /** destructor */
    ~TextureAtlas(){ Clear(); }

    /** returns the surface for an atlas */
    float ComputeSurface(size_t atlas_index) const;
    /** clear all the textures */
    void Clear();
    /** display information about all textures */
    void OutputTextureInfo(std::ostream & stream) const;
    /** display information about one texture */
    void OutputTextureInfo(TextureAtlasEntry const & entry, std::ostream & stream) const;

    /** display information about all textures */
    std::string GetTextureInfoString() const;
    /** display information about one texture */
    std::string GetTextureInfoString(TextureAtlasEntry const & entry) const;

    /** function to save the results */
    bool SaveAtlas(boost::filesystem::path const & filename) const;

    /** load an atlas from an index file */
    bool LoadAtlas(boost::filesystem::path const & filename);
    /** load an atlas from a json object */
    bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);

    /** create an XML document and output debug information */
    tinyxml2::XMLDocument * OutputToHTMLDocument(TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** create an XML document and output debug information */
    void OutputToHTMLDocument(tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** output the atlas trees in HTML format */
    bool OutputToHTMLFile(boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** output the atlas trees in HTML format */
    bool OutputToHTMLFile(char const * filename, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;

    /** get the number of atlas */
    size_t GetAtlasCount() const { return atlas_images.size(); }
    /** get the size of bitmap composing the atlas */
    glm::ivec2 GetAtlasDimension() const;

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

    /** get the name of an atlas image */
    boost::filesystem::path GetAtlasImageName(boost::filesystem::path image_filename, int index) const;
    /** split a filename into DIRECTORY, INDEX_FILENAME and IMAGE prefix path */
    void SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const;

    /** function to save images */
    bool SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** function to save images */
    bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;

  public:

    /** all the textures for the input */
    std::vector<TextureAtlasEntry> entries;
    /** all the image for the output */
    std::vector<FIBITMAP *> atlas_images;
  };

  /**
  * TextureAtlasGenerator :
  *   each time a texture is inserted, the space is split along 4 axis
  *   this creates a grid of points that serve to new positions for inserting textures ...
  *   it select the best position as the one that minimize space at left, right, top and bottom
  */

  class TextureAtlasGenerator
  {
    /** an definition is a set of vertical and horizontal lines that split the space */
    class AtlasDefinition
    {
    public:
      std::vector<int> split_x;
      std::vector<int> split_y;
    };

  public:

    /** constructor */     
    TextureAtlasGenerator() : width(0), height(0), padding(0), input(nullptr), output(nullptr){}
    /** destructor */     
    virtual ~TextureAtlasGenerator(){} 
    /** compute all texture positions */
    bool ComputeResult(TextureAtlasInput & in_input, TextureAtlas & in_ouput, int in_width, int in_height, int in_padding);

    /** create an atlas from a directory into another directory */
    static bool CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, int atlas_width, int atlas_height, int atlas_padding);

    /** returns a vector with all generated Image (to be deallocated after usage) */
    std::vector<FIBITMAP *> GenerateAtlasTextures() const;

  protected:

  public:

    /** clear the results */
    void Clear();
    /** returns the box for the atlas */
    AtlasRectangle GetAtlasRectangle() const;
    /** returns the number of atlas */
    size_t GetAtlasCount() const;
    /** add padding to a rectangle */
    AtlasRectangle AddPadding(AtlasRectangle const & r) const;
    /** returns the rectangle corresponding to the texture */
    AtlasRectangle GetRectangle(TextureAtlasEntry const & entry) const;
    /** test whether there is an intersection between each pair of textures in an atlas */
    bool EnsureValidResults(std::ostream & stream = std::cout) const;
    /** test whether there is an intersection between each pair of textures in an atlas */
    bool HasInterctingTexture(size_t atlas_index, AtlasRectangle const & r) const;
    /** the effective function to do the computation */
    bool DoComputeResult();
    /** an utility function that gets a score for a rectangle */
    float GetAdjacentSurface(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const;
    /** returns the position (if any) in an atlas withe the best score */
    float FindBestPositionInAtlas(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, int & x, int & y) const;
    /** insert an integer in a vector. keep it ordered */
    void InsertOrdered(std::vector<int> & v, int value);
    /** insert a texture in an atlas definition */
    void InsertTextureInAtlas(TextureAtlasEntry & output_entry, AtlasDefinition & atlas_def, int x, int y);
    /** an utility function that returns an array with 0.. count - 1*/
    static std::vector<size_t> CreateIndirectionTable(size_t count)
    {
      std::vector<size_t> result;    
      result.reserve(count);
      for (size_t i = 0 ; i < count ; ++i)
        result.push_back(i);
      return result;
    }
    /** an utility function to generate sub series from a function */
    template<typename FUNC>
    std::vector<size_t> CreateTextureIndirectionTable(FUNC func)
    {
      std::vector<size_t> result = CreateIndirectionTable(input->entries.size());
      std::sort(result.begin(), result.end(), func);
      return result;
    }

  protected:

    /** the width of an atlas texture */
    int                  width;
    /** the height of an atlas texture */
    int                  height;
    /** some padding for the texture : should be even */
    int                  padding;
    /** the input files */
    TextureAtlasInput  * input;
    /** the result */
    TextureAtlas       * output;
    /** all definitions */
    std::vector<AtlasDefinition> atlas_definitions;
  };
};
