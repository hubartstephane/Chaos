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
				constexpr float  HISTORY_DURATION   = 20.0f; // 20 seconds history on screen
				constexpr float  SAMPLES_PER_SECOND = 100.0f;
				constexpr size_t BUFFER_SIZE        = size_t(HISTORY_DURATION * SAMPLES_PER_SECOND);

				float current_time = (float)glfwGetTime();

				float values[BUFFER_SIZE];
				memset(values, 0, sizeof(values));

				float times [BUFFER_SIZE];
				memset(times, 0, sizeof(times));

				float max_value = 0.0f; // values are always positive

				int last_index_in_buffer = BUFFER_SIZE - 1;
				for (int i = (int)stats.size() - 1 ; i >= 0 ; --i)
				{
					GPURenderContextFrameStats const & st = stats[i];

					float dt = current_time - st.frame_time;
					if (dt > HISTORY_DURATION)
						break;

					int index_in_buffer = (BUFFER_SIZE - 1) - int(dt * SAMPLES_PER_SECOND);
					if (index_in_buffer < 0)
						break;

					values[index_in_buffer] = std::max(values[index_in_buffer], float(st.*data_ptr));
					times[index_in_buffer]  = std::max(times[index_in_buffer], st.frame_time);

					max_value = std::max(max_value, values[index_in_buffer]);

					last_index_in_buffer = index_in_buffer;
				}

				size_t write_index = 0;
				for (size_t read_index = last_index_in_buffer; read_index < BUFFER_SIZE ; ++read_index)
				{
					if (times[read_index] == 0)
						continue;

					values[write_index] = values[read_index];
					times[write_index] = times[read_index];
					++write_index;
				}

				int plot_style =
					ImPlotFlags_NoMenus |
					ImPlotFlags_NoLegend |
					ImPlotFlags_NoInputs |
					ImPlotFlags_NoMouseText |
					ImPlotFlags_NoBoxSelect;

				if (ImPlot::BeginPlot(title, ImVec2(-1, 0), plot_style))
				{
					ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

					int axis_x_style =
						ImPlotAxisFlags_AutoFit |
						ImPlotAxisFlags_NoMenus;

					int axis_y_style =
						ImPlotAxisFlags_NoLabel |
						ImPlotAxisFlags_NoMenus;

					ImPlot::SetupAxes("Time", title, axis_x_style, axis_y_style);
					ImPlot::SetupAxisLimits(ImAxis_X1, current_time - HISTORY_DURATION, current_time, ImPlotCond_Always);
					ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, max_value * 1.2f, ImPlotCond_Always);
										
					ImPlot::PlotShaded(title, times, values, int(write_index));
					ImPlot::PlotLine(title, times, values, int(write_index));

					ImPlot::PopStyleVar();
					ImPlot::EndPlot();
				}
			};

			DrawStat("Draw calls", &GPURenderContextFrameStats::drawcall_counter);
			DrawStat("Vertices", &GPURenderContextFrameStats::vertices_counter);
		}
	}

}; // namespace chaos