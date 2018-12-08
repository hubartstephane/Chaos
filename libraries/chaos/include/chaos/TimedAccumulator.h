#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Allocators.h>
#include <chaos/AllocatorTools.h>

namespace chaos
{
	template<typename T>
	class TimedAccumulator
	{
	public:

		/** constructor */
		TimedAccumulator(float in_timer_duration = 1.0f) :
			current_value(T()),
			accumulated_values(T()),
			timer_duration(in_timer_duration)
		{
		}
		/** insert some value into the accumulator */
		void Accumulate(T value)
		{
			accumulated_values += value;
		}
		/** update the time */
		void Tick(float delta_time)
		{
			timer_value += delta_time;
			if (timer_value >= timer_duration)
			{
				current_value = accumulated_values / timer_value;
				timer_value = 0.0f;				
				accumulated_values = T();
			}
		}
		/** get the previously accumulated value */
		T GetCurrentValue() const
		{
			return current_value;
		}

	protected:

		/** current value */
		T current_value;
		/** accumulated values */
		T accumulated_values;	
		/** duration to reset the timer */
		float timer_duration = 1.0f;
		/** the current time */
		float timer_value = 0.0f;
	};


}; // namespace chaos

