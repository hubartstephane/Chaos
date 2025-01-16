#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiRenderingStatsObject::ImGuiRenderingStatsObject()
	{
		imgui_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	}

	void ImGuiRenderingStatsObject::OnDrawImGuiContent(Window* window)
	{
		if (GPURenderer const * renderer = window->GetRenderer())
		{
			float average_framerate = renderer->GetAverageFrameRate();
			int   average_drawcall  = renderer->GetAverageDrawCalls();
			int   average_vertices  = renderer->GetAverageVertices();
			ImGui::Text("Frames     (per sec): %f", average_framerate);
			ImGui::Text("Draw calls (per sec): %d", average_drawcall);
			ImGui::Text("Vertices   (per sec): %d", average_vertices);
		}
	}

}; // namespace chaos