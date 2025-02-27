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
			uint64_t time_stamp        = render_context->GetTimestamp();
			float    average_framerate = render_context->GetAverageFrameRate();
			int      average_drawcall  = render_context->GetAverageDrawCalls();
			int      average_vertices  = render_context->GetAverageVertices();
			ImGui::Text("Time stamp      : %llu", time_stamp);
			ImGui::Text("Frames     / sec: %f",   average_framerate);
			ImGui::Text("Draw calls / sec: %d",   average_drawcall);
			ImGui::Text("Vertices   / sec: %d",   average_vertices);

			boost::circular_buffer<GPURenderContextFrameStats> const stats = render_context->GetStats();

			auto DrawStat = [stats](char const * title, int GPURenderContextFrameStats::*data_ptr)
			{
				constexpr size_t GRAPH_WIDTH = 500;
				constexpr float  SLICE_DURATION = 1.0f;

				float time[GRAPH_WIDTH];
				float min_value[GRAPH_WIDTH];
				float max_value[GRAPH_WIDTH];
				float y_max = 0.0f; // values are always positive

				int stat_index  = (int)stats.size() - 1;
				int graph_index = GRAPH_WIDTH - 1;
				for (; graph_index >= 0 && stat_index >= 0; --graph_index)
				{
					float base_time = (float)stats[stat_index].frame_time;
					base_time = float(int(base_time * 100.0f)) / 100.0f;

					time[graph_index] = base_time;

					int min_v = stats[stat_index].*data_ptr;
					int max_v = stats[stat_index].*data_ptr;
					do
					{
						float stat_time = (float)stats[stat_index].frame_time;
						if (stat_time < base_time)
							break;

						int value = stats[stat_index].*data_ptr;
						min_v = std::min(min_v, value);
						max_v = std::max(max_v, value);

					} while (--stat_index >= 0);

					min_value[graph_index] = std::min(min_value[graph_index], (float)min_v);
					max_value[graph_index] = std::max(max_value[graph_index], (float)max_v);
					y_max = std::max(y_max, max_value[graph_index]);
				}

				int plot_style =
					ImPlotFlags_NoMenus |
					ImPlotFlags_NoLegend |
					ImPlotFlags_NoInputs |
					ImPlotFlags_NoMouseText |
					ImPlotFlags_NoBoxSelect;

				if (ImPlot::BeginPlot(title, ImVec2(-1, 0), plot_style))
				{
					int axis_x_style =
						ImPlotAxisFlags_AutoFit |
						ImPlotAxisFlags_NoMenus;

					int axis_y_style =
						ImPlotAxisFlags_NoLabel |
						ImPlotAxisFlags_NoMenus;

					ImPlot::SetupAxes("Time", title, axis_x_style, axis_y_style);
					ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, y_max * 1.2f);

					ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

					int count = (GRAPH_WIDTH - 1) - graph_index;
					float const * mn = &min_value[graph_index + 1];
					float const * mx = &max_value[graph_index + 1];
					float const * t  = &time[graph_index + 1];

					ImPlot::PlotShaded(title, t, mx, count);
					ImPlot::PopStyleVar();
					ImPlot::EndPlot();
				}
			};

			DrawStat("Draw calls", &GPURenderContextFrameStats::drawcall_counter);
			DrawStat("Vertices", &GPURenderContextFrameStats::vertices_counter);
		}
	}

}; // namespace chaos