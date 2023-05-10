namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDrawableInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ImGuiDrawableInterface
	{
	public:

		/** destructor */
		virtual ~ImGuiDrawableInterface() {};
		/** the draw method */
		virtual void DrawImGui() {}
	};

#endif

}; // namespace chaos