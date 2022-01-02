namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Viewport;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class Viewport : public Object, public NamedObject
	{
	public:

		/** gets the window owning his viewport */
		ViewportWindow* GetWindow() { return window; }
		/** gets the window owning his viewport */
		ViewportWindow const * GetWindow() const { return window; }

	protected:

		/** the viewport window that owns this viewport */
		ViewportWindow* window = nullptr;
	};

#endif

}; // namespace chaos