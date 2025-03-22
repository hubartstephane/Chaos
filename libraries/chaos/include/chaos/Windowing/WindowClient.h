namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowClient;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * WindowClient: a content to be plugged into a window
	 */

	class CHAOS_API WindowClient : public Object, public WindowInterface
	{
		friend class Window;

	public:

		/** get the owning window */
		AutoCastable<Window> GetWindow() { return window.get(); }
		/** get the owning window */
		AutoConstCastable<Window> GetWindow() const { return window.get();}

	protected:

		/** override */
		virtual bool OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params) override;

		/** called whenever the client is being plugged into a window */
		virtual void OnPluggedIntoWindow(Window* in_window);
		/** called whenever the client is being unplugged into a window */
		virtual void OnUnpluggedFromWindow(Window* in_window);

#if _WIN32

		/** called whenever the language is being changed */
		virtual void OnInputLanguageChanged();

#endif // #if _WIN32

		/** called whenever the window is resized */
		virtual void OnWindowResize(glm::ivec2 size);
		/** called whenever the user try to close window */
		virtual bool OnWindowClosed();
		/** called whenever a file is dropped */
		virtual void OnDropFile(int count, char const** paths);
		/** called whenever the window becomes iconified or is restored */
		virtual void OnIconifiedStateChange(bool iconified);
		/** called whenever the window gain or loose focus */
		virtual void OnFocusStateChange(bool gain_focus);
		/** called whenever a monitor is connected or disconnected */
		virtual void OnMonitorEvent(GLFWmonitor* monitor, int monitor_state);


	protected:

		/** the window owing the client */
		weak_ptr<Window> window;
	};

#endif

}; // namespace chaos