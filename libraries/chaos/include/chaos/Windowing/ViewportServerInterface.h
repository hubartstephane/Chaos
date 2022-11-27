namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportServerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportServerInterface : an interface for using some Viewports
	*/

	class CHAOS_API ViewportServerInterface
	{

	public:

		/** gets the number of viewport */
		size_t GetViewportCount() const;
		/** gets a viewport by its index */
		AutoCastable<Viewport> GetViewport(size_t index);
		/** gets a viewport by its index */
		AutoConstCastable<Viewport> GetViewport(size_t index) const;
		/** find viewport by name */
		AutoCastable<Viewport> FindViewport(ObjectRequest request);
		/** find viewport by name */
		AutoConstCastable<Viewport> FindViewport(ObjectRequest request) const;

	protected:

		/** utility method called whenever a viewport is being attached */
		void AttachViewport(Viewport* viewport);
		/** utility method called whenever a viewport is being detached */
		void DetachViewport(Viewport* viewport);

	protected:

		/** the viewports */
		std::vector<shared_ptr<Viewport>> viewports;
	};

#endif

}; // namespace chaos