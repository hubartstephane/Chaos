namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SurfaceDescription;
	class TextureDescription;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* SurfaceDescription : used to describe an openGL surface
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
	* TextureDescription : used to describe an openGL texture
	*/

	class CHAOS_API TextureDescription : public SurfaceDescription
	{
	public:

		/** the type of the texture (1D/2D/3D/RECTANGLE/CUBE) */
		GLenum type = GL_NONE;
	};

#endif

}; // namespace chaos