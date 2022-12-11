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

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const* uniform_provider) override;

		/** gets the widget owning this */
		AutoCastable<Widget> GetParentWidget();
		/** gets the widget owning this */
		AutoConstCastable<Widget> GetParentWidget() const;

		/** gets the top-level window owning the widget */
		AutoCastable<Window> GetWindow();
		/** gets the top-level window owning the widget */
		AutoConstCastable<Window> GetWindow() const;

		/** get the placement */
		aabox2 const& GetPlacement() const { return placement; }
		/** set the placement */
		virtual void SetPlacement(aabox2 const& in_placement);

		/** get the layout */
		WidgetLayout const& GetLayout() const;
		/** set the layout */
		void SetLayout(WidgetLayout const& in_layout, bool immediate_update = true);

		/** gets the number of children */
		size_t GetChildWidgetCount() const;
		/** gets a child by its index */
		AutoCastable<Widget> GetChildWidget(size_t index);
		/** gets a child by its index */
		AutoConstCastable<Widget> GetChildWidget(size_t index) const;
		/** find child by name */
		AutoCastable<Widget> FindChildWidget(ObjectRequest request);
		/** find child by name */
		AutoConstCastable<Widget> FindChildWidget(ObjectRequest request) const;

		/** find descendant child by name */
		AutoCastable<Widget> FindDescendantWidget(ObjectRequest request);
		/** find descendant child by name */
		AutoConstCastable<Widget> FindDescendantWidget(ObjectRequest request) const;

		/** recompute the disposition for the whole widget hierarchy */
		void UpdatePlacementHierarchy(bool immediate_update);

		/** returns whether the disposition should be updated */
		bool IsUpdatePlacementHierarchyRequired() const { return placement_hierarchy_update_required; }

	protected:

		/** override */
		virtual bool DoTick(float delta_time);

		/** tweak the input placement according to WidgetLayout */
		aabox2 ApplyModifiersToPlacement(aabox2 const& in_placement) const;

	protected:

		/** called whenever the widget is orphaned */
		virtual void OnDetachedFromParent(Widget * in_parent);
		/** called whenever the widget becomes child of another widget */
		virtual void OnAttachedToParent(Widget* in_parent);

		/** utility method to update widget parent and call dedicated callback */
		void AttachChild(Widget* widget, bool immediate_update);
		/** utility method to update widget parent and call dedicated callback */
		void DetachChild(Widget* widget, bool immediate_update);

	protected:

		/** the owning widget */
		Widget* parent = nullptr;
		/** the placement of the widget (relative to its parent) */
		aabox2 placement;
		/** the layout for the widget */
		WidgetLayout layout;

		/** the child widgets */
		std::vector<shared_ptr<Widget>> child_widgets;

		/** whether layout should be recomputed */
		bool placement_hierarchy_update_required = false;
	};

#endif

}; // namespace chaos