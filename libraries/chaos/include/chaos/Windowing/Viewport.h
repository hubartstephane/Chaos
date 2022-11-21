namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Viewport;
	class ViewportComposer;

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

		/** get the placement */
		ViewportPlacement const& GetViewportPlacement() const { return placement; }
		/** set the placement */
		void SetViewportPlacement(ViewportPlacement const& in_placement) { placement = in_placement; }

	protected:

		/** called whenever the viewport is been detached from a ViewportServer */
		virtual void OnClientDetached(ViewportServerInterface * in_viewport_server);
		/** called whenever the viewport is been attached to a ViewportServer */
		virtual void OnClientAttached(ViewportServerInterface* in_viewport_server);

	protected:

		/** the server owning this viewport */
		ViewportServerInterface* viewport_server = nullptr;
		/** the placement of the viewport */
		ViewportPlacement placement;
	};






	class CHAOS_API ViewportComposer : public Viewport
	{
	public:

		/** gets the viewport layout */
		AutoCastable<ViewportLayout> GetViewportLayout();
		/** gets the viewport layout */
		AutoConstCastable<ViewportLayout> GetViewportLayout() const;

		/** change the viewport layout */
		void SetViewportLayout(ViewportLayout* in_viewport_layout);

		/** update the surface allocation for all viewports */
		void ComputeViewportPlacements(glm::ivec2 size);

	protected:

		/** the viewports */
		std::vector<shared_ptr<Viewport>> viewports;
		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;
	};



#endif

}; // namespace chaos