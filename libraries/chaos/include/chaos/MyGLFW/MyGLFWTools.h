#ifdef CHAOS_FORWARD_DECLARATION


#else

namespace chaos
{
	namespace MyGLFW
	{
		/** Get a list of all monitors sorted left to right */
		std::vector<GLFWmonitor*> GetSortedMonitors();
		/** Get monitor by its index (negative for LEFT, positive for RIGHT, 0 for primary */
		GLFWmonitor* GetMonitorByIndex(int monitor_index, bool relative_to_primary = false);
		/** Get monitor nearest the given position (for example center of a window) */
		GLFWmonitor* GetNearestMonitor(glm::ivec2 const& position);

	}; // namespace MyGLFW

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

