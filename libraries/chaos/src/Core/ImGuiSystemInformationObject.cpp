#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	int ImGuiSystemInformationObject::GetImGuiWindowFlags() const
	{
		return ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	}

	void ImGuiSystemInformationObject::OnDrawImGuiContent(Window* window)
	{
		int monitor_count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
		if (monitors != nullptr && monitor_count > 0)
		{
			for (int i = 0; i < monitor_count; ++i)
			{
				glm::ivec2 monitor_position = { 0, 0 };
				glfwGetMonitorPos(monitors[i], &monitor_position.x, &monitor_position.y);

				ImGui::Text("monitor[%d] position: (%d, %d)", i, monitor_position.x, monitor_position.y);

				if (GLFWvidmode const* mode = glfwGetVideoMode(monitors[i]))
					ImGui::Text("monitor[%d] size:     (%d, %d)", i, mode->width, mode->height);
			}
		}
	}

}; // namespace chaos