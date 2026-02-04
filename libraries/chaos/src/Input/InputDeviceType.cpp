#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<InputDeviceType> const InputDeviceType_metadata =
	{
		{ InputDeviceType::Unknown, "UNKNOWN" },
		{ InputDeviceType::Keyboard, "KEYBOARD" },
		{ InputDeviceType::Mouse, "MOUSE" },
		{ InputDeviceType::Gamepad, "GAMEPAD" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InputDeviceType, &InputDeviceType_metadata, CHAOS_API);

}; // namespace chaos
