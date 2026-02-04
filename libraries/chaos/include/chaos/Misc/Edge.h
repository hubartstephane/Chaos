namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Edge;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible edges (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Edge : int
	{
		Top = 0,
		Right = 1,
		Bottom = 2,
		Left = 3
	};

	CHAOS_DECLARE_ENUM_METHOD(Edge, CHAOS_API);

#endif

}; // namespace chaos