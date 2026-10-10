namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

template<typename FIRST, typename... PARAMS>
concept SameInputValueType =
(
	std::same_as<InputTypeToValueType_t<FIRST>, InputTypeToValueType_t<PARAMS>> && ...
);

template<typename... PARAMS>
requires SameInputValueType<PARAMS...>
class CombinedInput;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* CombinedInput: an object to aggregate some input by a OR operation
	*/

template<typename... PARAMS>
requires SameInputValueType<PARAMS...>
class CombinedInput
{
public:

	using tuple_type = std::tuple<PARAMS...>;
	using first_tuple_element = std::tuple_element_t<0, tuple_type>;

	using input_value_type = InputTypeToValueType_t<first_tuple_element>;
	using input_state_type = InputTypeToStateType_t<first_tuple_element>;

	/** constructor */
	CombinedInput(PARAMS... params) :
		child_inputs(std::forward<PARAMS>(params)...) {}

public:

	/** the input requests in the composition */
	tuple_type child_inputs;
};

#endif

}; // namespace chaos