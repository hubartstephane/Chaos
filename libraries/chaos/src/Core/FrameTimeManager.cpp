#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void FrameTimeManager::SetCurrentFrameTime(double in_frame_time)
	{
		assert(in_frame_time >= 0.0);

		if (current_frame_time != in_frame_time) // do not override previous_frame_time if there is no change
		{
			if (current_frame_time < 0.0) // for the very first call, ensure we have a FPS equal to 0
			{
				previous_frame_time = in_frame_time;
				current_frame_time = in_frame_time;
			}
			else
			{
				previous_frame_time = current_frame_time;
				current_frame_time = in_frame_time;
			}
		}
	}

	double FrameTimeManager::GetCurrentFrameTime() const
	{
		if (current_frame_time < 0.0)
			return 0.0;
		return current_frame_time;
	}

	double FrameTimeManager::GetCurrentFrameDuration() const
	{
		if (previous_frame_time < 0.0) // ensure previous_frame_time has been set once
			return 0.0;
		return (current_frame_time - previous_frame_time);
	}

}; // namespace chaos
