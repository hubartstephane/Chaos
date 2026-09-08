#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ModifiersInputCondition::GetModifierKeyPairValue(InputDeviceInterface const* in_input_device, Key key1, Key key2) const
	{
		for (Key key : {key1, key2})
		{
			InputStatus status = in_input_device->GetInputStatus(key);
			if (status == InputStatus::JustActivated || status == InputStatus::RepeatActive)
				return true;
		}
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
		struct ModifierRequest
		{
			KeyModifier modifier;
			Key         key1;
			Key         key2;
		};

		std::array<ModifierRequest, 3> const requests = 
		{
			ModifierRequest{KeyModifier::Alt, Key::LeftAlt, Key::RightAlt},
			ModifierRequest{KeyModifier::Shift, Key::LeftShift, Key::RightShift},
			ModifierRequest{KeyModifier::Control, Key::LeftControl, Key::RightControl}
		};

		if (modifiers != KeyModifier::None)
		{
			for (ModifierRequest const & request : requests)
			{
				if (HasAnyFlags(modifiers, request.modifier))
				{
					bool value = GetModifierKeyPairValue(in_params.input_device, request.key1, request.key2);
					if (value != wanted_value)
						return InputConditionResult::False;
				}
			}
		}
		return InputConditionResult::True;
	}

	ModifiersInputCondition RequireModifiers(KeyModifier in_modifiers)
	{
		return ModifiersInputCondition(in_modifiers, true);
	}

	ModifiersInputCondition ForbidModifiers(KeyModifier in_modifiers)
	{
		return ModifiersInputCondition(in_modifiers, false);
	}

}; // namespace chaos
