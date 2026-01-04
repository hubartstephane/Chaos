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
		static MappedInput1D const default_keyboard_mapping;
		/** the default mapping with dpad */
		static MappedInput1D const default_dpad_mapping;
		/** the unknown object */
		static MappedInput1D const UNKNOWN;

	public:

		/** the key bound to negative direction */
		Key neg_key = Key::UNKNOWN;
		/** the key bound to positive direction */
		Key pos_key = Key::UNKNOWN;
	};

#endif

}; // namespace chaos