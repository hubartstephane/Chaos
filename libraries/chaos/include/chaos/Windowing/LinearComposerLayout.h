namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LinearComposerLayout;

	enum class LinearComposerLayoutMode;
	enum class LinearComposerLayoutFillMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * LinearComposerLayoutMode: indicates whether widgets are displayed normally or in reverse direction
	 */

	enum class CHAOS_API LinearComposerLayoutMode : int
	{
		NORMAL,    // left to right or top to bottom
		REVERSED   // right to left or bottom to top
	};

	/**
	 * LinearComposerLayoutFillMode: indicates how child widgets should use available size
	 */

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

	enum class CHAOS_API LinearComposerLayoutFillMode : int
	{
		EXPANDED, // viewports take as many size on their line/row as possible
		UNIFORM_PACKED, // all viewports have same size. for incomplete line/row the viewports are packed alltogether
		UNIFORM_PACKED_REVERSED, // all viewports have same size. for incomplete line/row the viewports are packed alltogether
		UNIFORM_CENTERED, // all viewports have same size. for incomplete line/row the viewports are centered
	};

	/**
	 * LinearComposerLayout: an object used to distribute some surface among child widgets
	 */

	class CHAOS_API LinearComposerLayout
	{

	public:

		/** compute the placement of a child */
		aabox2 ComputePlacement(aabox2 const& placement, size_t index, size_t count) const;

	public:

		/** the orientation of the viewports */
		Orientation orientation = Orientation::HORIZONTAL;
		/** the max number of viewports along the orientation (0 for infinite) */
		size_t max_count = 0;
		/** insertion mode */
		LinearComposerLayoutMode horizontal_mode = LinearComposerLayoutMode::NORMAL;
		/** insertion mode */
		LinearComposerLayoutMode vertical_mode = LinearComposerLayoutMode::NORMAL;
		/** whether all viewports are to have the same size even for incomplete lines/rows */
		LinearComposerLayoutFillMode fill_mode = LinearComposerLayoutFillMode::EXPANDED;
		/** some padding */
		Padding padding;
	};

#endif

}; // namespace chaos