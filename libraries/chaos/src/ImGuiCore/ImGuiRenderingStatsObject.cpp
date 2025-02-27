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

			boost::circular_buffer<GPURenderContextFrameStats> const & stats = render_context->GetStats();

			float current_time = (float)glfwGetTime();

			auto DrawStat = [&stats, current_time](char const * title, int GPURenderContextFrameStats::*data_ptr)
			{
				constexpr float  HISTORY_DURATION   = 20.0f; // 20 seconds history on screen
				constexpr float  SAMPLES_PER_SECOND = 1.0f;
				constexpr float  SAMPLE_DURATION    = 1.0f / SAMPLES_PER_SECOND;
				constexpr size_t BUFFER_SIZE        = size_t(HISTORY_DURATION * SAMPLES_PER_SECOND) + 1;

				float reference_time = float(int((current_time - HISTORY_DURATION) * SAMPLES_PER_SECOND)) / SAMPLES_PER_SECOND;

				constexpr float UNSET_VALUE = -std::numeric_limits<float>::max(); // this 'special' value works well with the MAX operation

				float values[BUFFER_SIZE];
				for (size_t i = 0 ; i < BUFFER_SIZE ; ++i)
					values[i] = UNSET_VALUE;

				float times [BUFFER_SIZE];
				for (size_t i = 0; i < BUFFER_SIZE; ++i)
					times[i] = reference_time + float(i) * SAMPLE_DURATION;

				float max_value = 0.0f; // values are always positive

				int min_index_in_buffer = BUFFER_SIZE - 1;
				for (int i = (int)stats.size() - 1 ; i >= 0 ; --i)
				{
					GPURenderContextFrameStats const & st = stats[i];

					if (st.frame_time < reference_time)
						break;

					int index_in_buffer = int((st.frame_time - reference_time) / SAMPLE_DURATION);
					if (index_in_buffer < 0)
						break;
					if (index_in_buffer > BUFFER_SIZE - 1)
						index_in_buffer = BUFFER_SIZE - 1;

					values[index_in_buffer] = std::max(values[index_in_buffer], float(st.*data_ptr));
					max_value               = std::max(max_value, values[index_in_buffer]);
					min_index_in_buffer     = std::min(min_index_in_buffer, index_in_buffer);
				}

				size_t write_index = 0;
				for (size_t read_index = min_index_in_buffer ; read_index < BUFFER_SIZE ; ++read_index)
				{
					if (values[read_index] == UNSET_VALUE)
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
					ImPlot::SetupAxisLimits(ImAxis_X1, current_time - HISTORY_DURATION, current_time - SAMPLE_DURATION, ImPlotCond_Always);
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