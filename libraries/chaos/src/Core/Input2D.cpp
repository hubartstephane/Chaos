#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Input2D> const Input2D_metadata =
	{
		{ Input2D::UNKNOWN, "UNKNOWN" },
		{ Input2D::GAMEPAD_LEFT_STICK, "LEFT_STICK" },
		{ Input2D::GAMEPAD_RIGHT_STICK, "RIGHT_STICK" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Input2D, &Input2D_metadata, CHAOS_API);

}; // namespace chaos
