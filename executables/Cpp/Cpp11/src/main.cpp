#include <chaos/chaos.h>

using namespace chaos;


CHAOS_SPECIALIZE_CLASS_MAPPING(InputValueType, TaggedInput<Key>, bool);
CHAOS_SPECIALIZE_CLASS_MAPPING(InputValueType, TaggedInput<Input1D>, float);
CHAOS_SPECIALIZE_CLASS_MAPPING(InputValueType, TaggedInput<Input2D>, glm::vec2);
CHAOS_SPECIALIZE_CLASS_MAPPING(InputValueType, TaggedInput<MappedInput1D>, float);
CHAOS_SPECIALIZE_CLASS_MAPPING(InputValueType, TaggedInput<MappedInput2D>, glm::vec2);

CHAOS_GENERATE_IS_ANY_OF_CONCEPT(DigitalInput, Key, TaggedInput<Key>);
CHAOS_GENERATE_IS_ANY_OF_CONCEPT(AnalogicInput1D, Input1D, MappedInput1D, TaggedInput<Input1D>, TaggedInput<MappedInput1D>);
CHAOS_GENERATE_IS_ANY_OF_CONCEPT(AnalogicInput2D, Input2D, MappedInput2D, TaggedInput<Input2D>, TaggedInput<MappedInput2D>);

// ---------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------

template<typename ...PARAMS>
requires (
	((sizeof...(PARAMS) > 0)) &&
	(
		((DigitalInput<PARAMS> && ...)) ||
		((AnalogicInput1D<PARAMS> && ...)) ||
		((AnalogicInput2D<PARAMS> && ...))
	)
)
class CombineInput : public InputRequestBase
{
public:

	using tuple_type = std::tuple<PARAMS...>;

	using value_type = InputValueType_t<std::tuple_element_t<0, tuple_type>>; // use the first tuple element to deduce the type of the combined value for the input

	CombineInput(PARAMS... params):
		child_input_requests(std::forward<PARAMS>(params)...){}

	virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const
	{
		std::optional<InputRequestResult> result;

		value_type value = {};
		
		CheckHelper<0>(result, value, in_input_receiver, in_input_device, in_consumption_cache);
	
		return result.value_or(InputRequestResult::Invalid);
	}

protected:

	template<int INDEX>
	void CheckHelper(
		std::optional<InputRequestResult> & result,
		value_type & out_value,
		InputReceiverInterface const* in_input_receiver, 
		InputDeviceInterface const* in_input_device, 
		InputConsumptionCache& in_consumption_cache) const
	{
		if constexpr (INDEX < std::tuple_size_v<tuple_type>)
		{
			auto const & input = std::get<INDEX>(child_input_requests); // can be anything from Key ... Input2D, MappedInput1D ... TaggedInput<...>

			auto const & effective_input = GetInput(input); // remove TaggedInput<...>

			if constexpr (IsSameExt_v<decltype(effective_input), MappedInput1D>)
			{
				int i = 0;
				++i;
			}
			else if constexpr (IsSameExt_v<decltype(effective_input), MappedInput2D>)
			{
				int i = 0;
				++i;
			}
			else
			{

				//int i = 0;
				//++i;

				auto const* input_state = in_input_device->GetInputState(effective_input);
				if (input_state == nullptr)
					return;
			
			}



			//if (!in_consumption_cache.TryConsumeInput(in_input_receiver, in_input_device, input))
			//	return;



			//if (!in_consumption_cache.TryConsumeInput(in_input_receiver, input, in_input_device))
			//{
			
			//}

			CheckHelper<INDEX + 1>(result, out_value, in_input_receiver, in_input_device, in_consumption_cache);
		}
	}

protected:

	std::tuple<PARAMS...> child_input_requests;
};






// ---------------------------------------------------------------------------------------
template<typename ...PARAMS>
requires (
	((sizeof...(PARAMS) > 0)) &&
	(
		((DigitalInput<PARAMS> && ...)) ||
		((AnalogicInput1D<PARAMS> && ...)) ||
		((AnalogicInput2D<PARAMS> && ...))
	)
)
auto Combine(PARAMS const & ...params)
{
	return CombineInput<PARAMS...>(params...);
}



int main(int argc, char** argv, char** env)
{
	InputConsumptionCache cache;

	Combine(ConsultOnly(Input1D::GAMEPAD_LEFT_TRIGGER), MappedInput1D::default_dpad_mapping).Check(nullptr, KeyboardAndMouseDevice::GetInstance(), cache);

//	Combine(Input2D::GAMEPAD_LEFT_STICK, MappedInput2D::default_dpad_mapping).Check(nullptr, nullptr, cache);


	return 0;
}