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
		if (GPURenderContext const * render_context = window->GetRenderer())
		{
			float average_framerate = render_context->GetAverageFrameRate();
			int   average_drawcall  = render_context->GetAverageDrawCalls();
			int   average_vertices  = render_context->GetAverageVertices();
			ImGui::Text("Frames     (per sec): %f", average_framerate);
			ImGui::Text("Draw calls (per sec): %d", average_drawcall);
			ImGui::Text("Vertices   (per sec): %d", average_vertices);
		}
	}

}; // namespace chaos