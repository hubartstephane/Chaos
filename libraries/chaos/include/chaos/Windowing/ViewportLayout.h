namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportLayout;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The layout is responsible for computing the surface to reserve for all viewports owned by a ViewportWindow
	 */

	class CHAOS_API ViewportLayout : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportLayout, Object);

		friend class ViewportServerWindow;

	public:

		/** compute the position of all viewport in their viewport window */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const & window_size, size_t viewport_index, size_t viewport_count) const;

		/** gets the window using the layout */
		AutoCastable<ViewportServerWindow> GetViewportServerWindow() { return viewport_server; }
		/** gets the window using the layout */
		AutoConstCastable<ViewportServerWindow> GetViewportServerWindow() const { return viewport_server; }

		/** Update the window placements */
		void UpdateWindowViewportPlacements();

	protected:

		/** the viewport owning this layout */
		ViewportServerWindow* viewport_server = nullptr;
	};



	enum class CHAOS_API ViewportGridHorizontalFillMode : int
	{
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT
	};

	enum class CHAOS_API ViewportGridVerticalFillMode : int
	{
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP
	};

	//          max = 6
	// <----------------------->
	// +-----+-----+-----+-----+
	// |  X  |  X  |  X  |  X  | EXPANDED: all space for empty cells is occupied
	// +-----+-----+-----+-----+
	//
	//          max = 6
	// <----------------------->
	// +---+---+---+---+---+---+
	// | X | X | X | X |   |   | UNIFORM_PACKED: all cells have same size. there are packed alltogether in filling direction
	// +---+---+---+---+---+---+
	//
	//          max = 6
	// <----------------------->
	// +---+---+---+---+---+---+
	// |   | X | X | X | X |   | UNIFORM_CENTERED: all cells have same size. there are packed alltogether but centered on lined
	// +---+---+---+---+---+---+

	enum class CHAOS_API ViewportGridMode : int
	{
		EXPANDED, // viewports take as many size on their line/row as possible
		UNIFORM_PACKED, // all viewports have same size. for incomplete line/row the viewports are packed alltogether
		UNIFORM_CENTERED, // all viewports have same size. for incomplete line/row the viewports are centered
	};

	class CHAOS_API ViewportGridLayout : public ViewportLayout
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportGridLayout, ViewportLayout);

	public:

		/** override */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const override;

		/** change the max number of viewport along the orientation */
		void SetMaxViewportCount(size_t in_size, bool update_placements = true);
		/** change the orientation */
		void SetOrientation(Orientation in_orientation, bool update_placements = true);
		/** change horizontal fill mode */
		void SetHorizontalFillMode(ViewportGridHorizontalFillMode in_mode, bool update_placements = true);
		/** change vertical fill mode */
		void SetVerticalFillMode(ViewportGridVerticalFillMode in_mode, bool update_placements = true);
		/** change the mode */
		void SetMode(ViewportGridMode in_mode, bool update_placements = true);

	protected:

		/** the orientation of the viewports */
		Orientation orientation = Orientation::HORIZONTAL;
		/** the max number of viewports along the orientation (0 for infinite) */
		size_t size = 0;
		/** fill mode */
		ViewportGridHorizontalFillMode horizontal_fill_mode = ViewportGridHorizontalFillMode::LEFT_TO_RIGHT;
		/** fill mode */
		ViewportGridVerticalFillMode vertical_fill_mode = ViewportGridVerticalFillMode::TOP_TO_BOTTOM;
		/** whether all viewports are to have the same size even for incomplete lines/rows */
		ViewportGridMode mode = ViewportGridMode::EXPANDED;
		/** some padding */
		ViewportPadding padding;
	};

#endif

}; // namespace chaos