namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportPlacement;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The viewport position and size in its ViewportWindow
	 */
	class CHAOS_API ViewportPlacement
	{
	public:

		/** the top/left position of the viewport */
		glm::ivec2 position = { 0, 0 };
		/** the size allocated for the viewport */
		glm::ivec2 size = { 0, 0 };
	};

#endif

}; // namespace chaos