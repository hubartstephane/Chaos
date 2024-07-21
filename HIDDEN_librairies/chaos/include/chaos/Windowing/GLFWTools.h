namespace chaos
{
	namespace GLFWTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** Get a list of all monitors sorted left to right */
		CHAOS_API std::vector<GLFWmonitor*> GetSortedMonitors();
		/** Get the index of a monitor in sorted list */
		CHAOS_API int GetMonitorIndex(GLFWmonitor* monitor);
		/** Get monitor by its index (negative for LEFT, positive for RIGHT, 0 for primary */
		CHAOS_API GLFWmonitor* GetMonitorByIndex(int monitor_index, bool relative_to_primary = false);
		/** Get monitor nearest the given position (for example center of a window) */
		CHAOS_API GLFWmonitor* GetNearestMonitor(glm::ivec2 const& position);
		/** convert an absolute position into a monitor system */
		CHAOS_API glm::ivec2 AbsolutePositionToMonitor(glm::ivec2 position, GLFWmonitor* monitor);
		/** convert a monitor system position into an absolute */
		CHAOS_API glm::ivec2 MonitorPositionToAbsolute(glm::ivec2 position, GLFWmonitor* monitor);

#endif

	}; // namespace GLFWTools

}; // namespace chaos

