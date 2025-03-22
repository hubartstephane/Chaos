namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SimpleWindowApplication;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* SimpleWindowApplication
	*/

	class CHAOS_API SimpleWindowApplication : public WindowApplication
	{
	public:

		/** constructor */
		SimpleWindowApplication(SubClassOf<Window> in_main_window_class = {}, WindowPlacementInfo const& in_main_window_placement_info = {}, WindowCreateParams const& in_main_window_create_params = {});

		/** gets the main window of the application */
		AutoCastable<Window> GetMainWindow() { return FindWindow("main"); }
		/** gets the main window of the application */
		AutoConstCastable<Window> GetMainWindow() const { return FindWindow("main"); }

	protected:

		/** override */
		virtual int MainBody() override;

		/** create the main window */
		virtual Window* CreateMainWindow();
		/** create the main window */
		virtual Window* DoCreateMainWindow(WindowCreateParams const& create_params);

	protected:

		/** the main window params */
		WindowCreateParams main_window_create_params;
		/** the class of the main window */
		SubClassOf<Window> main_window_class;
		/** the main window initial placement */
		WindowPlacementInfo main_window_placement_info;
	};

	/**
	* RunSimpleWindowApplication : utility template function to run a window application only from a window class
	*/

	template<typename WINDOW_TYPE, typename ...PARAMS>
	decltype(auto) RunSimpleWindowApplication(int argc, char** argv, char** env, PARAMS && ...params)
	{
		static_assert(std::is_base_of_v<Window, WINDOW_TYPE>);
		return RunApplication<SimpleWindowApplication>(argc, argv, env, WINDOW_TYPE::GetStaticClass(), std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos