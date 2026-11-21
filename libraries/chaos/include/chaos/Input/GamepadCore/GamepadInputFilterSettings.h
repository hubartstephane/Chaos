namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadInputFilterSettings;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadInputFilterSettings: some configuration describing how to handle gamepad input
	*/

	class CHAOS_API GamepadInputFilterSettings
	{
	public:

		/** how axis and sticks are interpolated */
		InterpolationType length_interpolation_type = InterpolationType::Linear;
		/** dead_zone applied to axes and sticks (using length for input2D). If value lower than this, consider it as 0 */
		float dead_zone = 0.2f;
		/** max_zone applied to axes and sticks (using length for input2D). If value greater than this, consider it as 1 */
		float max_zone  = 0.9f;
		/** factor of snapping. A factor of 0 mean no snapping. 1 full snapping */
		float sector_snap_angle_ratio = 0.3f;
		/** number of sector for angle snapping */
		int sector_snap_count = 4;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, GamepadInputFilterSettings const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, GamepadInputFilterSettings& dst);

#endif

}; // namespace chaos