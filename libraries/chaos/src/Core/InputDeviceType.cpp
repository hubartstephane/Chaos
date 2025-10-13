#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<InputDeviceType> const InputDeviceType_metadata =
	{
		{ InputDeviceType::UNKNOWN, "UNKNOWN" },
		{ InputDeviceType::KEYBOARD, "KEYBOARD" },
		{ InputDeviceType::MOUSE, "MOUSE" },
		{ InputDeviceType::GAMEPAD, "GAMEPAD" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InputDeviceType, &InputDeviceType_metadata, CHAOS_API);

}; // namespace chaos
