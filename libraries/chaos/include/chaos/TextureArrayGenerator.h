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
  public:
    
    /** constructor */
    ImageSliceRegister(std::vector<ImageSliceRegiterEntry> & in_slice_register):
      slice_register(in_slice_register){}
    
    /** the method to insert one slice */
    bool InsertSlice(ImageDescription & description, void * user_data = nullptr);

  protected:

    /** test whether the description is valid */
    bool IsImageSliceValid(ImageDescription const & description) const;

    /** the array that contains all slices */
    std::vector<ImageSliceRegiterEntry> & slice_register;
  };

  /**
  * ImageDescriptionGeneratorProxy : class that deserve registration of several slices
  */

  class ImageDescriptionGeneratorProxy
  {
  public:

    /** constructor */
    ImageDescriptionGeneratorProxy(){}
    /** destructor */
    virtual ~ImageDescriptionGeneratorProxy(){}
    /** the method to override to add all slice we want */
    virtual void AddSlices(ImageSliceRegister & slice_register){}
    /** the method to override to release all slices */
    virtual void ReleaseSlices(ImageSliceRegiterEntry * slices, size_t count){}
  };

  /**
  * ImageDescriptionGenerator : used to generate proxy for TextureArrayGeneration
  */

  class ImageDescriptionGenerator
  {
  public:

    /** constructor */
    ImageDescriptionGenerator() {}
    /** destructor */
    virtual ~ImageDescriptionGenerator() {}
    /** proxy generation method */
    virtual ImageDescriptionGeneratorProxy * CreateProxy() const = 0;
  };



  /*
  class ImageDescriptionGeneratorProxy : public ImageDescriptionGeneratorProxy
  {


  };
  */



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
      ImageDescriptionGeneratorProxy * proxy;
      /** optional int where the first slice allocated will be stored */
      int * slice_index;
    };

    /** constructor */
    TextureArrayGenerator();
    /** destructor */
    virtual ~TextureArrayGenerator();

    /** the insertion method (returns the slice considered) */
    int AddGenerator(ImageDescriptionGenerator const & generator, int * resulting_slice_index = nullptr);
    /** clean all generators */
    void Clean();
    /** generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(GenTextureParameters const & parameters) const;

  protected:

    /** internal method to generate the texture array */
    boost::intrusive_ptr<Texture> GenerateTexture(std::vector<ImageSliceRegiterEntry> & slices, int bpp, int width, int height, GenTextureParameters const & parameters) const;

  protected:

    /** the registered element to generate */
    std::vector<GeneratorEntry> generators;
  };

};
