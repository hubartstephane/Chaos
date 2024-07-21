namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WrapBoxWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WrapBoxWidget: a container whose children are aligned in a direction and change row/colum after number of element
	*/

	class CHAOS_API WrapBoxWidget : public LinearComposerLayoutWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(WrapBoxWidget, LinearComposerLayoutWidget);

	public:

		/** override */
		virtual LinearComposerLayout GetComposerLayout() const override;
		/** override */
		virtual WidgetSurfaceRequirement GetSurfaceRequirement() const override;
	};

#endif

}; // namespace chaos