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

			glm::ivec2 window_position = window->GetWindowPosition(true);   // include decorators
			glm::ivec2 client_position = window->GetWindowPosition(false);  // client area only

			glm::ivec2 window_size = window->GetWindowSize(true);  // include decorators
			glm::ivec2 client_size = window->GetWindowSize(false); // client area only

			aabox2 viewport = window->GetRequiredViewport(client_size);
			double x = 0.0;
			double y = 0.0;
			glfwGetCursorPos(window->GetGLFWHandler(), &x, &y);

			// trace debugging information
			ImGui::Text("cursor              : (%0.3f, %0.3f)", (float)x, (float)y);
			ImGui::Text("window   position   : (%0.3f, %0.3f)", (float)window_position.x, (float)window_position.y);
			ImGui::Text("window   size       : (%0.3f, %0.3f)", (float)window_size.x, (float)window_size.y);
			ImGui::Text("client   position   : (%0.3f, %0.3f)", (float)client_position.x, (float)client_position.y);
			ImGui::Text("client   size       : (%0.3f, %0.3f)", (float)client_size.x, (float)client_size.y);
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