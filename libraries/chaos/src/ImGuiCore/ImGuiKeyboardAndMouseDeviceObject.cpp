#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiKeyboardAndMouseDeviceObject::DisplayKeyboardAndMouseKeyStates(KeyboardAndMouseDevice const* keyboard_and_mouse_device, char const* table_title, char const * title, bool hide_cold_keys, InputDeviceType key_type) const
	{
		DisplayAllKeyInfo(keyboard_and_mouse_device, table_title, title, [&](Key key, KeyState const & state)
		{
			if (GetKeyInputDevice(key) != key_type)
				return false;

			if (hide_cold_keys)
			{
				if (!state.IsStateInitialized()) // only show values that as been set at least once
					return false;
				if (!state.GetValue() && state.GetSameValueTimer() > 10.0f) // do not bother display up keys for more than 10s
					return false;
			}
			return true;
		});
	};

	void ImGuiKeyboardAndMouseDeviceObject::OnDrawImGuiContent(Window * window)
	{
		KeyboardAndMouseDevice const* keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();
		if (keyboard_and_mouse_device == nullptr)
			return;

		ImGui::SeparatorText("Mouse");
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Mouse Table", "Buttons", false, InputDeviceType::MOUSE);
		DisplayAllInput1DInfo(keyboard_and_mouse_device, "MouseInput1D", "Input1D");
		DisplayAllInput2DInfo(keyboard_and_mouse_device, "MouseInput2D", "Input2D");

		ImGui::Dummy({ 0.0f, 20.0f });

		ImGui::SeparatorText("Keyboard");
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Keyboard Table", "Keys", true, InputDeviceType::KEYBOARD);
	}

}; // namespace chaos