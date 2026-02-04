#include <chaos/chaos.h>

using namespace chaos;

//Key ----> State (Invalid | Rejected | Ok) -----> Query (bool )



#if 0
Key
Input1D
Input2D
MappedInput1D
MappedInput2D

Combine()
Or()
And()
Inspect()
Modifiers()
Any()

Inactive()
JustDeactivated()
InactiveRepeated()
Active()
JustActivated()
ActiveRepeated()

#endif

#if 0

template<InputTypeExt PARAM1, InputTypeExt... PARAMS>
auto Compose(PARAM1 Input1, PARAMS... params)
	requires
(sizeof...(PARAMS) >= 1) &&
((std::is_same_v<InputValue_t<PARAMS>, InputValue_t<PARAM1>>) && ...)
{
	return 666;
}
auto r1 = Compose(Key::A, Key::B);
auto r2 = Compose(Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_FIRST);
auto r3 = Compose(MappedInput1D::default_dpad_mapping, Input1D::GAMEPAD_FIRST);
auto r4 = Compose(Input1D::GAMEPAD_FIRST, MappedInput1D::default_dpad_mapping, Input1D::GAMEPAD_FIRST);
auto r5 = Compose(Input2D::GAMEPAD_LEFT_STICK, MappedInput2D::default_dpad_mapping, Input2D::GAMEPAD_FIRST);

#endif

// --------------------------------------------------------------------------------------------


CHAOS_GENERATE_IS_ANY_OF_CONCEPT(InputConditionValueType,
	Key,
	Input1D,
	Input2D,
	MappedInput1D,
	MappedInput2D,
	bool,
	InputState_t<Key>,
	InputState_t<Input1D>,
	InputState_t<Input2D>,
	InputValue_t<Key>,
	InputValue_t<Input1D>,
	InputValue_t<Input2D>);


enum class CHAOS_API InputConditionResultXXX : int
{
	Success,
	Invalid,  // the request is not properly formed
	Rejected  // the inputs for this request have already been consumed
};

// ----------------------------------------------------------------------------

class InputConditionBaseXXX
{
public:

	virtual InputConditionResultXXX Check(InputConditionCheckParams const& in_params) const
	{
		return InputConditionResultXXX::Success;
	}
};

// ----------------------------------------------------------------------------

template<typename RETURN_TYPE>
class InputConditionXXX : public InputConditionBaseXXX
{
public:

	using type = RETURN_TYPE;

	virtual InputConditionResultXXX Check(InputConditionCheckParams const& in_params) const
	{
		RETURN_TYPE result;
		return CheckWithResult(in_params, result);
	}

	virtual InputConditionResultXXX CheckWithResult(InputConditionCheckParams const& in_params, RETURN_TYPE & out_result) const
	{
		return InputConditionResultXXX::Success;
	}
};

// ----------------------------------------------------------------------------
//
// InputConditionInputXXX <--- constructed with INPUT_TYPE_EXT


template<InputTypeExt INPUT_TYPE_EXT>
class InputConditionInputXXX : public InputConditionXXX<INPUT_TYPE_EXT>
{
public:

	InputConditionInputXXX(INPUT_TYPE_EXT const & in_input):
		input(in_input){}

	virtual InputConditionResultXXX CheckWithResult(InputConditionCheckParams const& in_params, INPUT_TYPE_EXT& out_result) const override
	{
		out_result = input;
		return InputConditionResultXXX::Success;
	}

protected:

	INPUT_TYPE_EXT input = INPUT_TYPE_EXT::UNKNOWN;
};

// ----------------------------------------------------------------------------
//
// InputConditionStateXXX <--- constructed with   INPUT_TYPE_EXT,     InputConditionInputXXX<INPUT_TYPE_EXT>,       InputConditionStateXXX<....> 

template<InputTypeExt INPUT_TYPE_EXT>
class InputConditionStateXXX : public InputConditionXXX<InputState_t<INPUT_TYPE_EXT>>
{
public:


	virtual InputConditionResultXXX CheckWithResult(InputConditionCheckParams const& in_params, InputState_t<INPUT_TYPE_EXT> & out_result) const override
	{
		return InputConditionResultXXX::Success;
	}


};

// ----------------------------------------------------------------------------

class InputConditionSuccessXXX : public InputConditionXXX<bool>
{
public:

};

// ----------------------------------------------------------------------------

template<typename CHILD_CONDITION>
class InputConditionCheckState : public InputConditionSuccessXXX
{
public:

	InputConditionCheckState(InputStatus in_status, CHILD_CONDITION const & in_child_condition):
		child_condition(in_child_condition),
		status(in_status)
	{
	}

	virtual InputConditionResultXXX CheckWithResult(InputConditionCheckParams const& in_params, bool& out_result) const override
	{
		CHILD_CONDITION::type state;

		InputConditionResultXXX child_result = child_condition.CheckWithResult(in_params, state);
		if (child_result == InputConditionResultXXX::Success)
		{
		
		}
		return child_result;
	}

protected:

	InputStatus status = InputStatus::NONE;
	CHILD_CONDITION child_condition;
};



#if 0
template<typename... PARAMS>
auto OrXXX(PARAMS... params)
{
	auto TransformToInputCondition(auto src)
	{
		if constexpr (std::is_base_of_v<InputConditionBaseXXX,T>)
			return src;
		else if constexpr (InputType, T>)

	};

	return InputConditionOr<sizeof...(PARAMS)>
	(
		
	);

}


#endif

template<typename T>
concept InputConditionType = (std::is_base_of_v<InputConditionBaseXXX, T> || InputType<T>);

template<InputConditionType T>
decltype(auto) TransformToInputCondition(T const& src)
{
	if constexpr (std::is_base_of_v<InputConditionBaseXXX, T>)
		return src;
	else if constexpr (InputType<T>)
		return InputConditionInputXXX(src);
	else
		static_assert(0);
}

template<InputConditionType T>
auto TransformToInputCondition(T && src)
{
	if constexpr (std::is_base_of_v<InputConditionBaseXXX, T>)
		return src;
	else if constexpr (InputType<T>)
		return InputConditionInputXXX(src);
	else
		static_assert(0);
}

template<InputConditionType PARAM0, InputConditionType... PARAMS>
auto OrX(PARAM0 param0, PARAMS... params) requires ((std::is_same_v<std::remove_cvref_t<PARAM0>, std::remove_cvref_t<PARAMS>>) && ...)
{
	return 666;
}

#if 0

template<InputTypeExt PARAM1, InputTypeExt... PARAMS>
auto Compose(PARAM1 Input1, PARAMS... params)
	requires
(sizeof...(PARAMS) >= 1) &&
((std::is_same_v<InputValue_t<PARAMS>, InputValue_t<PARAM1>>) && ...)
{
	return 666;
}

#endif

int main(int argc, char** argv, char** env)
{


	if (InputType<Key>)
		argc = 0;

	decltype(auto) p1 = TransformToInputCondition(Key::UNKNOWN);

	decltype(auto) p2 = TransformToInputCondition(TransformToInputCondition(Key::UNKNOWN));


	InputConditionStateXXX<Key> CS;
	decltype(auto) p3 = TransformToInputCondition(CS);

	auto x = OrX(p1,p1,p1);


	return 0;
}