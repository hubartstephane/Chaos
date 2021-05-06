#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class Edge;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	/** the possible edges (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Edge : int
	{
		TOP = 0,
		RIGHT = 1,
		BOTTOM = 2,
		LEFT = 3
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
