namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OverlayWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* OverlayWidget: a container whose children are one over the other
	*/

	class CHAOS_API OverlayWidget : public LinearComposerWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(OverlayWidget, LinearComposerWidget);

	public:

		/** override */
		virtual void SetPlacement(aabox2 const& in_placement) override;
		/** override */
		virtual WidgetSurfaceRequirement GetSurfaceRequirement() const override;
	};

#endif

}; // namespace chaos