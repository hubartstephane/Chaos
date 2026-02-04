namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Corner;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible corners (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Corner : int
	{
		TopRight = 0,
		BottomRight = 1,
		BottomLeft = 2,
		TopLeft = 3
	};

#endif

}; // namespace chaos