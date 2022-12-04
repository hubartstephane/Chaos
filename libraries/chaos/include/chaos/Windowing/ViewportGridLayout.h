namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportGridLayout;
	class ViewportGridLayoutSettings;

	enum class ViewportGridInsertionMode;
	enum class ViewportGridFillMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API ViewportGridInsertionMode : int
	{
		NORMAL,    // left to right or top to bottom
		REVERSED   // right to left or bottom to top
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
	// | X | X | X | X |   |   | UNIFORM_PACKED: all cells have same size. there are packed alltogether in insertion direction
	// +---+---+---+---+---+---+
	//
	//          max = 6
	// <----------------------->
	// +---+---+---+---+---+---+
	// |   |   | X | X | X | X | UNIFORM_PACKED_REVERSED: all cells have same size. there are packed alltogether in insertion direction
	// +---+---+---+---+---+---+
	// 
	//          max = 6
	// <----------------------->
	// +---+---+---+---+---+---+
	// |   | X | X | X | X |   | UNIFORM_CENTERED: all cells have same size. there are packed alltogether but centered on lined
	// +---+---+---+---+---+---+

	enum class CHAOS_API ViewportGridFillMode : int
	{
		EXPANDED, // viewports take as many size on their line/row as possible
		UNIFORM_PACKED, // all viewports have same size. for incomplete line/row the viewports are packed alltogether
		UNIFORM_PACKED_REVERSED, // all viewports have same size. for incomplete line/row the viewports are packed alltogether
		UNIFORM_CENTERED, // all viewports have same size. for incomplete line/row the viewports are centered
	};

	class CHAOS_API ViewportGridLayoutSettings
	{
	public:

		/** the orientation of the viewports */
		Orientation orientation = Orientation::HORIZONTAL;
		/** the max number of viewports along the orientation (0 for infinite) */
		size_t max_count = 0;
		/** insertion mode */
		ViewportGridInsertionMode horizontal_insertion_mode = ViewportGridInsertionMode::NORMAL;
		/** insertion mode */
		ViewportGridInsertionMode vertical_insertion_mode = ViewportGridInsertionMode::NORMAL;
		/** whether all viewports are to have the same size even for incomplete lines/rows */
		ViewportGridFillMode fill_mode = ViewportGridFillMode::EXPANDED;
		/** some padding */
		Padding padding;
	};

	class CHAOS_API ViewportGridLayout : public ViewportLayout
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportGridLayout, ViewportLayout);

	public:

		/** override */
		//virtual aabox2 ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const override;

		/** change the whole settings for that layout */
		void SetSettings(ViewportGridLayoutSettings const& in_settings, bool update_placement_hierarchy = true);
		/** change the max number of viewport along the orientation */
		void SetMaxViewportCount(size_t in_max_count, bool update_placement_hierarchy = true);
		/** change the orientation */
		void SetOrientation(Orientation in_orientation, bool update_placement_hierarchy = true);
		/** change horizontal insertion mode */
		void SetHorizontalInsertionMode(ViewportGridInsertionMode in_insertion_mode, bool update_placement_hierarchy = true);
		/** change vertical insertion mode */
		void SetVerticalInsertionMode(ViewportGridInsertionMode in_insertion_mode, bool update_placement_hierarchy = true);
		/** change the fill mode */
		void SetFillMode(ViewportGridFillMode in_fill_mode, bool update_placement_hierarchy = true);

	protected:

		/** the behaviour of the viewport layout */
		ViewportGridLayoutSettings settings;
	};

#endif

}; // namespace chaos