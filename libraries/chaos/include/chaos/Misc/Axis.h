#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class Axis;

}; // namespace chaos

#else 
	
namespace chaos
{
	/** the possible axis */
	enum class Axis : int
	{
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION