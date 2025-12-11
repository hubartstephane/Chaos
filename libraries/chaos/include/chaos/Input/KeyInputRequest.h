namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyInputRequest;

	enum class KeyStatusRequestType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyStatusRequestType: The requested input status
	*/

	enum class CHAOS_API KeyStatusRequestType : int
	{
		Up,
		Release,
		Press,
		Repeat,
		Down
	};

	CHAOS_DECLARE_ENUM_METHOD(KeyStatusRequestType, CHAOS_API);

	/**
	* KeyInputRequest: a specialization of InputRequestBase for a single key
	*/

	class CHAOS_API KeyInputRequest : public InputRequestBase
	{

	public:

		using InputRequestBase::IsRequestRelatedTo; // mandatory elsewhere IsRequestRelatedTo(input1D/2D) will be hidden by the single override method

		/** default constructor */
		KeyInputRequest() = default;
		/** copy constructor */
		KeyInputRequest(KeyInputRequest const & src) = default;
		/** constructor with initialization */
		KeyInputRequest(Key const & in_key, KeyStatusRequestType in_request_type = KeyStatusRequestType::Press):
			key(in_key),
			request_type(in_request_type)
		{}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override;

		/** override */
		virtual char const * GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const override;

	public:

		/** the concerned key */
		Key key;
		/** the required state of the key */
		KeyStatusRequestType request_type = KeyStatusRequestType::Press;
	};

	/**
	* Some request getters
	**/

	CHAOS_API KeyInputRequest KeyUp(Key key);

	CHAOS_API KeyInputRequest KeyRepeat(Key key);

	CHAOS_API KeyInputRequest KeyPressed(Key key);

	CHAOS_API KeyInputRequest KeyReleased(Key key);

	CHAOS_API KeyInputRequest KeyDown(Key key);

#endif

}; // namespace chaos