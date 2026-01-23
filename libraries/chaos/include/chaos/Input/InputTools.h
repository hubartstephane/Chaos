namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class VirtualKeyDefinition;

	class VirtualInput2DDefinition;

	using VirtualKeyDefinitionEnumerationFunction = LightweightFunction<bool(VirtualKeyDefinition const &)>;

	using VirtualInput2DDefinitionEnumerationFunction = LightweightFunction<bool(VirtualInput2DDefinition const &)>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * VirtualKeyDefinition: describe a relation between a Key and an Input1D. In practise, the key is just an interpretation of the Input1D as a bool input
	 */

	class VirtualKeyDefinition
	{
	public:

		Key     key     = Key::UNKNOWN;
		Input1D input1D = Input1D::UNKNOWN;
	};

	/**
	 * VirtualInput2DDefinition: describe a relation between a Input2D and two Input1D. In practise, the Input2D is just an interpretation of the two Input1D as a vec2 input
	 */

	class VirtualInput2DDefinition
	{
	public:

		Input2D input2D   = Input2D::UNKNOWN;
		Input1D input1D_x = Input1D::UNKNOWN;
		Input1D input1D_y = Input1D::UNKNOWN;
	};

	namespace InputTools
	{
		/** enumerate all relations Key <-> input1D */
		bool CHAOS_API EnumerateVirtualKeyDefinition(VirtualKeyDefinitionEnumerationFunction func);
		/** enumerate all relations input2D <-> Input1D, Input1D */
		bool CHAOS_API EnumerateVirtualInput2DDefinition(VirtualInput2DDefinitionEnumerationFunction func);

		/** enumerate all relations involving some Key */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Key in_input, VirtualKeyDefinitionEnumerationFunction func);
		/** enumerate all relations involving some Input1D */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Input1D in_input, VirtualKeyDefinitionEnumerationFunction key_func, VirtualInput2DDefinitionEnumerationFunction input2D_func);
		/** enumerate all relations involving some Input2D */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Input2D in_input, VirtualInput2DDefinitionEnumerationFunction func);

	}; // namespace InputTools

#endif

}; // namespace chaos