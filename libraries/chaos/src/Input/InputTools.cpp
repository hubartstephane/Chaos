#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace InputTools
	{
		bool EnumerateVirtualKeyDefinition(VirtualKeyDefinitionEnumerationFunction func)
		{
			static const std::array<VirtualKeyDefinition, 2> definitions = 
			{
				VirtualKeyDefinition{Key::GAMEPAD_LEFT_TRIGGER, Input1D::GAMEPAD_LEFT_TRIGGER},
				VirtualKeyDefinition{Key::GAMEPAD_RIGHT_TRIGGER, Input1D::GAMEPAD_RIGHT_TRIGGER}
			};

			for (VirtualKeyDefinition const & def : definitions)
				if (func(def))
					return true;
			return false;
		}

		bool EnumerateVirtualInput2DDefinition(VirtualInput2DDefinitionEnumerationFunction func)
		{
			static const std::array<VirtualInput2DDefinition, 3> definitions =
			{
				VirtualInput2DDefinition { Input2D::GAMEPAD_LEFT_STICK, Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y },
				VirtualInput2DDefinition { Input2D::GAMEPAD_RIGHT_STICK, Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y },
				VirtualInput2DDefinition { Input2D::MOUSE_WHEEL, Input1D::MOUSE_WHEEL_X, Input1D::MOUSE_WHEEL_Y }
			};

			for (VirtualInput2DDefinition const& def : definitions)
				if (func(def))
					return true;
			return false;
		}

		bool EnumerateRelatedInputsDefinition(Key in_input, VirtualKeyDefinitionEnumerationFunction func)
		{
			return EnumerateVirtualKeyDefinition([&](VirtualKeyDefinition const & def)
			{
				if (in_input != def.key)
					return false;
				return func(def);
			});
		}

		bool EnumerateRelatedInputsDefinition(Input1D in_input, VirtualKeyDefinitionEnumerationFunction key_func, VirtualInput2DDefinitionEnumerationFunction input2D_func)
		{
			return EnumerateVirtualKeyDefinition([&](VirtualKeyDefinition const& def)
			{
				if (in_input != def.input1D)
					return false;
				return key_func(def);
			}) 
			&&
			EnumerateVirtualInput2DDefinition([&](VirtualInput2DDefinition const& def)
			{
				if (in_input != def.input1D_x && in_input != def.input1D_y)
					return false;
				return input2D_func(def);
			});
		}

		bool EnumerateRelatedInputsDefinition(Input2D in_input, VirtualInput2DDefinitionEnumerationFunction func)
		{
			return EnumerateVirtualInput2DDefinition([&](VirtualInput2DDefinition const& def)
			{
				if (in_input != def.input2D)
					return false;
				return func(def);
			});
		}

	}; // namespace InputTools

}; // namespace chaos
