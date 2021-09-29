namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Axis;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible axis */
	enum class Axis : int
	{
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2
	};

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json, Axis& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json, Axis const& src);

#endif

}; // namespace chaos