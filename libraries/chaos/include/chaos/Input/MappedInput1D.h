namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MappedInput1D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * MappedInput1D: some key binding to get an input1D from keys
	 */

	class MappedInput1D
	{
	public:

		/** the default mapping with keyboard */
		static MappedInput1D const keyboard_arrows;
		/** the default mapping with dpad */
		static MappedInput1D const gamepad_dpad;
		/** the unknown object */
		static MappedInput1D const Unknown;

	public:

		/** the key bound to negative direction */
		Key neg_key = Key::Unknown;
		/** the key bound to positive direction */
		Key pos_key = Key::Unknown;
	};

#endif

}; // namespace chaos