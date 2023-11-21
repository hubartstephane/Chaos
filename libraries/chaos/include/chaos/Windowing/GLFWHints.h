namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GLFWHints;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GLFWHints : this represents hints for the application
	*/

	class CHAOS_API GLFWHints
	{
	public:

		/** gives set hints to GLFW */
		void ApplyHints();

	public:

		/** true if we use an opengl debug context */
#if _DEBUG
		int debug_context = 1;
#else
		int debug_context = 0;
#endif
		/** the major version of opengl */
		int major_version = 4;
		/** the major version of opengl */
		int minor_version = 4;
		/** the refresh rate (only usefull in fullscreen mode) */
		int refresh_rate = 60;
		/** the opengl profile */
		int opengl_profile = GLFW_OPENGL_CORE_PROFILE;
		/** whether we want the fps to be unlimited */
		bool unlimited_fps = false;
		/** number of samples in multisamples (0 for none) */
		int samples = 0;
		/** self description */
		int double_buffer = 1;
		/** self description */
		int depth_bits = 24;
		/** self description */
		int stencil_bits = 8;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, GLFWHints const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, GLFWHints& dst);

#endif

}; // namespace chaos