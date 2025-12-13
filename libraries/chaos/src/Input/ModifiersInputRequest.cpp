#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ModifiersInputRequest::GetModifierKeyPairValue(InputDeviceInterface const* in_input_device, Key key1, Key key2) const
	{
		InputStatus key1_status = in_input_device->GetInputStatus(key1);
		if (key1_status == InputStatus::BECOME_ACTIVE || key1_status == InputStatus::STAY_ACTIVE)
			return true;
		InputStatus key2_status = in_input_device->GetInputStatus(key2);
		if (key2_status == InputStatus::BECOME_ACTIVE || key2_status == InputStatus::STAY_ACTIVE)
			return true;
		return false;
	}

	char const* ModifiersInputRequest::GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const
	{
		char buffer[256];
		char const * modifiers_string = EnumToString(modifiers, buffer, sizeof(buffer));
		if (wanted_value)
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod+[%s]", modifiers_string);
		else
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod-[%s]", modifiers_string);
		return debug_info_storage.buffer;
	}

	InputRequestResult ModifiersInputRequest::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const
	{
		if (modifiers != KeyModifier::None)
		{
			if (HasAnyFlags(modifiers, KeyModifier::Alt))
			{
				bool alt_value = GetModifierKeyPairValue(in_input_device, Key::LEFT_ALT, Key::RIGHT_ALT);
				if (alt_value != wanted_value)
					return InputRequestResult::False;
			}

			if (HasAnyFlags(modifiers, KeyModifier::Shift))
			{
				bool shift_value = GetModifierKeyPairValue(in_input_device, Key::LEFT_SHIFT, Key::RIGHT_SHIFT);
				if (shift_value != wanted_value)
					return InputRequestResult::False;
			}

			if (HasAnyFlags(modifiers, KeyModifier::Control))
			{
				bool control_value = GetModifierKeyPairValue(in_input_device, Key::LEFT_CONTROL, Key::RIGHT_CONTROL);
				if (control_value != wanted_value)
					return InputRequestResult::False;
			}
		}
		return InputRequestResult::True;
	}


}; // namespace chaos
