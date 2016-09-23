#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Texture.h>
#include <chaos/GLTextureTools.h>
#include <chaos/ImageTools.h>

namespace chaos
{

  /**
  * ImageSliceRegiterEntry : an entry in the register of slices
  */

  class ImageSliceRegiterEntry
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
    std::vector<ImageSliceRegiterEntry> slices;
  };

  /**
  * ImageSliceGeneratorProxy : class that deserve registration of several slices
  */

  class ImageSliceGeneratorProxy
  {
  public:

    /** constructor */
    ImageSliceGeneratorProxy(){}
    /** destructor */
    virtual ~ImageSliceGeneratorProxy(){}
    /** the method to override to add all slice we want */
    virtual void AddSlices(ImageSliceRegister & slice_register){}
    /** the method to override to release all slices */
    virtual void ReleaseSlices(ImageSliceRegiterEntry * slices, size_t count){}
  };

  /**
  * ImageSliceGenerator : used to generate proxy for TextureArrayGeneration
  */

  class ImageSliceGenerator
  {
  public:

    /** constructor */
    ImageSliceGenerator() {}
    /** destructor */
    virtual ~ImageSliceGenerator() {}
    /** proxy generation method */
    virtual ImageSliceGeneratorProxy * CreateProxy() const = 0;
  };

  /**
  * ImageLoaderSliceGeneratorProxy : a slice generator proxy from a image 
  */

  class ImageLoaderSliceGeneratorProxy : public ImageSliceGeneratorProxy
  {
  public:

    /** constructor */
    ImageLoaderSliceGeneratorProxy(FIBITMAP * in_image, bool in_release_image) :
      image(in_image),
      release_image(in_release_image) {}
    /** the method to override to add all slice we want */
    virtual void AddSlices(ImageSliceRegister & slice_register);
    /** the method to override to release all slices */
    virtual void ReleaseSlices(ImageSliceRegiterEntry * slices, size_t count);

  protected:

    /** the image */
    FIBITMAP * image;
    /** whether the image has to be released */
    bool release_image;
  };

  /**
   * ImageLoaderSliceGenerator : a slice generator from a image filename
   */

  class ImageLoaderSliceGenerator : public ImageSliceGenerator
  {
  public:

    /** constructor */
    ImageLoaderSliceGenerator(boost::filesystem::path const & in_image_path):
      image_path(in_image_path){}
    /** proxy generation method */
    virtual ImageSliceGeneratorProxy * CreateProxy() const;

  protected:

    /** path of the resource file */
    boost::filesystem::path image_path;
  };

  /**
   * TextureArrayGenerator : an helper class that is used to generate texture array    GL_TEXTURE_1D_ARRAY,    GL_TEXTURE_2D_ARRAY or    GL_TEXTURE_CUBE_ARRAY
   */

  class TextureArrayGenerator
  {
  public:

    class GeneratorEntry
    {
    public:
      /** the proxy that will request some slices */
      ImageSliceGeneratorProxy * proxy;
      /** optional int where the first slice allocated will be stored */
      int * slice_index;
    };

    /** constructor */
    TextureArrayGenerator();
    /** destructor */
    virtual ~TextureArrayGenerator();

    /** the insertion method (returns the slice considered) */
    bool AddGenerator(ImageSliceGenerator const & generator, int * resulting_slice_index = nullptr);
    /** clean all generators */
    void Clean();
    /** generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(GenTextureParameters const & parameters = GenTextureParameters()) const;

  protected:

    /** internal method to generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(ImageSliceRegister & slice_register, int bpp, int width, int height, GenTextureParameters const & parameters) const;

  protected:

    /** the registered element to generate */
    std::vector<GeneratorEntry> generators;
  };

};
