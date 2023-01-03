namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class VerticalBoxWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* VerticalBoxWidget: a container whose children are vertically aligned
	*/

	class CHAOS_API VerticalBoxWidget : public LinearComposerLayoutWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(VerticalBoxWidget, LinearComposerLayoutWidget);

	public:

		/** override */
		virtual LinearComposerLayout GetComposerLayout() const override;
		/** override */
		virtual WidgetSurfaceRequirement GetSurfaceRequirement() const override;
	};

#endif

}; // namespace chaos