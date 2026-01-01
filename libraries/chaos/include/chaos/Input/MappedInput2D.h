namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MappedInput2D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * MappedInput2D: some key binding to get an input2D from keys
	 */

	class MappedInput2D
	{
	public:

		/** the default mapping with keyboard */
		static MappedInput2D const default_keyboard_mapping;
		/** the default mapping with dpad */
		static MappedInput2D const default_dpad_mapping;

	public:

		/** the key bound to left direction */
		Key left_key = Key::UNKNOWN;
		/** the key bound to right direction */
		Key right_key = Key::UNKNOWN;
		/** the key bound to down direction */
		Key down_key = Key::UNKNOWN;
		/** the key bound to up direction */
		Key up_key = Key::UNKNOWN;
	};

#endif

}; // namespace chaos