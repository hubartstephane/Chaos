namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

	enum class InputConsumptionFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputConsumptionFlags, CHAOS_API);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	using VirtualKeyEnumerationFunction = LightweightFunction<bool(Key, Input1D)>;

	using VirtualInputEnumerationFunction = LightweightFunction<bool(Input1D, Input1D, Input2D)>;

	/**
	 * InputConsumptionFlags: a bitfields to customize InputConsumptionCache request
	 */

	enum class InputConsumptionFlags : int
	{
		NONE = 0,
		CONSULT_ONLY = 1 // consult cache (so call can fail) but do not lock resource for further calls
	};

	/**
	 * InputConsumptionCache: an object that keeps that trace of which inputs have been handled this frame
	 * 
	 * XXX: A trace of the input receiver consuming an input is kept. The same input may be further request but only by the same consumer
	 *      (nullptr is a valid consumer meaning no one will ever be allowed to consumed the input)
	 *      The same is true for SetConsumeAllInputs(...)
	 * 
	 * XXX: among Keyboard, Mouse and Gamepad the State is a persistent pointer that may be used as a map key
	 * 
	 *      there is no such concept for MappedInput1D/MappedInput2D
	 */

	class CHAOS_API InputConsumptionCache
	{

	public:

		/** all incomming input requests will be considered as consumed */
		void SetConsumeAllInputs(InputReceiverInterface const* in_input_receiver);

		/** clear the cache */
		void Clear();

		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags = InputConsumptionFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags = InputConsumptionFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags = InputConsumptionFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags = InputConsumptionFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags = InputConsumptionFlags::NONE);

	protected:

		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionFlags in_flags);

	protected:

		/** keys that are consumed */
		std::map<Key, InputReceiverInterface const*> consumed_keys;
		/** input1D that are consumed */
		std::map<Input1D, InputReceiverInterface const*> consumed_input1D;
		/** input2D that are consumed */
		std::map<Input2D, InputReceiverInterface const*> consumed_input2D;

		/** the receiver that claimed for all inputs */
		std::optional<InputReceiverInterface const*> all_inputs_consumer;
	};

#endif

}; // namespace chaos