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
	};

	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, GLFWHints const& src);

	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, GLFWHints& dst);

#endif

}; // namespace chaos