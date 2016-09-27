#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Texture.h>
#include <chaos/GLTextureTools.h>
#include <chaos/ImageTools.h>

namespace chaos
{

  /**
  * ImageSliceRegisterEntry : an entry in the register of slices
  */

  class ImageSliceRegisterEntry
  {
  public:

    /** the image description */
    ImageDescription description;
    /** a user data that may be used later for memory releasing */
    void * user_data;
  };

  /**
   * ImageSliceRegister : utility function that serves to register a slice into a vector
   */

  class ImageSliceRegister
  {
    friend class TextureArrayGenerator;

  public:
    
    /** constructor */
    ImageSliceRegister(){}    
    /** the method to insert one slice */
    bool InsertSlice(ImageDescription & description, void * user_data = nullptr);
    /** gets the number of slices inserted */
    size_t size() const { return slices.size();}

  protected:

    /** test whether the description is valid */
    bool IsImageSliceValid(ImageDescription const & description) const;

    /** the array that contains all slices */
    std::vector<ImageSliceRegisterEntry> slices;
  };

  /**
  * ImageSliceGeneratorProxyBase : class that deserve registration of several slices
  */

  class ImageSliceGeneratorProxyBase
  {
  public:

    /** constructor */
	  ImageSliceGeneratorProxyBase(){}
    /** destructor */
    virtual ~ImageSliceGeneratorProxyBase(){}
    /** the method to override to add all slice we want */
    virtual void AddSlices(ImageSliceRegister & slice_register){}
    /** the method to override to release all slices */
    virtual void ReleaseSlices(ImageSliceRegisterEntry * slices, size_t count){}
  };

  /**
  * ImageSliceGeneratorBase : used to generate proxy for TextureArrayGeneration
  */

  class ImageSliceGeneratorBase
  {
  public:

    /** constructor */
	  ImageSliceGeneratorBase() {}
    /** destructor */
    virtual ~ImageSliceGeneratorBase() {}
    /** proxy generation method */
    virtual ImageSliceGeneratorProxyBase * CreateProxy() const = 0;
  };

  /**
  * ImageSliceGeneratorProxy : a slice generator proxy from a image 
  */

  class ImageSliceGeneratorProxy : public ImageSliceGeneratorProxyBase
  {
  public:

    /** constructor */
	  ImageSliceGeneratorProxy(FIBITMAP * in_image, bool in_release_image) :
      image(in_image),
      multi_image(nullptr),
      release_image(in_release_image) {}

	  ImageSliceGeneratorProxy(FIMULTIBITMAP * in_multi_image, bool in_release_image) :
      image(nullptr),
      multi_image(in_multi_image),
      release_image(in_release_image) {}

    /** destructor */
    ~ImageSliceGeneratorProxy();
    /** the method to override to add all slice we want */
    virtual void AddSlices(ImageSliceRegister & slice_register);
    /** the method to override to release all slices */
    virtual void ReleaseSlices(ImageSliceRegisterEntry * slices, size_t count);

  protected:

    /** the image */
    FIBITMAP * image;
    /** the image */
    FIMULTIBITMAP * multi_image;
    /** whether the image has to be released */
    bool release_image;
  };

  /**
   * ImageSliceGenerator : a slice generator from a image filename
   */

  class ImageSliceGenerator : public ImageSliceGeneratorBase
  {
  public:

    /** constructor */
	  ImageSliceGenerator(boost::filesystem::path const & in_image_path):
      image_path(in_image_path), 
      image(nullptr), 
      multi_image(nullptr),
      release_image(false){}

	  ImageSliceGenerator(FIBITMAP * in_image, bool in_release_image):
      image(in_image),
      multi_image(nullptr),
      release_image(in_release_image) 
    {
      assert(image != nullptr);
    }

	  ImageSliceGenerator(FIMULTIBITMAP * in_multi_image, bool in_release_image):
      image(nullptr),
      multi_image(in_multi_image),
      release_image(in_release_image) 
    {
      assert(multi_image != nullptr);
    }

    /** proxy generation method */
    virtual ImageSliceGeneratorProxyBase * CreateProxy() const;

  protected:

    /** path of the resource file */
    boost::filesystem::path image_path;

    /** the image */
    FIBITMAP * image;
    /** the image */
    FIMULTIBITMAP * multi_image;
    /** whether the image has to be released */
    bool release_image;
  };














  /**
   * TextureArrayGenerator : an helper class that is used to generate texture array    GL_TEXTURE_1D_ARRAY,    GL_TEXTURE_2D_ARRAY or    GL_TEXTURE_CUBE_ARRAY
   */

  class TextureArrayGenerator
  {
  public:

    class SliceInfo
    {
    public:

      /** first slice allocated for the generator */
      int first_slice;
      /** number of slice used by the generator */
      int slice_count;
    };

    class GeneratorEntry
    {
    public:
      /** the proxy that will request some slices */
      ImageSliceGeneratorProxyBase * proxy;
      /** optional where the slices are allocated */
      SliceInfo * slice_info;
    };

    /** constructor */
    TextureArrayGenerator();
    /** destructor */
    virtual ~TextureArrayGenerator();

    /** the insertion method (returns the slice considered) */
    bool AddGenerator(ImageSliceGenerator const & generator, SliceInfo * slice_info = nullptr);
    /** clean all generators */
    void Clean();
    /** generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(GenTextureParameters const & parameters = GenTextureParameters());

  protected:

    /** internal method to generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(ImageSliceRegister & slice_register, int bpp, int width, int height, GenTextureParameters const & parameters) const;

  protected:

    /** the registered element to generate */
    std::vector<GeneratorEntry> generators;
  };

};
