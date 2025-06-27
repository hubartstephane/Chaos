namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class WindowCreateParams;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowCreateParams : parameters for playing single window application
	*/

	class CHAOS_API WindowCreateParams
	{
	public:

		/** the title */
		std::string title;
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
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, WindowCreateParams const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, WindowCreateParams& dst);

#endif

}; // namespace chaos