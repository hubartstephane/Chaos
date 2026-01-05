namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	using VirtualKeyEnumerationFunction = LightweightFunction<bool(Key, Input1D)>;

	using VirtualInputEnumerationFunction = LightweightFunction<bool(Input1D, Input1D, Input2D)>;

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
		bool TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Key> in_input);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input1D> in_input);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input2D> in_input);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<MappedInput1D> in_input);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		bool TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<MappedInput2D> in_input);

	protected:

		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Key> in_input);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input1D> in_input);
		/** internal method that check whether an input has already been consumed yet Mark it as consumed */
		bool DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input2D> in_input);

	protected:

		/** keys that are consumed */
		std::map<std::pair<Key, KeyState const*>, InputReceiverInterface const*> consumed_keys;
		/** input1D that are consumed */
		std::map<std::pair<Input1D, Input1DState const*>, InputReceiverInterface const*> consumed_input1D;
		/** input2D that are consumed */
		std::map<std::pair<Input2D, Input2DState const*>, InputReceiverInterface const*> consumed_input2D;

		/** the receiver that claimed for all inputs */
		std::optional<InputReceiverInterface const*> all_inputs_consumer;
	};

#endif

}; // namespace chaos