namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Widget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Widget : some kind of "virtual sub-window"
	*/

	class CHAOS_API Widget : public Object, public WindowInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(Widget, Object);

	public:

		/** gets the widget owning this */
		AutoCastable<Widget> GetParent();
		/** gets the widget owning this */
		AutoConstCastable<Widget> GetParent() const;

		/** gets the top-level window owning the widget */
		AutoCastable<Window> GetWindow();
		/** gets the top-level window owning the widget */
		AutoConstCastable<Window> GetWindow() const;

		/** get the placement */
		WidgetPlacement const& GetPlacement() const { return placement; }
		/** set the placement */
		virtual void SetPlacement(WidgetPlacement const& in_placement);

		/** get the layout */
		WidgetLayout const& GetLayout() const;
		/** set the layout */
		void SetLayout(WidgetLayout const& in_layout, bool update_placement_hierarchy = true);

		/** recompute the disposition for the whole widget hierarchy */
		void UpdatePlacementHierarchy();

	protected:

		/** tweak the input placement according to WidgetLayout */
		WidgetPlacement ApplyModifiersToPlacement(WidgetPlacement const& in_placement);

	protected:

		/** called whenever the widget is orphaned */
		virtual void OnDetachedFromParent(Widget * in_parent);
		/** called whenever the widget becomes child of another widget */
		virtual void OnAttachedToParent(Widget* in_parent);

	protected:

		/** the owning widget */
		Widget* parent = nullptr;
		/** the placement of the widget (relative to its parent) */
		WidgetPlacement placement;
		/** the layout for the widget */
		WidgetLayout layout;
	};

#endif

}; // namespace chaos