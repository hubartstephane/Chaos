#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiWindowInformationDrawable::SetWindow(Window* in_window)
	{
		window = in_window;
	}

	void ImGuiWindowInformationDrawable::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		ImGui::Begin("Window Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

		if (window != nullptr)
		{
			// collect window information
			glm::ivec2 window_size = window->GetWindowSize(false); // only client size
			aabox2 viewport = window->GetRequiredViewport(window_size);
			double x = 0.0;
			double y = 0.0;
			glfwGetCursorPos(window->GetGLFWHandler(), &x, &y);

			// trace debugging information
			ImGui::Text("cursor              : (%0.3f, %0.3f)", (float)x, (float)y);
			ImGui::Text("window   size       : (%0.3f, %0.3f)", (float)window_size.x, (float)window_size.y);
			ImGui::Text("viewport position   : (%0.3f, %0.3f)", (float)viewport.position.x, (float)viewport.position.y);
			ImGui::Text("viewport size       : (%0.3f, %0.3f)", (float)viewport.size.x, (float)viewport.size.y);

			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("WantCaptureMouse    : %d", io.WantCaptureMouse);
			ImGui::Text("WantCaptureKeyboard : %d", io.WantCaptureKeyboard);
		}
		else
		{
			ImGui::Text("missing call to SetWindow(..)");
		}
		ImGui::End();
	}

}; // namespace chaos