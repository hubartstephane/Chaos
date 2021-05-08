namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class TimedAccumulator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T>
	class TimedAccumulator
	{
	public:

		/** constructor */
		TimedAccumulator(float in_timer_duration = 1.0f) :
			timer_duration(in_timer_duration)
		{
		}
		/** constructor */
		TimedAccumulator(TimedAccumulator const& src) = default;

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
				current_value = T(float(accumulated_values) / timer_value);
				timer_value = 0.0f;
				accumulated_values = {};
			}
		}
		/** get the previously accumulated value */
		T GetCurrentValue() const
		{
			return current_value;
		}

	protected:

		/** current value */
		T current_value = {};
		/** accumulated values */
		T accumulated_values = {};
		/** duration to reset the timer */
		float timer_duration = 1.0f;
		/** the current time */
		float timer_value = 0.0f;
	};

#endif

}; // namespace chaos