#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class Corner;

}; // namespace chaos

#else 

namespace chaos
{

	/** the possible corners (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Corner : int
	{
		TOP_RIGHT = 0,
		BOTTOM_RIGHT = 1,
		BOTTOM_LEFT = 2,
		TOP_LEFT = 3		
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
