#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiSystemInformationDrawable::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		ImGui::Begin("System Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

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
		ImGui::End();
	}

}; // namespace chaos