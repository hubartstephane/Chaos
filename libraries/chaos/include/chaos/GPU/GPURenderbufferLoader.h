namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderbufferLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPURenderbufferLoader : used to have some generic functions for OpenGL
	**/

	class CHAOS_API GPURenderbufferLoader
	{
	public:

		/** create a renderbuffer */
		GPURenderbuffer* GenRenderbufferObject(PixelFormat const& pixel_format, glm::ivec2 const& size);
	};

#endif

}; // namespace chaos