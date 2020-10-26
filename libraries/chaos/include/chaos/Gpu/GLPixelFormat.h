#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{

	class GLPixelFormat;

}; // namespace chaos

#else

namespace chaos
{

	class GLPixelFormat
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

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


