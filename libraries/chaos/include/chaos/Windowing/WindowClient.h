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

		/** called whenever the client is being plugged into a window */
		virtual void OnPluggedIntoWindow(Window* in_window);
		/** called whenever the client is being unplugged into a window */
		virtual void OnUnpluggedFromWindow(Window* in_window);

	protected:

		/** the window owing the client */
		weak_ptr<Window> window;
	};

#endif

}; // namespace chaos