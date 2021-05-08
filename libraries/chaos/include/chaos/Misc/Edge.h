namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Edge;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible edges (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Edge : int
	{
		TOP = 0,
		RIGHT = 1,
		BOTTOM = 2,
		LEFT = 3
	};

#endif

}; // namespace chaos