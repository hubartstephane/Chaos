namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class TextWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Widget : some kind of "virtual sub-window"
	*/

	class CHAOS_API TextWidget : public Widget
	{
		TextWidget(TextWidget, Widget);

	public:

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const* uniform_provider) override;

	};

#endif

}; // namespace chaos