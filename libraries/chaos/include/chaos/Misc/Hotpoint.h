namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Hotpoint;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(Hotpoint, CHAOS_API)

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API Hotpoint : int
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
	CHAOS_API glm::vec2 ConvertHotpointToBottomLeft(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint hotpoint);
	/** given a hotpoint and a hotpoint, returns any other hotpoint position */
	CHAOS_API glm::vec2 ConvertHotpoint(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint initial_hotpoint, Hotpoint final_hotpoint);

	CHAOS_DECLARE_ENUM_METHOD(Hotpoint, CHAOS_API);

#endif

}; // namespace chaos