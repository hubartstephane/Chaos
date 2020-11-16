#include <chaos/Chaos.h>

namespace chaos
{
	namespace MyGLFW
	{
		std::vector<GLFWmonitor *> GetSortedMonitors()
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

		GLFWmonitor * GetMonitorByIndex(int monitor_index, bool relative_to_primary) // monitor_index relative to primary monitor
		{
			GLFWmonitor * result = glfwGetPrimaryMonitor();
			if (monitor_index != 0)
			{
				std::vector<GLFWmonitor *> monitors = GetSortedMonitors();

				// get the base monitor index (relative to the primary or to the most to the left)
				int base_monitor_index = 0;
				if (relative_to_primary)
				{
					auto b = monitors.begin();
					auto e = monitors.end();
					auto it = std::find(b, e, result);

					base_monitor_index = (int)(it - b); // we want to offset it with positive or negative values (cast it into signed)
				}
				// compute the index of the monitor we are interested in (and clamp)
				int result_index = base_monitor_index + monitor_index;
				if (result_index < 0)
					result_index = 0;
				else if (result_index >= (int)monitors.size())
					result_index = (int)monitors.size() - 1;

				result = monitors[(size_t)result_index];
			}
			return result;
		}

		GLFWmonitor * GetNearestMonitor(glm::ivec2 const & position)
		{		
			// get all monitors
			int monitor_count = 0;
			GLFWmonitor ** monitors = glfwGetMonitors(&monitor_count);
			if (monitors == nullptr || monitor_count == 0)
				return nullptr;

			GLFWmonitor * best_monitor   = nullptr;
			int           best_distance2 = std::numeric_limits<int>::max();
			// 1 - search the monitor for which position is fully inside
			// 2 - search the monitor for which position.x is inside the range
			// 3 - search the monitor for which position.y is inside the range
			// 4 - search the nearest position center
			for (int step = 0 ; step < 4 ; ++step)
			{
				for (int i = 0 ; i < monitor_count ; ++i)
				{
					GLFWmonitor * monitor = monitors[i];

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
						glm::ivec2 delta_pos = position - monitor_center;

						int distance2 = delta_pos.x * delta_pos.x + delta_pos.y * delta_pos.y; // glm::dot(...) and glm::distance2(...) only work with floating values
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
