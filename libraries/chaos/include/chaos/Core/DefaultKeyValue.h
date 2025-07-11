namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	struct DefaultKeyValue;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * DefaultKeyValue: an intermediate value owner for MouseButtonEvent,KeyEvent.
	 */

	template<>
	struct DefaultKeyValue<int>
	{
		static const int value = 0;
	};

	template<>
	struct DefaultKeyValue<KeyboardButton>
	{
		static const KeyboardButton value = KeyboardButton::UNKNOWN;
	};

#endif

}; // namespace chaos