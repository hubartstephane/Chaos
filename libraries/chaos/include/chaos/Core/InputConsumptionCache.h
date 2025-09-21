namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputConsumptionCache: a singleton to keep trace of inputs that have been handled this frame
	 */

	class InputConsumptionCache : public Singleton<InputConsumptionCache>
	{
	public:

		/** clear the cache */
		void Clear();

		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkKeyConsumed(Key in_key, InputDeviceInterface const * in_input_device);
		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkAxisConsumed(GamepadAxis in_axis, InputDeviceInterface const * in_input_device);
		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkStickConsumed(GamepadStick in_stick, InputDeviceInterface const * in_input_device);

		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkKeyConsumed(Key in_key, KeyState const* in_state);
		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkAxisConsumed(GamepadAxis in_axis, AxisState const* in_state);
		/** returns whether the input has not been consumed yet, then mark it as consumed */
		bool CheckAndMarkStickConsumed(GamepadStick in_stick, StickState const* in_state);

	protected:

		/** keys that are consumed */
		std::set<std::pair<Key, KeyState const*>> consumed_keys;
		/** axes that are consumed */
		std::set<std::pair<GamepadAxis, AxisState const*>> consumed_axes;
		/** sticks that are consumed */
		std::set<std::pair<GamepadStick, StickState const*>> consumed_sticks;
	};

#endif

}; // namespace chaos