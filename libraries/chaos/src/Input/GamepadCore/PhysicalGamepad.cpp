#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const* PhysicalGamepad::GetName() const
	{
		if (is_present)
			return glfwGetJoystickName(int(gamepad_index));
		return nullptr;
	}

	char const * PhysicalGamepad::GetGUID() const
	{
		if (is_present)
			return glfwGetJoystickGUID(int(gamepad_index));
		return nullptr;
	}

	void PhysicalGamepad::ClearInputs()
	{
		gamepad_state.Clear();
	}

	bool PhysicalGamepad::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		if (IsPresent())
			if (gamepad_state.EnumerateDeviceHierarchy(func))
				return true;
		return InputDeviceInterface::EnumerateDeviceHierarchy(func);
	}

	void PhysicalGamepad::UpdateAxisAndButtons(GamepadInputFilterSettings const& in_filter_settings)
	{
		if (!IsPresent())
			return;
		gamepad_state.UpdateAxisAndButtons(gamepad_index, in_filter_settings);
	}

	Gamepad* PhysicalGamepad::CaptureDevice(GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks)
	{
		return gamepad_manager->DoCaptureDevice(this, in_filter_settings, in_callbacks);
	}

}; // namespace chaos
