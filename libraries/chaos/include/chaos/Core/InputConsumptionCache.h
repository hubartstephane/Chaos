namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputConsumptionCache: an object that keeps that trace of which inputs have been handled this frame
	 */

	class InputConsumptionCache
	{
	public:

		/** clear the cache */
		void Clear();

		/** check whether the key is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(Key const& in_key, InputDeviceInterface const* in_input_device);
		/** check whether the key is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(GamepadAxis in_axis, InputDeviceInterface const* in_input_device);
		/** check whether the key is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(GamepadStick in_stick, InputDeviceInterface const* in_input_device);

	protected:

		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(Key in_key, InputDeviceInterface const* in_input_device);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(GamepadAxis in_axis, InputDeviceInterface const* in_input_device);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(GamepadStick in_stick, InputDeviceInterface const* in_input_device);

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