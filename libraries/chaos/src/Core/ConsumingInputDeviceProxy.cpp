#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const* ConsumingInputDeviceProxy::DoGetKeyState(Key key) const override
	{

		return nullptr;
	}

	AxisState const* ConsumingInputDeviceProxy::DoGetAxisState(GamepadAxis axis) const override
	{

		return nullptr;
	}
	
	StickState const* ConsumingInputDeviceProxy::DoGetStickState(GamepadStick stick) const override
	{

		return nullptr;
	}
}; // namespace chaos
