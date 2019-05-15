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

		GLFWmonitor * Tools::GetNearestMonitor(glm::ivec2 const & position)
		{		
			// return all monitors
			std::vector<GLFWmonitor *> monitors = GetSortedMonitors();
			if (monitors.size() == 0)
				return nullptr;

			GLFWmonitor * best_monitor   = nullptr;
			int           best_distance2 = std::numeric_limits<int>::max();
			// 1 - search the monitor for which position is fully inside
			// 2 - search the monitor for which position.x is inside the range
			// 3 - search the monitor for which position.y is inside the range
			// 4 - search the nearest position center
			for (int step = 0 ; step < 4 ; ++step)
			{
				for (GLFWmonitor * monitor : monitors)
				{
					GLFWvidmode const * mode = glfwGetVideoMode(monitor);
					if (mode == nullptr)
						continue;

					glm::ivec2 monitor_position;
					glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);

					bool x_inside = (position.x >= monitor_position.x && position.x < monitor_position.x + mode->width);
					bool y_inside = (position.y >= monitor_position.y && position.y < monitor_position.y + mode->height);

					if (step == 0)
					{
						if (x_inside && y_inside)
							return monitor;
					}
					else if (step == 1)
					{
						if (x_inside)
							return monitor;
					}
					else if (step == 2)
					{
						if (y_inside)
							return monitor;
					}
					else if (step == 3)
					{
						glm::ivec2 monitor_center = monitor_position + glm::ivec2(mode->width, mode->height) / 2;
						int distance2 = glm::distance2(position , monitor_center);
						if (best_monitor == nullptr || distance2 < best_distance2)
						{
							best_distance2 = distance2;
							best_monitor   = monitor;
						}
					}
				}		
			}
			return best_monitor;
		}

	}; // namespace MyGLFW

}; // namespace chaos
