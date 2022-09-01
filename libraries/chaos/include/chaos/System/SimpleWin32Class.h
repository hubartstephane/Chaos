namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SimpleWin32CreateParam;
	class SimpleWin32ClassBase;

	template<typename WIN_CLASS>
	class SimpleWin32Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* SimpleWin32CreateParam : a class to hold the creation parameters
	*/

	class CHAOS_API SimpleWin32CreateParam
	{
	public:

		DWORD dwExStyle = 0;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		int   x = CW_USEDEFAULT;
		int   y = CW_USEDEFAULT;
		int   nWidth = CW_USEDEFAULT;
		int   nHeight = CW_USEDEFAULT;
	};

	/**
	* SimpleWin32Window : an handler for on-the-shelve window class
	*/

	class CHAOS_API SimpleWin32ClassBase
	{
	public:

		/** constructor */
		SimpleWin32ClassBase(char const* in_window_classname);

	protected:

		/** create a HWND and bind it to the class object. Delete the class object in case of failure */
		class SimpleWin32Window* DoCreateWindowHinstance(HWND hParent, char const* window_title, class SimpleWin32Window* result, SimpleWin32CreateParam const& create_params);

	protected:

		/** the classname for the window */
		char const* window_classname;
	};

	/**
	* SimpleWin32Class : a generic class for windows
	*/

	template<typename WIN_CLASS>
	class CHAOS_API SimpleWin32Class : public SimpleWin32ClassBase
	{
	public:

		/** the type of windows */
		using window_type = WIN_CLASS;

		/** constructor */
		SimpleWin32Class(char const* in_window_classname) : SimpleWin32ClassBase(in_window_classname) {}

		/** create one window instance */
		template<typename PARAM_TYPE>
		window_type* CreateWindowHinstance(HWND hParent, char const* window_title, PARAM_TYPE const& params, SimpleWin32CreateParam create_params = SimpleWin32CreateParam())
		{
			return (window_type*)DoCreateWindowHinstance(hParent, window_title, new window_type(params), create_params);
		}
		/** create one window instance (window constructor without any parameters) */
		window_type* CreateWindowHinstance(HWND hParent, char const* window_title, SimpleWin32CreateParam create_params = SimpleWin32CreateParam())
		{
			return (window_type*)DoCreateWindowHinstance(hParent, window_title, new window_type, create_params);
		}
	};

#endif

}; // namespace chaos