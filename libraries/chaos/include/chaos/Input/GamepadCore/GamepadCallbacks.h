namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadCallbacks;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadCallbacks : some callbacks that may be plugged into a gamepad
	*/

	class CHAOS_API GamepadCallbacks : public Object
	{
	public:

		/** destructor */
		virtual ~GamepadCallbacks() = default;

		/** called to determin if a physical gamepad is a good candidate for binding */
		virtual bool AcceptPhysicalDevice(class PhysicalGamepad*) { return true; }
		/** called whenever a gamepad is disconnected */
		virtual bool OnGamepadDisconnected(class Gamepad*) { return true; }
		/** called whenever a gamepad is "connected" (a new ID is given to it) */
		virtual bool OnGamepadConnected(class Gamepad*) { return true; }
		/** called to add some filters on inputs */
		virtual void OnGamepadStateUpdated(class GamepadState&) {}
	};

#endif

}; // namespace chaos