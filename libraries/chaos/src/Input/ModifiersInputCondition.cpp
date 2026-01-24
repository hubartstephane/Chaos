#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ModifiersInputCondition::GetModifierKeyPairValue(InputDeviceInterface const* in_input_device, Key key1, Key key2) const
	{
		InputStatus key1_status = in_input_device->GetInputStatus(key1);
		if (key1_status == InputStatus::BECOME_ACTIVE || key1_status == InputStatus::STAY_ACTIVE)
			return true;
		InputStatus key2_status = in_input_device->GetInputStatus(key2);
		if (key2_status == InputStatus::BECOME_ACTIVE || key2_status == InputStatus::STAY_ACTIVE)
			return true;
		return false;
	}

	char const* ModifiersInputCondition::GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const
	{
		char buffer[256];
		char const * modifiers_string = EnumToString(modifiers, buffer, sizeof(buffer));
		if (wanted_value)
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod+[%s]", modifiers_string);
		else
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod-[%s]", modifiers_string);
		return debug_info_storage.buffer;
	}

	InputConditionResult ModifiersInputCondition::Check(InputConditionCheckParams const& in_params) const
	{
		if (modifiers != KeyModifier::None)
		{
			if (HasAnyFlags(modifiers, KeyModifier::Alt))
			{
				bool alt_value = GetModifierKeyPairValue(in_params.input_device, Key::LEFT_ALT, Key::RIGHT_ALT);
				if (alt_value != wanted_value)
					return InputConditionResult::False;
			}

			if (HasAnyFlags(modifiers, KeyModifier::Shift))
			{
				bool shift_value = GetModifierKeyPairValue(in_params.input_device, Key::LEFT_SHIFT, Key::RIGHT_SHIFT);
				if (shift_value != wanted_value)
					return InputConditionResult::False;
			}

			if (HasAnyFlags(modifiers, KeyModifier::Control))
			{
				bool control_value = GetModifierKeyPairValue(in_params.input_device, Key::LEFT_CONTROL, Key::RIGHT_CONTROL);
				if (control_value != wanted_value)
					return InputConditionResult::False;
			}
		}
		return InputConditionResult::True;
	}


}; // namespace chaos
