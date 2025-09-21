namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ConsumingInputDeviceProxy;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ConsumingInputDeviceProxy: a specialization of InputDeviceInterface that keep track of requests and prevent the same request to be sent again
	*/

	class CHAOS_API ConsumingInputDeviceProxy : public InputDeviceInterface
	{

	protected:

		/** override */
		virtual KeyState const* DoGetKeyState(Key key) const override;
		/** override */
		virtual AxisState const* DoGetAxisState(GamepadAxis axis) const override;
		/** override */
		virtual StickState const* DoGetStickState(GamepadStick stick) const override;

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