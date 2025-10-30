#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace InputTools
	{
		bool EnumerateVirtualKey(VirtualKeyEnumerationFunction func)
		{
			if (func(Key::GAMEPAD_LEFT_TRIGGER, Input1D::GAMEPAD_LEFT_TRIGGER))
				return true;
			if (func(Key::GAMEPAD_RIGHT_TRIGGER, Input1D::GAMEPAD_RIGHT_TRIGGER))
				return true;
			return false;
		}

		bool EnumerateVirtualInput2D(VirtualInput2DEnumerationFunction func)
		{
			if (func(Input2D::GAMEPAD_LEFT_STICK, Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y))
				return true;
			if (func(Input2D::GAMEPAD_RIGHT_STICK, Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y))
				return true;
			if (func(Input2D::MOUSE_WHEEL, Input1D::MOUSE_WHEEL_X, Input1D::MOUSE_WHEEL_Y))
				return true;

			return false;
		}

		bool EnumerateRelatedInputs(Key in_input, VirtualKeyEnumerationFunction func)
		{
			return EnumerateVirtualKey([&](Key key, Input1D input)
			{
				if (key != in_input)
					return false;
				return func(key, input);
			});
		}

		bool EnumerateRelatedInputs(Input1D in_input, VirtualKeyEnumerationFunction key_func, VirtualInput2DEnumerationFunction input2D_func)
		{
			return EnumerateVirtualKey([&](Key key, Input1D input)
			{
				if (in_input != input)
					return false;
				return key_func(key, input);
			}) 
			&&
			EnumerateVirtualInput2D([&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
			{
				if (in_input != input1D_x && in_input != input1D_y)
					return false;
				return input2D_func(input2D, input1D_x, input1D_y);
			});
		}

		bool EnumerateRelatedInputs(Input2D in_input, VirtualInput2DEnumerationFunction func)
		{
			return EnumerateVirtualInput2D([&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
			{
				if (in_input != input2D)
					return false;
				return func(input2D, input1D_x, input1D_y);
			});
		}

	}; // namespace InputTools

}; // namespace chaos
