namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class WindowPlacementInfo;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowPlacementInfo: describes how the window should be placed
	*/

	class CHAOS_API WindowPlacementInfo
	{
	public:

		/** the wanted monitor */
		GLFWmonitor* monitor = nullptr;
		/** the monitor index (used only if monitor is not explicitly defined above) */
		std::optional<int> monitor_index;

		/** the wanted position (if a monitor is explicitly given, the position is relative to this monitor, elsewhere it is absolute) */
		std::optional<glm::ivec2> position;
		/** the wanted size (fit the whole monitor if not defined or with negative values or fullscreen is set to true) */
		std::optional<glm::ivec2> size;
		/** a fullscreen window fits the whole monitor and displays no decorator */
		bool fullscreen = false;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, WindowPlacementInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, WindowPlacementInfo& dst);

#endif

}; // namespace chaos