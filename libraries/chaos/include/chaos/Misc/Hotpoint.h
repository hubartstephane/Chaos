namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Hotpoint;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class Hotpoint : int
	{
		LEFT = 1,
		RIGHT = 2,
		HMIDDLE = 0,

		BOTTOM = 4,
		TOP = 8,
		VMIDDLE = 0,

		BOTTOM_LEFT = BOTTOM | LEFT,
		TOP_LEFT = TOP | LEFT,
		BOTTOM_RIGHT = BOTTOM | RIGHT,
		TOP_RIGHT = TOP | RIGHT,
		CENTER = VMIDDLE | HMIDDLE
	};

	/** given a hotpoint and a hotpoint, returns the BottomLeft hotpoint position */
	glm::vec2 ConvertHotpointToBottomLeft(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint hotpoint);
	/** given a hotpoint and a hotpoint, returns any other hotpoint position */
	glm::vec2 ConvertHotpoint(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint initial_hotpoint, Hotpoint final_hotpoint);

	CHAOS_DECLARE_ENUM_METHOD(Hotpoint);

#endif

}; // namespace chaos