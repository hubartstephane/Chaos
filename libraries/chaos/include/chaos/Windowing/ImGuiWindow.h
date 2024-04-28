namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiWindow: a window that deserves to display imgui content
	 **/
	class CHAOS_API ImGuiWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImGuiWindow, Window);

	public:

		/** constructor */
		ImGuiWindow();

		/** get the content */
		AutoCastable<ImGuiObject> GetContent() { return content.get(); }
		/** get the content */
		AutoConstCastable<ImGuiObject> GetContent() const { return content.get(); }

		/** change the content */
		void SetContent(ImGuiObject* in_content);

		/** create a ImGui window with a given type */
		template<typename CONTENT_TYPE>
		static ImGuiWindow* CreateImGuiWindow()
		{
			if (ImGuiWindow* result = new ImGuiWindow)
			{
				result->SetContent(new CONTENT_TYPE);
				return result;
			}
			return nullptr;
		}

	protected:

		/** override */
		virtual void OnDrawWindowImGuiContent() override;
		/** override */
		virtual void RegisterImGuiProxies() override;

	protected:

		/** ImGui information */
		shared_ptr<ImGuiObject> content;
	};

	/**
	* RunImGuiApplication : utility template function to run an application with a single window based on some ImGui content
	*/

	template<typename CONTENT_TYPE, typename ...PARAMS>
	decltype(auto) RunImGuiApplication(int argc, char** argv, char** env, PARAMS && ...params)
	{
		class MyImGuiApplication : public WindowApplication
		{
		public:

			using WindowApplication::WindowApplication;

			virtual Window* DoCreateMainWindow(WindowCreateParams const& create_params) override
			{
				if (Window* result = CreateTypedWindow(main_window_class, main_window_placement_info, create_params, "main_window"))
				{
					if (ImGuiWindow* imgui_window = auto_cast(result))
					{
						imgui_window->SetContent(new CONTENT_TYPE);
						imgui_window->SetWindowCategory(WindowCategory::MAIN_WINDOW);
					}
					return result;
				}
				return nullptr;
			}
		};

		return RunApplication<MyImGuiApplication>(argc, argv, env, ImGuiWindow::GetStaticClass(), std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos