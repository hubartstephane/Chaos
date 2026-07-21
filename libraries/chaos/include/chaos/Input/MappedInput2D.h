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
		static MappedInput2D const keyboard_arrows;
		/** the default mapping with dpad */
		static MappedInput2D const gamepad_dpad;
		/** the unknown object */
		static MappedInput2D const Unknown;

	public:

		/** the key bound to left direction */
		Key left_key = Key::Unknown;
		/** the key bound to right direction */
		Key right_key = Key::Unknown;
		/** the key bound to down direction */
		Key down_key = Key::Unknown;
		/** the key bound to up direction */
		Key up_key = Key::Unknown;
	};

#endif

}; // namespace chaos