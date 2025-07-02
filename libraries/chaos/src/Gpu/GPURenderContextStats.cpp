#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_GLOBAL_VARIABLE(size_t, renderer_stats_size, 1200 * 20) // let's say at 1200 fps for very small project, we have 20 seconds stored

	GPURenderContextStats::GPURenderContextStats():
		frame_stats(renderer_stats_size.Get())
	{
	}

	void GPURenderContextStats::OnBeginRenderingFrame(uint64_t rendering_timestamp)
	{
		current_frame_stat = GPURenderContextFrameStats();
		current_frame_stat.rendering_timestamp = rendering_timestamp;
		current_frame_stat.frame_start_time = (float)glfwGetTime();
	}

	void GPURenderContextStats::OnEndRenderingFrame()
	{
		// update the frame rate
		framerate_counter.Accumulate(1.0f);
		// store statistics
		current_frame_stat.frame_end_time = (float)glfwGetTime();
		frame_stats.push_back(current_frame_stat);
		// prepare next frame stats
		current_frame_stat = GPURenderContextFrameStats();
	}

	float GPURenderContextStats::GetAverageFrameRate() const
	{
		return framerate_counter.GetCurrentValue();
	}

	int GPURenderContextStats::GetAverageDrawCalls() const
	{
		return drawcall_counter.GetCurrentValue();
	}

	int GPURenderContextStats::GetAverageVertices() const
	{
		return vertices_counter.GetCurrentValue();
	}


	void GPURenderContextStats::Tick(float delta_time)
	{
		last_delta_time = delta_time;

		framerate_counter.Tick(delta_time);
		drawcall_counter.Tick(delta_time);
		vertices_counter.Tick(delta_time);
	}

	void GPURenderContextStats::OnDrawCall(int vertice_count)
	{
		vertices_counter.Accumulate(vertice_count);
		current_frame_stat.vertices_counter += vertice_count;

		drawcall_counter.Accumulate(1);
		++current_frame_stat.drawcall_counter;
	}

}; // namespace chaos
