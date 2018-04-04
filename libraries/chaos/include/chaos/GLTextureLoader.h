#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Texture.h>
#include <chaos/GPUFileResource.h>
#include <chaos/FilePath.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
	/**
	* GLTextureLoader : used to have some loading texture functions for OpenGL
	**/

	class GLTextureLoader : protected GPUFileResourceFriend  // give the hability to change path and names to the resource
	{
	public:

		/** Generate a texture from a json content */
		virtual Texture * GenTextureObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an file */
    virtual Texture * GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
    virtual Texture * GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
    virtual Texture * GenTextureObject(FIBITMAP * image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a cube texture from a skybox */
    virtual Texture * GenTextureObject(SkyBoxImages const * skybox, PixelFormatMergeParams const & merge_params = PixelFormatMergeParams(), GenTextureParameters const & parameters = GenTextureParameters());

    /** Generate a texture from lambda */
    template<typename T, typename GENERATOR>
    static Texture * GenTextureObject(int width, int height, GENERATOR const & generator, GenTextureParameters const & parameters = GenTextureParameters())
    {
      Texture * result = nullptr;

      PixelFormat pixel_format = PixelFormat::GetPixelFormat<T>();

      int buffer_size = ImageTools::GetMemoryRequirementForAlignedTexture(pixel_format, width, height);

      char * buffer = new char[buffer_size];
      if (buffer != nullptr)
      {
        ImageDescription desc = ImageTools::GetImageDescriptionForAlignedTexture(pixel_format, width, height, buffer);
        generator(desc);
        result = GenTextureObject(desc, parameters);
        delete[](buffer);
      }
      return result;
    }

  protected:

    /** for cubemap texture, returns a layer index depending on the face considered */
    static int GetCubeMapLayerValueFromSkyBoxFace(int face, int level = 0);
	};

}; // namespace chaos
