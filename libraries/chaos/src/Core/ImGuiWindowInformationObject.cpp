#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiWindowInformationObject::SetWindow(Window* in_window)
	{
		window = in_window;
	}

	void ImGuiWindowInformationObject::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		bool open_value = true;

		if (ImGui::Begin("Window Information", &open_value, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
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

				WindowFrameSizeInfo framesize_info;
				glfwGetWindowFrameSize(window->GetGLFWHandler(), &framesize_info.left, &framesize_info.top, &framesize_info.right, &framesize_info.bottom);

				// trace debugging information
				ImGui::Text("cursor              : (%d, %d)", (int)x, (int)y);
				ImGui::Separator();
				ImGui::Text("window   position   : (%d, %d)", window_position.x, window_position.y);
				ImGui::Text("window   size       : (%d, %d)", window_size.x, window_size.y);
				ImGui::Separator();
				ImGui::Text("client   position   : (%d, %d)", client_position.x, client_position.y);
				ImGui::Text("client   size       : (%d, %d)", client_size.x, client_size.y);
				ImGui::Separator();
				ImGui::Text("viewport position   : (%d, %d)", viewport.position.x, viewport.position.y);
				ImGui::Text("viewport size       : (%d, %d)", viewport.size.x, viewport.size.y);
				ImGui::Separator();
				ImGui::Text("framesize left      : (%d)", framesize_info.left);
				ImGui::Text("framesize right     : (%d)", framesize_info.right);
				ImGui::Text("framesize top       : (%d)", framesize_info.top);
				ImGui::Text("framesize bottom    : (%d)", framesize_info.bottom);
				ImGui::Separator();
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

		if (!open_value)
			RequestClosing();
	}

}; // namespace chaos