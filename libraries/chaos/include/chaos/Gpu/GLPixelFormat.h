namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GLPixelFormat;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GLPixelFormat
	{
	public:

		/** constructor */
		GLPixelFormat() = default;
		/** constructor */
		GLPixelFormat(GLenum in_format, GLenum in_internal_format);
		/** returns true whether the pixel format is supported */
		bool IsValid() const;

	public:

		/** how the data should be stored in CPU side */
		GLenum format = GL_NONE;
		/** how the data should be stored in GPU side */
		GLenum internal_format = GL_NONE;
	};

#endif

}; // namespace chaos