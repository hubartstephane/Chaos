namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Corner;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible corners (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class CHAOS_API Corner : int
	{
		TOP_RIGHT = 0,
		BOTTOM_RIGHT = 1,
		BOTTOM_LEFT = 2,
		TOP_LEFT = 3
	};

#endif

}; // namespace chaos