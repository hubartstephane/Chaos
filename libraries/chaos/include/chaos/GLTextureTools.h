#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUFileResource.h>
#include <chaos/FilePath.h>
#include <chaos/GLPixelFormat.h>

namespace chaos
{

	/**
	* GenTextureParameters : some parameters to load textures
	**/

	class GenTextureParameters
	{
	public:

		/** parameters for glTexParameteri(...) */
		GLenum wrap_s = GL_REPEAT;
		/** parameters for glTexParameteri(...) */
		GLenum wrap_t = GL_REPEAT;
		/** parameters for glTexParameteri(...) */
		GLenum wrap_r = GL_REPEAT;
		/** parameters for glTexParameteri(...) */
		GLenum mag_filter = GL_LINEAR;
		/** parameters for glTexParameteri(...) */
		GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR;
		/** parameters for glTexImageXX(...) */
		GLint  level = 0;
		/** parameters for glTexImageXX(...) */
		GLint  border = 0;
		/** enable build mipmaps */
		bool build_mipmaps = true;
		/** enable the texture to be used has rectangular instead of GL_TEXTURE_1D or GL_TEXTURE_2D */
		bool rectangle_texture = false;
	};


	/**
	* GLTextureTools : used to have some generic functions for OpenGL
	**/

	class GLTextureTools
	{
	public:

		/** get buffer containing the texture data */
		static char * GetTextureImage(GLuint texture_id, GLint level, ImageDescription & desc);
		/** get the pixel format of the texture */
		static PixelFormat GetTexturePixelFormat(GLuint texture_id, GLint level);
		/** get the pixel format from an internal format */
		static PixelFormat GetPixelFormat(GLenum internal_format);

		/** returns true whether type is an array texture type */
		static bool IsArrayTextureType(GLenum type);
		/** returns true whether type is not an array texture type */
		static bool IsFlatTextureType(GLenum type);

		/** transform a texture type into an array type */
		static GLenum ToArrayTextureType(GLenum type);
		/** transform a texture type into a flat type */
		static GLenum ToFlatTextureType(GLenum type);

		/** returns the maximum number of mipmap */
		static int GetMipmapLevelCount(int width, int height);
		static int GetMipmapLevelCount(int width);
		/** Get Format/Internal Format pair from the description */
		static GLPixelFormat GetGLPixelFormat(PixelFormat const & pixel_format);

		/** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE) from dimension */
		static GLenum GetTextureTargetFromSize(int width, int height, bool rectangle_texture);

		/** prepare store parameters */
		static char * PrepareGLTextureTransfert(ImageDescription const & desc);

		/** utility function for texture loading */
		static void GenTextureApplyParameters(GLuint texture_id, TextureDescription const & texture_description, GenTextureParameters const & parameters);
	};

}; // namespace chaos
