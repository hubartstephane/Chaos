namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LinearComposerLayoutWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* LinearComposerLayoutWidget: a composer that use the span layout to compute child placement
	*/

	class CHAOS_API LinearComposerLayoutWidget : public LinearComposerWidget
	{
	public:

		/** override */
		virtual void SetPlacement(aabox2 const& in_placement) override;

		/** gets the layout for placement computation */
		virtual LinearComposerLayout GetComposerLayout() const { return {}; }
	};

#endif

}; // namespace chaos