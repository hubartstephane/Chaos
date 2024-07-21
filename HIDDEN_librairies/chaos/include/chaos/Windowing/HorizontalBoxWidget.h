namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class HorizontalBoxWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* HorizontalBoxWidget: a container whose children are horizontally aligned
	*/

	class CHAOS_API HorizontalBoxWidget : public LinearComposerLayoutWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(HorizontalBoxWidget, LinearComposerLayoutWidget);

	public:

		/** override */
		virtual LinearComposerLayout GetComposerLayout() const override;
		/** override */
		virtual WidgetSurfaceRequirement GetSurfaceRequirement() const override;
	};

#endif

}; // namespace chaos