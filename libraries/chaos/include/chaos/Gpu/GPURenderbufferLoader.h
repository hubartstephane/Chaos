#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPURenderbufferLoader;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* GPURenderbufferLoader : used to have some generic functions for OpenGL
	**/

	class GPURenderbufferLoader
	{
	public:

		/** create a renderbuffer */
		GPURenderbuffer * GenRenderbufferObject(PixelFormat const & pixel_format, glm::ivec2 const & size);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

