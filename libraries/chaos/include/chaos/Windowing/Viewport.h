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
		friend class ViewportServerInterface;

		CHAOS_DECLARE_OBJECT_CLASS(Viewport, Object);

	public:

		/** gets the server owning this client */
		AutoCastable<ViewportServerInterface> GetViewportServer();
		/** gets the server owning this client */
		AutoConstCastable<ViewportServerInterface> GetViewportServer() const;

		/** gets the top-level window owning the viewport */
		AutoCastable<ViewportServerWindow> GetServerWindow();
		/** gets the top-level window owning the viewport */
		AutoConstCastable<ViewportServerWindow> GetServerWindow() const;

		/** get the placement */
		ViewportPlacement const& GetViewportPlacement() const { return placement; }
		/** set the placement */
		virtual void SetViewportPlacement(ViewportPlacement const& in_placement);

		/** gets the viewport properties */
		ViewportProperties const& GetViewportProperties() const;
		/** change the viewport properties */
		void SetViewportProperties(ViewportProperties const& in_properties, bool update_placement_hierarchy = true);

		/** recompute the whole viewport disposition */
		void UpdateViewportPlacementHierarchy();

	protected:

		/** tweak the input placement according to ViewportProperties */
		ViewportPlacement ApplyModifiersToPlacement(ViewportPlacement const& in_placement);

	protected:

		/** called whenever the viewport is been detached from a ViewportServer */
		virtual void OnDetachedFromServer(ViewportServerInterface * in_viewport_server);
		/** called whenever the viewport is been attached to a ViewportServer */
		virtual void OnAttachedToServer(ViewportServerInterface* in_viewport_server);

	protected:

		/** the server owning this viewport */
		ViewportServerInterface* viewport_server = nullptr;
		/** the placement of the viewport (relative to its parent server) */
		ViewportPlacement placement;
		/** the properties for this viewport */
		ViewportProperties properties;
	};

#endif

}; // namespace chaos