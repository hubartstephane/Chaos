namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadPollInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadPollInterface: an interface for objects that want to poll gamepad states
	*/

	class GamepadPollInterface
	{
	public:

		/** called whenever a physical state is to be polled */
		virtual bool DoPollGamepad(PhysicalGamepad * physical_gamepad);
	};

#endif

}; // namespace chaos