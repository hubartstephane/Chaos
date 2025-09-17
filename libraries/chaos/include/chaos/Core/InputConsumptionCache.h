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

		/** mark the key as consumed this frame */
		void MarkKeyConsumed(Key in_key, InputDeviceInterface* in_input_device);
		/** mark the axis as consumed this frame */
		void MarkAxisConsumed(GamepadAxis in_axis, InputDeviceInterface* in_input_device);
		/** mark the stick as consumed this frame */
		void MarkStickConsumed(GamepadStick in_stick, InputDeviceInterface* in_input_device);

		/** check whether key is marked as consumed this frame */
		bool IsKeyMarkedConsumed(Key in_key, InputDeviceInterface const* in_input_device) const;
		/** check whether axis is marked as consumed this frame */
		bool IsAxisMarkedConsumed(GamepadAxis in_axis, InputDeviceInterface const* in_input_device) const;
		/** check whether stick is marked as consumed this frame */
		bool IsStickMarkedConsumed(GamepadStick in_stick, InputDeviceInterface const* in_input_device) const;

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