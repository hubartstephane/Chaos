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

				int count = (int)std::min(GRAPH_WIDTH, stats.size());

				float x[GRAPH_WIDTH];
				float y[GRAPH_WIDTH];
				float y_max = -1.0f; // values are always positive
				for (int i = 0; i < count; ++i)
				{
					GPURenderContextFrameStats const& frame_stat = stats[i];
					x[i] = (float)frame_stat.frame_time;
					y[i] = (float)(&frame_stat->*data_ptr);
					y_max = std::max(y_max, y[i]);
				}

				int plot_style =
					ImPlotFlags_NoMenus |
					ImPlotFlags_NoLegend |
					ImPlotFlags_NoMouseText |
					ImPlotFlags_NoBoxSelect |
					ImPlotAxisFlags_LockMin |
					ImPlotAxisFlags_LockMax |
					ImPlotAxisFlags_NoSideSwitch;

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
					ImPlot::PlotShaded(title, x, y, count);
					ImPlot::PopStyleVar();
					ImPlot::EndPlot();
				}
			};

			DrawStat("Draw calls", &GPURenderContextFrameStats::drawcall_counter);
			DrawStat("Vertices", &GPURenderContextFrameStats::vertices_counter);
		}
	}

}; // namespace chaos