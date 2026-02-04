namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Hotpoint;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(Hotpoint, CHAOS_API)

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class Hotpoint : int
	{
		Left = 1,
		Right = 2,
		HMiddle = 0,

		Bottom = 4,
		Top = 8,
		VMiddle = 0,

		BottomLeft = Bottom | Left,
		TopLeft = Top | Left,
		BottomRight = Bottom | Right,
		TopRight = Top | Right,
		Center = VMiddle | HMiddle
	};

	/** given a hotpoint and a hotpoint, returns the BottomLeft hotpoint position */
	CHAOS_API glm::vec2 ConvertHotpointToBottomLeft(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint hotpoint);
	/** given a hotpoint and a hotpoint, returns any other hotpoint position */
	CHAOS_API glm::vec2 ConvertHotpoint(glm::vec2 const& pt, glm::vec2 const& size, Hotpoint initial_hotpoint, Hotpoint final_hotpoint);

	CHAOS_DECLARE_ENUM_METHOD(Hotpoint, CHAOS_API);

#endif

}; // namespace chaos