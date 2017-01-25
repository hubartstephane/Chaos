#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Texture.h>

namespace chaos
{

	/**
	* GenTextureResult : result for gen texture methods
	**/

	class GenTextureResult
	{
	public:

		/** constructor */
		GenTextureResult() : texture_id(0) {}

		/** the id of the texture */
		GLuint             texture_id;
		/** the GL handler for the texture */
		TextureDescription texture_description;
	};

	/**
	* GenTextureParameters : some parameters to load textures
	**/

	class GenTextureParameters
	{
	public:

		/** parameters for glTexParameteri(...) */
		GLenum wrap_s{ GL_REPEAT };
		/** parameters for glTexParameteri(...) */
		GLenum wrap_t{ GL_REPEAT };
		/** parameters for glTexParameteri(...) */
		GLenum wrap_r{ GL_REPEAT };
		/** parameters for glTexParameteri(...) */
		GLenum mag_filter{ GL_LINEAR };
		/** parameters for glTexParameteri(...) */
		GLenum min_filter{ GL_LINEAR_MIPMAP_LINEAR };
		/** parameters for glTexImageXX(...) */
		GLint  level{ 0 };
		/** parameters for glTexImageXX(...) */
		GLint  border{ 0 };
		/** enable build mipmaps */
		bool build_mipmaps{ true };
		/** enable the texture to be used has rectangular instead of GL_TEXTURE_1D or GL_TEXTURE_2D */
		bool rectangle_texture{ false };
	};


	/**
	* GLTextureTools : used to have some generic functions for OpenGL
	**/

	class GLTextureTools
	{
	public:

		/** get buffer containing the texture data */
		static ImageDescription	GetTextureImage(GLuint texture_id, GLint level);

		/** returns true whether type is an array texture type */
		static bool IsArrayTextureType(GLenum type);
		/** returns true whether type is not an array texture type */
		static bool IsFlatTextureType(GLenum type);

		/** transform a texture type into an array type */
		static GLenum ToArrayTextureType(GLenum type);
		/** transform a texture type into a flat type */
		static GLenum ToFlatTextureType(GLenum type);

		/** Generate a 1D/2D/rectangle texture from an file */
		static GenTextureResult GenTexture(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
		static GenTextureResult GenTexture(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
		static GenTextureResult GenTexture(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a cube texture from a skybox */
		static GenTextureResult GenTexture(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());

		/** Generate a texture from lambda */
		template<typename T>
		static GenTextureResult GenTexture(int width, std::function<void(ImageDescription const &, T *)> const & generator, GenTextureParameters const & parameters)
		{
			GenTextureResult result;

			T * buffer = new T[width];
			if (buffer != nullptr)
			{
				std::pair<int, int> pixel_format = ImageDescription::GetPixelFormat<T>();

				ImageDescription desc(buffer, width, 1, pixel_format.first, pixel_format.second, 0);
				generator(desc, buffer);
				result = GenTexture(desc, parameters);
				delete[](buffer);
			}
			return result;
		}

		template<typename T>
		GenTextureResult GenTexture(int width, int height, std::function<void(ImageDescription const &, T *)> const & generator, GenTextureParameters const & parameters)
		{
			GenTextureResult result;

			T * buffer = new T[width * height];
			if (buffer != nullptr)
			{
				std::pair<int, int> pixel_format = ImageDescription::GetPixelFormat<T>();

				ImageDescription desc(buffer, width, height, pixel_format.first, pixel_format.second, 0);
				generator(desc, buffer);
				result = GenTexture(desc, parameters);
				delete[](buffer);
			}
			return result;
		}

		/** Generate a 1D/2D/rectangle texture from an file */
		static boost::intrusive_ptr<Texture> GenTextureObject(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
		static boost::intrusive_ptr<Texture> GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a 1D/2D/rectangle texture from an image */
		static boost::intrusive_ptr<Texture> GenTextureObject(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
		/** Generate a cube texture from a skybox */
		static boost::intrusive_ptr<Texture> GenTextureObject(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());

		/** Generate a texture from lambda */
		template<typename T>
		static boost::intrusive_ptr<Texture> GenTextureObject(int width, std::function<void(ImageDescription const &, T *)> const & generator, GenTextureParameters const & parameters)
		{
			GenTextureResult result = GenTexture(width, generator, parameters);
			if (result.texture_id > 0)
				return new Texture(result.texture_id, result.texture_description);
			return nullptr;
		}

		template<typename T>
		boost::intrusive_ptr<Texture> GLTextureTools::GenTextureObject(int width, int height, std::function<void(ImageDescription const &, T *)> const & generator, GenTextureParameters const & parameters)
		{
			GenTextureResult result = GenTexture(width, height, generator, parameters);
			if (result.texture_id > 0)
				return new Texture(result.texture_id, result.texture_description);
			return nullptr;
		}


		/** returns the maximum number of mipmap */
		static int GetMipmapLevelCount(int width, int height);
		static int GetMipmapLevelCount(int width);
		/** for cubemap texture, returns a layer index depending on the face considered */
		static int GetLayerValueFromCubeMapFace(GLenum face, int level = 0);
		/** Get Format/Internal Format pair from BPP */
		static std::pair<GLenum, GLenum> GetTextureFormats(int component_type, int component_count);

		/** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE) from dimension */
		static GLenum GetTextureTargetFromSize(int width, int height, bool rectangle_texture);

		/** utility function for texture loading */
		static void GenTextureApplyParameters(GenTextureResult const & result, GenTextureParameters const & parameters);
	};

}; // namespace chaos
