namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportServerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportServerInterface : an interface for using some Viewport
	*/

	class CHAOS_API ViewportServerInterface
	{

	public:

		/** get current viewport */
		AutoCastable<Viewport> GetViewport();
		/** get current viewport */
		AutoConstCastable<Viewport> GetViewport() const;
		/** set current viewport */
		void SetViewport(Viewport* in_viewport);

	protected:

		/** the client that receives all  events */
		shared_ptr<Viewport> viewport;
	};

#endif

}; // namespace chaos