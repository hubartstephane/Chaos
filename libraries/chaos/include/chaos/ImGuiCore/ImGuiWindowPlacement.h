namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiWindowPlacementType;
	class ImGuiWindowPlacement;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the reference point for placing a imgui popup */
	enum class ImGuiWindowPlacementType
	{
		ScreenCenter,
		CursorPosition,
		FreePosition
	};

	/** how to place a imgui popup */
	class ImGuiWindowPlacement
	{
	public:

		/** set the next window placement */
		void PrepareNextWindowPlacement() const;

		/** get an instance for a popup centered on screen */
		static ImGuiWindowPlacement GetCenterOnScreenPlacement(const glm::vec2& in_alignment = { 0.5f, 0.5f });
		/** get an instance for a popup placed on cursor position */
		static ImGuiWindowPlacement GetCenterOnCursorPlacement(const glm::vec2& in_alignment = { 0.0f, 0.0f });
		/** get an instance for a popup at a given position */
		static ImGuiWindowPlacement GetCenterOnPositionPlacement(const glm::vec2& in_position, const glm::vec2& in_alignment = { 0.5f, 0.5f });

	public:

		/** how to place the popup */
		ImGuiWindowPlacementType placement_type = ImGuiWindowPlacementType::ScreenCenter;
		/** the position of the popup */
		glm::vec2 position = { 0.0f, 0.0f };
		/** the alignment of the popup */
		glm::vec2 alignment = { 0.5f, 0.5f };
	};

#endif

}; // namespace chaos