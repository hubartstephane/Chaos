namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TextureType : GLenum;

	class SurfaceDescription;
	class TextureDescription;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* TextureType: type of texture for hardware
	*/

	enum class TextureType : GLenum
	{
		Unknown = GL_NONE,

		Texture1D = GL_TEXTURE_1D,
		Texture2D = GL_TEXTURE_2D,
		Texture3D = GL_TEXTURE_3D,
		TextureRectangle = GL_TEXTURE_RECTANGLE,
		TextureCubeMap = GL_TEXTURE_CUBE_MAP,

		Texture1DArray = GL_TEXTURE_1D_ARRAY,
		Texture2DArray = GL_TEXTURE_2D_ARRAY,
		TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY
	};

	/**
	* SurfaceDescription: used to describe an openGL surface
	*/

	class CHAOS_API SurfaceDescription
	{
	public:

		/** the format of the surface (RGB / RGBA ...) */
		PixelFormat pixel_format;
		/** the image width */
		int    width = 0;
		/** the image height */
		int    height = 0;
		/** the image depth */
		int    depth = 0;

	public:

		/** get the pixel format of the description */
		PixelFormat GetPixelFormat() const;
		/** get the size of the texture */
		glm::ivec2 GetSize(int mipmap = 0) const;
	};


	/**
	* TextureDescription: used to describe an openGL texture
	*/

	class CHAOS_API TextureDescription : public SurfaceDescription
	{
	public:

		/** the type of the texture (1D/2D/3D/RECTANGLE/CUBE ...) */
		TextureType type = TextureType::Unknown;
	};

#endif

}; // namespace chaos