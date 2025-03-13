namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiApplication;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiApplication: an application that creates a window to display imgui content
	 **/

	class CHAOS_API ImGuiApplication : public WindowApplication
	{
	public:

		/** constructor */
		ImGuiApplication(ImGuiObject * in_imgui_object, SubClassOf<ImGuiWindow> in_main_window_class = {}, WindowPlacementInfo const& in_main_window_placement_info = {}, WindowCreateParams const& in_main_window_create_params = {});

		/** override */
		virtual Window* DoCreateMainWindow(WindowCreateParams const& create_params) override;

	protected:

		/** the ImGuiObject to display */
		shared_ptr<ImGuiObject> imgui_object;
	};

	/**
	* RunImGuiApplication : utility template function to run an application with a single window based on some ImGui content
	*/

	template<typename CONTENT_TYPE, typename ...PARAMS>
	decltype(auto) RunImGuiApplication(int argc, char** argv, char** env, PARAMS && ...params)
	{
		static_assert(std::is_base_of_v<ImGuiObject, CONTENT_TYPE>);
		return RunApplication<ImGuiApplication>(argc, argv, env, new CONTENT_TYPE, std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos