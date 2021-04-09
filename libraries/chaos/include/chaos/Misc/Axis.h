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

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, Axis& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, Axis const& src);


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION