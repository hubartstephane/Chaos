namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GLFWWindowHints;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GLFWWindowHints : this represents hints for GLFWwindow creation
	*/

	class CHAOS_API GLFWWindowHints
	{
	public:

		/** gives set hints to GLFW */
		void ApplyHints() const;

	public:

		/** true if the window can be resized */
		int resizable = 1;
		/** true if the window starts visible */
		int start_visible = 1;
		/** true if the window has some decoration */
		int decorated = 1;
		/** true if the window is toplevel */
		int toplevel = 0;
		/** self description */
		int focused = 0;
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
		/** self description */
		int red_bits = 8;
		/** self description */
		int green_bits = 8;
		/** self description */
		int blue_bits = 8;
		/** self description */
		int alpha_bits = 8;
	};

	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, GLFWWindowHints const& src);

	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, GLFWWindowHints& dst);

#endif

}; // namespace chaos