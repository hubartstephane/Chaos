#include <chaos/MyGLFWTools.h>

namespace chaos
{
	namespace MyGLFW
	{
		/**
		* Tools
		*/

		std::vector<GLFWmonitor *> Tools::GetSortedMonitors()
		{
			std::vector<GLFWmonitor *> result;

			int monitor_count = 0;
			GLFWmonitor ** monitors = glfwGetMonitors(&monitor_count);

			if (monitor_count > 0 && monitors != nullptr)
			{
				result.reserve((size_t)monitor_count);

				// fill that array with pairs : (monitors, X position)
				for (int i = 0; i < monitor_count; ++i)
					result.push_back(monitors[i]);

				// sort the array by X position of the monitor    
				auto b = result.begin();
				auto e = result.end();

				std::sort(b, e, [](GLFWmonitor * src1, GLFWmonitor * src2) {
					int x1 = 0;
					int y1 = 0;
					int x2 = 0;
					int y2 = 0;
					glfwGetMonitorPos(src1, &x1, &y1);
					glfwGetMonitorPos(src2, &x2, &y2);
					return (x1 < x2) ? true : false;
				});
			}
			return result;
		}

		GLFWmonitor * Tools::GetMonitorByIndex(int monitor_index) // monitor_index relative to primary monitor
		{
			GLFWmonitor * result = glfwGetPrimaryMonitor();
			if (monitor_index != 0)
			{
				std::vector<GLFWmonitor *> monitors = GetSortedMonitors();

				// search the primary monitor inside
				auto b = monitors.begin();
				auto e = monitors.end();
				auto it = std::find(b, e, result);

				// primary index
				int primary_index = (int)(it - b); // we want to offset it with positive or negative values (cast it into signed)

				 // compute the index of the monitor we are interested in (and clamp)
				int result_index = primary_index + monitor_index;
				if (result_index < 0)
					result_index = 0;
				else if (result_index >= (int)monitors.size())
					result_index = (int)monitors.size() - 1;

				result = monitors[(size_t)result_index];
			}
			return result;
		}

	}; // namespace MyGLFW

}; // namespace chaos
