namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TextureWrapMethod : GLenum;
	enum class TextureMagnificationFilter : GLenum;
	enum class TextureMinificationFilter : GLenum;

	class TextureWrapMethods;

	class GenTextureParameters;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * TextureWrapMethod: express how texture wrapped during sampling
	 */

	enum class TextureWrapMethod : GLenum
	{
		Repeat = GL_REPEAT,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
		MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
	};

	/**
	 * TextureMagnificationFilter: Magnification filter
	 */

	enum class TextureMagnificationFilter : GLenum
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

	/**
	 * TextureMinificationFilter: Mignification filter
	 */

	enum class TextureMinificationFilter : GLenum
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	/**
	 * TextureWrapMethods: wrapping methods along 3 axis
	 */

	class CHAOS_API TextureWrapMethods
	{
	public:

		/** wrap method along x/s axis */
		TextureWrapMethod wrap_x = TextureWrapMethod::Repeat;
		/** wrap method along y/t axis */
		TextureWrapMethod wrap_y = TextureWrapMethod::Repeat;
		/** wrap method along z/u axis */
		TextureWrapMethod wrap_z = TextureWrapMethod::Repeat;
	};

	/**
	* GenTextureParameters : some parameters to load textures
	**/

	class CHAOS_API GenTextureParameters
	{
	public:


		// TODO : take care of level and border


		/** wrapping methods */
		TextureWrapMethods wrap_methods;


		/** parameters for glTexParameteri(...) */
		TextureMagnificationFilter mag_filter = TextureMagnificationFilter::Linear;
		/** parameters for glTexParameteri(...) */
		TextureMinificationFilter min_filter = TextureMinificationFilter::LinearMipmapLinear;

		/** parameters for glTexImageXX(...) */
		GLint  level = 0;
		/** parameters for glTexImageXX(...) */
		GLint  border = 0;
		/** whether at texture creation we want to have mipmaps */
		bool reserve_mipmaps = true;
		/** enable build mipmaps */
		bool build_mipmaps = true;
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
		CHAOS_API bool IsArrayTextureType(TextureType type);
		/** returns true whether type is not an array texture type */
		CHAOS_API bool IsFlatTextureType(TextureType type);

		/** transform a texture type into an array type */
		CHAOS_API TextureType ToArrayTextureType(TextureType type);
		/** transform a texture type into a flat type */
		CHAOS_API TextureType ToFlatTextureType(TextureType type);

		/** returns the maximum number of mipmap */
		CHAOS_API int GetMipmapLevelCount(int width, int height);
		CHAOS_API int GetMipmapLevelCount(int width);
		/** Get Format/Internal Format pair from the description */
		CHAOS_API GLPixelFormat GetGLPixelFormat(PixelFormat const& pixel_format);

		/** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D) from dimension */
		CHAOS_API TextureType GetTexture2DTypeFromSize(int width, int height);

		/** prepare store parameters */
		CHAOS_API char* PrepareGLTextureTransfert(ImageDescription const& desc);

	}; // namespace GLTextureTools

#endif

}; // namespace chaos