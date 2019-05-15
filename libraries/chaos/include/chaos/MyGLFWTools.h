#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace MyGLFW
	{
		/**
		* Tools : some usefull tools for GLFW
		*/

		class Tools
		{
		public:

			/** Get a list of all monitors sorted left to right */
			static std::vector<GLFWmonitor *> GetSortedMonitors();
			/** Get monitor by its index (negative for LEFT, positive for RIGHT, 0 for primary */
			static GLFWmonitor * GetMonitorByIndex(int monitor_index);
			/** Get monitor nearest the given position (for example center of a window) */
			static GLFWmonitor * GetNearestMonitor(glm::ivec2 const & position);
		};

	}; // namespace MyGLFW

}; // namespace chaos

