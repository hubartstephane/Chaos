#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{

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
   * TextureAtlasEntry : class to represents an entry inside the atlas
   */

  class TextureAtlasEntry
  {
  public:

    /** the name of the texture */
    std::string filename;   
    /** the atlas in which it is stored in result */
    size_t      atlas;
    /** the top-left corner of the texture (beware, padding must be add for correct result) */
    int         x;
    /** the top-left corner of the texture (beware, padding must be add for correct result) */
    int         y;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         width;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         height;
    /** the bitmap */
    FIBITMAP  * bitmap;
#if _DEBUG
    /** the position at which it has been inserted inside an atlas */
    size_t      order_of_insertion;
#endif

  public:

    /** returns the rectangle corresponding to the texture */
    AtlasRectangle GetRectangle() const
    {
      AtlasRectangle result;
      result.x      = x;
      result.y      = y;
      result.width  = width;
      result.height = height;
      return result; 
    }
  };

  /**
   * TextureAtlasData : the set of textures to be given to an AtlasCreator (both input and output)
   */

  class TextureAtlasData
  {
  public:

    /** constructor */
    TextureAtlasData(bool in_debug_mode = false): debug_mode(in_debug_mode){}
    /** destructor */
    ~TextureAtlasData(){ Clear(); }
    /** Set the object in debug mode */ 
    void SetDebugMode(bool in_debug_mode = true);

    /** insert multiple texture before computation */
    bool AddTextureFilesFromDirectory(boost::filesystem::path const & path);
    /** insert multiple texture before computation */
    bool AddTextureFilesFromDirectory(char const * filename);
    /** insert a texture before computation */
    bool AddTextureFile(boost::filesystem::path const & path);
    /** insert a texture before computation */
    bool AddTextureFile(char const * filename);
    /** reset result */
    void ResetResult();
    /** returns the surface for an atlas */
    float ComputeSurface(size_t atlas_index, int padding) const;
    /** clear all the textures */
    void Clear();
    /** display information about all textures */
    void OutputTextureInfo(std::ostream & stream, int padding) const;
    /** display information about one texture */
    void OutputTextureInfo(TextureAtlasEntry const & texture, std::ostream & stream, int padding) const;

    /** display information about all textures */
    std::string GetTextureInfoString(int padding) const;
    /** display information about one texture */
    std::string GetTextureInfoString(TextureAtlasEntry const & texture, int padding) const;

  public:
    /** atlas in debug mode */
    bool debug_mode;
    /** all the textures */
    std::vector<TextureAtlasEntry> textures;
  };

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
   * TextureAtlasCreatorBase : base class for atlas creation
   */

  class TextureAtlasCreatorBase
  {
  public:

    /** constructor */     
    TextureAtlasCreatorBase() : width(0), height(0), padding(0), data(nullptr){}
    /** destructor */     
    virtual ~TextureAtlasCreatorBase(){}
    /** returns the number of textures needed for the atlas */
    virtual size_t GetAtlasCount() const;
    /** reset the object */
    virtual void Clear();    
    /** compute all texture positions */
    bool ComputeResult(TextureAtlasData & in_data, int in_width, int in_height, int in_padding);
    /** returns the box for the atlas */
    AtlasRectangle GetAtlasRectangle() const;

    /** create an XML document and output debug information */
    tinyxml2::XMLDocument * OutputToHTMLDocument(TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** create an XML document and output debug information */
    void OutputToHTMLDocument(tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** output the atlas trees in HTML format */
    bool OutputToHTMLFile(boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;
    /** output the atlas trees in HTML format */
    bool OutputToHTMLFile(char const * filename, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams()) const;

    /** function to save the results */
    bool SaveResults(char const * pattern) const;

  protected:

    /** get a string with the general information */
    std::string GetGeneralInformationString() const;
    /** get a string with the surface occupation of all atlas */
    std::string GetAtlasSpaceOccupationString() const;
    /** get a string with the surface occupation of one atlas */
    std::string GetAtlasSpaceOccupationString(size_t atlas_index) const;

    /** add padding to a rectangle */
    AtlasRectangle AddPadding(AtlasRectangle const & r) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(std::ostream & stream = std::cout) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(size_t atlas_index, std::ostream & stream = std::cout) const;
    /** display the general information if the atlas */
    void OutputGeneralInformation(std::ostream & stream = std::cout) const;
    /** test whether there is an intersection between each pair of textures in an atlas */
    bool EnsureValid(std::ostream & stream = std::cout) const;
    /** test whether there is an intersection between each pair of textures in an atlas */
    bool HasInterctingTexture(size_t atlas_index, AtlasRectangle const & r) const;
    /** the effective function to do the computation */
    virtual bool DoComputeResult();
    /** multiply an integer with a float (two conversions) */
    static int MultDimension(int a, float b)
    {
      return (int)(((float)a) * b);
    }
    /** helper function to update texture data */
    size_t UpdateTexture(TextureAtlasEntry & new_texture, size_t atlas, int x, int y);
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
      std::vector<size_t> result = CreateIndirectionTable(data->textures.size());
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
    /** the result */
    TextureAtlasData   * data;
  };

  /**
   * TextureAtlasCreator : the "final" implementation of the AtlasCreator
   *                       each time a texture is inserted, the space is split along 4 axis
   *                       this creates a grid of points that serve to new positions for inserting textures ...
   *                       it select the best position as the one that minimize space at left, right, top and bottom
   */

  class TextureAtlasCreator : public TextureAtlasCreatorBase
  {
    /** an definition is a set of vertical and horizontal lines that split the space */
    class AtlasDefinition 
    {
    public:
      std::vector<int> split_x;
      std::vector<int> split_y;
    };

  public:

    /** returns the number of atlas */
    virtual size_t GetAtlasCount() const;
    /** clear the results */
    virtual void Clear();
    /** create an atlas from a directory into another directory */
    static bool CreateAtlasFromDirectory(char const * src_dir, char const * dst_pattern, int atlas_width, int atlas_height, int atlas_padding);

  protected:

    /** an utility function that gets a score for a rectangle */
    float GetAdjacentSurface(TextureAtlasEntry const & texture, AtlasDefinition const & atlas, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const;
    /** returns the position (if any) in an atlas withe the best score */
    float FindBestPositionInAtlas(TextureAtlasEntry const & texture, AtlasDefinition const & atlas, int & x, int & y) const;
    /** insert an integer in a vector. keep it ordered */
    void InsertOrdered(std::vector<int> & v, int value);
    /** insert a texture in an atlas definition */
    void InsertTextureInAtlas(TextureAtlasEntry & texture, AtlasDefinition & atlas, int x, int y);
    /** the computation method */
    virtual bool DoComputeResult();

  protected:
    /** all definitions */
    std::vector<AtlasDefinition> atlas_definitions;
  };
};
