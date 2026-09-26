#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiKeyboardAndMouseDeviceObject::DisplayKeyboardAndMouseKeyStates(KeyboardAndMouseDevice const* keyboard_and_mouse_device, char const* table_title, char const * title, InputDeviceType key_type) const
	{
		DisplayAllKeyInfo(keyboard_and_mouse_device, table_title, title, [&](Key key, KeyState const & state)
		{
			if (GetDeviceForInput(key) != key_type)
				return false;
			return true;
		});
	};

	void ImGuiKeyboardAndMouseDeviceObject::OnDrawImGuiContent(Window * window)
	{
		KeyboardAndMouseDevice const* keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();
		if (keyboard_and_mouse_device == nullptr)
			return;

		ImGui::SeparatorText("Mouse");
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Mouse Table", "Buttons", InputDeviceType::Mouse);
		DisplayAllInput1DInfo(keyboard_and_mouse_device, "MouseInput1D", "Input1D");
		DisplayAllInput2DInfo(keyboard_and_mouse_device, "MouseInput2D", "Input2D");

		ImGui::Dummy({ 0.0f, 20.0f });

		ImGui::SeparatorText("Keyboard");
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Keyboard Table", "Keys", InputDeviceType::Keyboard);
	}

}; // namespace chaos