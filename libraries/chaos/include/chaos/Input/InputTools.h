namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	using VirtualKeyEnumerationFunction = LightweightFunction<bool(Key, Input1D)>;

	using VirtualInput2DEnumerationFunction = LightweightFunction<bool(Input2D, Input1D, Input1D)>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	namespace InputTools
	{
		/** enumerate all relation Key <- input1D */
		bool CHAOS_API EnumerateVirtualKey(VirtualKeyEnumerationFunction func);
		/** enumerate all relation input2D <- Input1D, Input1D */
		bool CHAOS_API EnumerateVirtualInput2D(VirtualInput2DEnumerationFunction func);

		/** enumerate all relations implying some Key */
		bool CHAOS_API EnumerateRelatedInputs(Key in_input, VirtualKeyEnumerationFunction func);
		/** enumerate all relations implying some Input1D */
		bool CHAOS_API EnumerateRelatedInputs(Input1D in_input, VirtualKeyEnumerationFunction key_func, VirtualInput2DEnumerationFunction input2D_func);
		/** enumerate all relations implying some Input2D */
		bool CHAOS_API EnumerateRelatedInputs(Input2D in_input, VirtualInput2DEnumerationFunction func);

	}; // namespace InputTools

#endif

}; // namespace chaos