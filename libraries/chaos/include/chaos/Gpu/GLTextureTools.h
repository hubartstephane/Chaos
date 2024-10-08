namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GenTextureParameters;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GenTextureParameters : some parameters to load textures
	**/

	class CHAOS_API GenTextureParameters
	{
	public:


		// TODO : take care of level and border

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
		/** whether at texture creation we want to have mipmaps */
		bool reserve_mipmaps = true;
		/** enable build mipmaps */
		bool build_mipmaps = true;
		/** enable the texture to be used has rectangular instead of GL_TEXTURE_1D or GL_TEXTURE_2D */
		bool rectangle_texture = false;
	};


	/**
	* GLTextureTools : used to have some generic functions for OpenGL
	**/

	namespace GLTextureTools
	{
		/** get buffer containing the texture data */
		CHAOS_API char* GetTextureImage(GLuint texture_id, GLint level, ImageDescription& desc);
		/** get the pixel format of the texture */
		CHAOS_API PixelFormat GetTexturePixelFormat(GLuint texture_id, GLint level);
		/** get the pixel format from an internal format */
		CHAOS_API PixelFormat GetPixelFormat(GLenum internal_format);

		/** returns true whether type is an array texture type */
		CHAOS_API bool IsArrayTextureType(GLenum type);
		/** returns true whether type is not an array texture type */
		CHAOS_API bool IsFlatTextureType(GLenum type);

		/** transform a texture type into an array type */
		CHAOS_API GLenum ToArrayTextureType(GLenum type);
		/** transform a texture type into a flat type */
		CHAOS_API GLenum ToFlatTextureType(GLenum type);

		/** returns the maximum number of mipmap */
		CHAOS_API int GetMipmapLevelCount(int width, int height);
		CHAOS_API int GetMipmapLevelCount(int width);
		/** Get Format/Internal Format pair from the description */
		CHAOS_API GLPixelFormat GetGLPixelFormat(PixelFormat const& pixel_format);

		/** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE) from dimension */
		CHAOS_API GLenum GetTextureTargetFromSize(int width, int height, bool rectangle_texture);

		/** prepare store parameters */
		CHAOS_API char* PrepareGLTextureTransfert(ImageDescription const& desc);

		/** utility function for texture loading */
		CHAOS_API void GenTextureApplyParameters(GLuint texture_id, TextureDescription const& texture_description, GenTextureParameters const& parameters);

	}; // namespace GLTextureTools

#endif

}; // namespace chaos