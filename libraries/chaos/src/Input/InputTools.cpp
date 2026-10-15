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
				VirtualKeyDefinition{Key::GamepadLeftTrigger, Input1D::GamepadLeftTrigger},
				VirtualKeyDefinition{Key::GamepadRightTrigger, Input1D::GamepadRightTrigger}
			};

			for (VirtualKeyDefinition const & def : definitions)
				if (func(def))
					return true;
			return false;
		}

		bool EnumerateVirtualInput2DDefinition(VirtualInput2DDefinitionEnumerationFunction func)
		{
			static const std::array<VirtualInput2DDefinition, 4> definitions =
			{
				VirtualInput2DDefinition { Input2D::GamepadLeftStick, Input1D::GamepadLeftAxisX, Input1D::GamepadLeftAxisY },
				VirtualInput2DDefinition { Input2D::GamepadRightStick, Input1D::GamepadRightAxisX, Input1D::GamepadRightAxisY },
				VirtualInput2DDefinition { Input2D::MouseDelta, Input1D::MouseDeltaX, Input1D::MouseDeltaY },
				VirtualInput2DDefinition { Input2D::MouseWheel, Input1D::MouseWheelX, Input1D::MouseWheelY }
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

		size_t GetInputString(MappedInput1D const & in_input, char * in_buffer, size_t in_bufsize)
		{
			size_t initial_bufsize = in_bufsize;

			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, "MappedInput1D(") &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.neg_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, " / ") && 
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.pos_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, ")");

			return size_t(initial_bufsize - in_bufsize);
		}

		size_t GetInputString(MappedInput2D const & in_input, char * in_buffer, size_t in_bufsize)
		{
			size_t initial_bufsize = in_bufsize;

			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, "MappedInput2D(") &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.left_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, " / ") && 
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.right_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, " / ") &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.up_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, " / ") &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, GetBasicInputName(in_input.down_key)) &&
			StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, ")");

			return size_t(initial_bufsize - in_bufsize);
		}

	}; // namespace InputTools

}; // namespace chaos
