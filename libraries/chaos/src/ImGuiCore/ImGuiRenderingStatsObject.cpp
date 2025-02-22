#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static float GetDrawCallForCircularBuffer(void* buffer, int index)
	{
		auto stats = (boost::circular_buffer<GPURenderContextFrameStats> const *)(buffer);
		return (float)stats->operator[](size_t(index)).drawcall_counter;
	}

	static float GetVerticesForCircularBuffer(void* buffer, int index)
	{
		auto stats = (boost::circular_buffer<GPURenderContextFrameStats> const*)(buffer);
		return (float)stats->operator[](size_t(index)).vertices_counter;
	}

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

			boost::circular_buffer<GPURenderContextFrameStats> const &stats = render_context->GetStats();

			if (size_t stat_count = stats.size())
			{
				int point_count = std::min(int(stat_count), 500);


#if 0
				// Commence un tracé ImPlot dans une fenêtre
				if (ImPlot::BeginPlot("Graphique en temps réel", ImVec2(-1, 300))) {
					// Configure les axes avec des labels et un ajustement automatique
					ImPlot::SetupAxes("Temps (s)", "Valeur", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
					// Vous pouvez également configurer des graduations spécifiques avec ImPlot::SetupAxisTicks si besoin

					// ImPlot::PlotLine attend un tableau contigu de float.
					// Comme dataBuffer peut être non contigu après wrapping, on copie les données dans un vector.
					//std::vector<float> data(dataBuffer.begin(), dataBuffer.end());
					//ImPlot::PlotLine("Signal", data.data(), static_cast<int>(data.size()));

					ImGui::PlotLines("draw_calls", &GetDrawCallForCircularBuffer, (void*)&stats, point_count, value_offset, nullptr, scale_min, scale_max, graph_size);

					ImPlot::EndPlot();
				}

	
				ImVec2 graph_size   = {500.0f, 500.0f};
				float  scale_min    = 5.0f;
				float  scale_max    = 5.0f;
				int    value_offset = 0;

				ImGui::PlotLines("draw_calls", &GetDrawCallForCircularBuffer, (void *)&stats, point_count, value_offset, nullptr, scale_min, scale_max, graph_size);

				ImGui::PlotLines("vertices", &GetVerticesForCircularBuffer, (void*)&stats, point_count, value_offset, nullptr, scale_min, scale_max, graph_size);
				#endif
			}
		}
	}

}; // namespace chaos