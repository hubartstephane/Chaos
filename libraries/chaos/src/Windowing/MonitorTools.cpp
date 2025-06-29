#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace MonitorTools
	{
		std::vector<GLFWmonitor*> GetSortedMonitors()
		{
			std::vector<GLFWmonitor*> result;

			int monitor_count = 0;
			GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);

			if (monitor_count > 0 && monitors != nullptr)
			{
				result.reserve((size_t)monitor_count);

				// fill that array with pairs : (monitors, X position)
				for (int i = 0; i < monitor_count; ++i)
					result.push_back(monitors[i]);

				// sort the array by X position of the monitor
				auto b = result.begin();
				auto e = result.end();

				std::sort(b, e, [](GLFWmonitor* src1, GLFWmonitor* src2) {
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

		int GetMonitorIndex(GLFWmonitor* monitor)
		{
			std::vector<GLFWmonitor*> monitors = GetSortedMonitors();
			for (int i = 0; i < monitors.size(); ++i)
				if (monitors[i] == monitor)
					return i;
			return 0;
		}

		GLFWmonitor* GetMonitorByIndex(int monitor_index, bool relative_to_primary) // monitor_index relative to primary monitor
		{
			std::vector<GLFWmonitor*> monitors = GetSortedMonitors();
			if (monitors.size() == 0)
				return nullptr;

			GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();

			// get the base monitor index (relative to the primary or to the most to the left)
			int base_monitor_index = 0;
			if (relative_to_primary)
			{
				auto b = monitors.begin();
				auto e = monitors.end();
				auto it = std::find(b, e, primary_monitor);

				base_monitor_index = int(it - b); // we want to offset it with positive or negative values (cast it into signed)
			}
			// compute the index of the monitor we are interested in (and clamp)
			int effective_monitor_index = std::clamp(
				base_monitor_index + monitor_index,
				0,
				int(monitors.size()) - 1);

			return monitors[(size_t)effective_monitor_index];
		}

		GLFWmonitor* GetNearestMonitor(glm::ivec2 const& position)
		{
			// get all monitors
			int monitor_count = 0;
			GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
			if (monitors == nullptr || monitor_count == 0)
				return nullptr;

			GLFWmonitor* best_monitor = nullptr;
			int          best_distance2 = std::numeric_limits<int>::max();
			// 1 - search the monitor for which position is fully inside
			// 2 - search the nearest position center
			for (int step = 0; step < 2; ++step)
			{
				for (int i = 0; i < monitor_count; ++i)
				{
					GLFWmonitor* monitor = monitors[i];

					GLFWvidmode const* mode = glfwGetVideoMode(monitor);
					if (mode == nullptr)
						continue;

					glm::ivec2 monitor_position;
					glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);

					if (step == 0)
					{
						if ((position.x >= monitor_position.x && position.x < monitor_position.x + mode->width) &&
						    (position.y >= monitor_position.y && position.y < monitor_position.y + mode->height))
							return monitor;
					}
					else
					{
						glm::ivec2 monitor_center = monitor_position + glm::ivec2(mode->width, mode->height) / 2;
						glm::ivec2 delta_pos = position - monitor_center;

						int distance2 = delta_pos.x * delta_pos.x + delta_pos.y * delta_pos.y; // glm::dot(...) and glm::distance2(...) only work with floating values
						if (best_monitor == nullptr || distance2 < best_distance2)
						{
							best_distance2 = distance2;
							best_monitor = monitor;
						}
					}
				}
			}
			return best_monitor;
		}

		glm::ivec2 AbsolutePositionToMonitor(glm::ivec2 position, GLFWmonitor* monitor)
		{
			if (monitor != nullptr)
			{
				glm::ivec2 monitor_position = { 0, 0 };
				glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);
				position -= monitor_position;
			}
			return position;
		}

		glm::ivec2 MonitorPositionToAbsolute(glm::ivec2 position, GLFWmonitor* monitor)
		{
			if (monitor != nullptr)
			{
				glm::ivec2 monitor_position = { 0, 0 };
				glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);
				position += monitor_position;
			}
			return position;
		}

	glm::ivec2 MonitorTools::GetTweakedWindowPositionForMonitors(glm::ivec2 const & window_position, glm::ivec2 const & window_size)
	{
		//        monitor
		// 
		//  +---+---------+---+
		//  |   |         |   |  ensure that the window toolbar touchs the center portion of the monitor
		//  |   |         |   |  (the toolbar should never be cropped along the Y axis)
		//  |   +---------+   |
		//  |                 |
		//  +-----------------+

		const int MIN_HOVER_X = 300;
		const int MIN_HOVER_Y = 150;

		int monitor_count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
		if (monitor_count == 0)
			return window_position;

		auto length2 = [](glm::ivec2 const& src)
		{
			return (src.x * src.x + src.y * src.y);
		};

		std::optional<glm::ivec2> smaller_displacement;
		for (int i = 0; i < monitor_count; ++i)
		{
			if (GLFWmonitor* monitor = monitors[i])
			{
				if (GLFWvidmode const* other_mode = glfwGetVideoMode(monitor))
				{
					// get size and position of the monitor
					glm::ivec2 monitor_position = { 0 , 0 };
					glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);

					glm::ivec2 monitor_size = { other_mode->width , other_mode->height };

					// shrink it with the BORDER
					monitor_position.x += MIN_HOVER_X;  // do not shrink the monitor area at the top. we don't want fullscreen window to be pushed down
					monitor_size.x -= 2 * MIN_HOVER_X;
					monitor_size.y -= MIN_HOVER_Y;

					// compute the displacement required so that the window is touching this monitor
					glm::ivec2 required_displacement = { 0, 0 };

					if (window_position.x > monitor_position.x + monitor_size.x)
						required_displacement.x = (monitor_position.x + monitor_size.x) - window_position.x;
					else if (window_position.x + window_size.x < monitor_position.x)
						required_displacement.x = (monitor_position.x - window_size.x) - window_position.x;

					if (window_position.y > monitor_position.y + monitor_size.y)
						required_displacement.y = (monitor_position.y + monitor_size.y) - window_position.y;
					else if (window_position.y < monitor_position.y)
						required_displacement.y = monitor_position.y - window_position.y;

					// early exit if no displacement is required
					if (required_displacement.x == 0 && required_displacement.y == 0)
						return window_position;

					// compare displacement with the best case
					if ((!smaller_displacement.has_value()) ||
						(length2(smaller_displacement.value()) > length2(required_displacement)))
					{
						smaller_displacement = required_displacement;
					}
				}
			}
		}

		if (smaller_displacement.has_value())
		{
			return window_position + smaller_displacement.value();
		}
		return window_position;
	}


	}; // namespace MonitorTools

}; // namespace chaos
