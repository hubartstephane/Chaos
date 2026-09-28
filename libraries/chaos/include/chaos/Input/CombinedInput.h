namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

template<typename FIRST, typename... PARAMS>
concept SameInputType =
(
	std::same_as<InputValue_t<FIRST>, InputValue_t<PARAMS>> && ...
);

template<typename... PARAMS>
requires SameInputType<PARAMS...>
class CompositeInput;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* CompositeInput: an object to aggregate some input by a OR operation
	*/

template<typename... PARAMS>
requires SameInputType<PARAMS...>
class CompositeInput
{
public:

	using input_value_type = InputValue_t<std::tuple_element_t<0, std::tuple<PARAMS...>>>;
	using input_state_type = InputState_t<std::tuple_element_t<0, std::tuple<PARAMS...>>>;

	/** constructor */
	CompositeInput(PARAMS... params) :
		child_inputs(std::forward<PARAMS>(params)...) {}

public:

	/** the input requests in the composition */
	std::tuple<PARAMS...> child_inputs;
};

#endif

}; // namespace chaos