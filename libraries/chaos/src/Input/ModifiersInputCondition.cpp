#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const* ModifiersInputCondition::GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const
	{
		char buffer[256];
		char const * modifiers_string = EnumToString(modifiers, buffer, sizeof(buffer));
		if (wanted_value)
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod+(%s)", modifiers_string);
		else
			std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mod-(%s)", modifiers_string);
		return debug_info_storage.buffer;
	}

	InputConditionResult ModifiersInputCondition::Check(InputConditionCheckParams const& in_params) const
	{
		InputDeviceTypeMask involved_devices = InputDeviceTypeMask::None;

		if (modifiers != KeyModifier::None)
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

			auto GetModifierKeyPairValue = [&](Key key1, Key key2) 
			{
				for (Key key : {key1, key2})
				{
					InputStatus status = in_params.input_device->GetInputStatus(key);
					if (status == InputStatus::JustActivated || status == InputStatus::RepeatActive)
						return true;
				}
				return false;
			};

			for (ModifierRequest const & request : requests)
			{
				if (HasAnyFlags(modifiers, request.modifier))
				{
					bool value = GetModifierKeyPairValue(request.key1, request.key2);
					if (value)
						involved_devices |= InputDeviceTypeMask::Keyboard;
					if (value != wanted_value)
						return { InputConditionResultType::False, InputDeviceTypeMask::None, XXX(nullptr) };
				}
			}
		}
		return {InputConditionResultType::True, involved_devices, XXX(nullptr) };
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
