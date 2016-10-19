#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TextureAtlas.h>

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
  * TextureAtlasInputEntry : class to represents an entry inside the atlas
  */

  class TextureAtlasInputEntry
  {
  public:

    /** constructor */
    TextureAtlasInputEntry():
      width(0),
      height(0),
      bpp(0),
      bitmap(nullptr),
      release_bitmap(true) {}

    /** the name of the texture */
    std::string name;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         width;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         height;
    /** the bpp of the texture */
    int         bpp;
    /** the bitmap */
    FIBITMAP  * bitmap;
    /** whether the bitmap is to be destroyed at the end */
    bool        release_bitmap;
  };

  /**
   * TextureAtlasInputBase : the set of textures to be given to an AtlasCreator (input)
   */

  class TextureAtlasInputBase
  {
    friend class TextureAtlasGenerator;

  public:

    /** destructor */
    virtual ~TextureAtlasInputBase(){}

    /** insert multiple texture before computation */
    bool AddTextureFilesFromDirectory(boost::filesystem::path const & path);
    /** insert a texture before computation */
    bool AddTextureFile(boost::filesystem::path const & path, char const * name /*= nullptr*/);
    /** insert a texture before computation */
    bool AddTextureFile(char const * filename, char const * name /*= nullptr*/);
    /** insert an image inside the atlas */
    bool AddFakeImageSource(char const * name);

    /** insert an image inside the atlas */
    virtual bool AddImageSource(char const * name, FIBITMAP * image, bool release_bitmap = true) = 0;
    /** clear all the textures */
    virtual void Clear() = 0;

    /** gets the number of entries in the input */
    virtual size_t GetEntriesCount() const = 0;
  };

  /**
   * TextureAtlasInput : the set of textures to be given to an AtlasCreator (input)
   */

  class TextureAtlasInput : public TextureAtlasInputBase
  {
    friend class TextureAtlasGenerator;

  public:

    /** destructor */
    virtual ~TextureAtlasInput(){ Clear(); }    
    /** inherited */
    virtual bool AddImageSource(char const * filename, FIBITMAP * image, bool release_bitmap = true) override;
    /** inherited */
    virtual void Clear() override;
    /** gets the number of entries in the input */
    virtual size_t GetEntriesCount() const override { return entries.size();};

  protected:

    /** all the textures for the input */
    std::vector<TextureAtlasInputEntry> entries;
  };

  /**
   * TextureAtlasGeneratorParams : parameters used when generating an atlas
   */

  class TextureAtlasGeneratorParams
  {
  public:

    /** default constructor */
    TextureAtlasGeneratorParams() :
      atlas_width(0),
      atlas_height(0),
      atlas_max_width(0),
      atlas_max_height(0),
      force_power_of_2(true),
      force_square(true),
      atlas_padding(0),
      atlas_bpp(0){}

    /** initializer contructor */
    TextureAtlasGeneratorParams(int in_width, int in_height, int in_padding, int in_bpp):
      atlas_width(in_width),
      atlas_height(in_height),
      atlas_max_width(0),
      atlas_max_height(0),
      force_power_of_2(true),
      force_square(true),
      atlas_padding(in_padding),
      atlas_bpp(in_bpp) {}

    /** the width of an atlas texture */
    int atlas_width;
    /** the height of an atlas texture */
    int atlas_height;
    /** the max width of an atlas texture (if resized). 0 = no limit */
    int atlas_max_width;
    /** the max height of an atlas texture (if resized). 0 = no limit */
    int atlas_max_height;
    /** whether we have to use power of 2 values */
    bool force_power_of_2;
    /** whether we have to use square texture */
    bool force_square;
    /** some padding for the texture : should be even */
    int atlas_padding;
    /** the wanted bpp (0 for deduced from images) */
    int atlas_bpp;
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
    TextureAtlasGenerator() : input(nullptr), output(nullptr){}
    /** destructor */     
    virtual ~TextureAtlasGenerator(){} 
    /** compute all texture positions */
    bool ComputeResult(TextureAtlasInput & in_input, TextureAtlas & in_ouput, TextureAtlasGeneratorParams const & in_params = TextureAtlasGeneratorParams());

    /** create an atlas from a directory into another directory */
    static bool CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, TextureAtlasGeneratorParams const & in_params = TextureAtlasGeneratorParams());

    /** returns a vector with all generated Image (to be deallocated after usage) */
    std::vector<FIBITMAP *> GenerateAtlasTextures() const;

  protected:

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

    /** the params for generation */
    TextureAtlasGeneratorParams params;
    /** the input files */
    TextureAtlasInput  * input;
    /** the result */
    TextureAtlas       * output;
    /** all definitions */
    std::vector<AtlasDefinition> atlas_definitions;
  };
};
