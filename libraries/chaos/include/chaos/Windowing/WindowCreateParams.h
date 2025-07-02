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
		bool resizable = true;
		/** true if the window starts visible */
		bool start_visible = true;
		/** true if the window has some decoration */
		bool decorated = true;
		/** true if the window is toplevel */
		bool toplevel = false;
		/** self description */
		bool focused = false;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, WindowCreateParams const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, WindowCreateParams& dst);

#endif

}; // namespace chaos