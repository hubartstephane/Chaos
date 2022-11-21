namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Viewport;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Viewport : an object that deserves to be plugged inside a ViewportServerWindow
	*/

	class CHAOS_API Viewport : public Object, public WindowInterface
	{
		friend class ViewportServerWindowInterface;

		CHAOS_DECLARE_OBJECT_CLASS(Viewport, Object);

	public:

		/** gets the server owning this client */
		AutoCastable<ViewportServerWindowInterface> GetViewportServer();
		/** gets the server owning this client */
		AutoConstCastable<ViewportServerWindowInterface> GetViewportServer() const;

	protected:

		/** called whenever the viewport is been detached from a ViewportServer */
		virtual void OnClientDetached(ViewportServerWindowInterface * in_viewport_server);
		/** called whenever the viewport is been attached to a ViewportServer */
		virtual void OnClientAttached(ViewportServerWindowInterface* in_viewport_server);

	protected:

		/** the server owning this viewport */
		ViewportServerWindowInterface* viewport_server = nullptr;
	};

#endif

}; // namespace chaos