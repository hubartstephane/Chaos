#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiKeyboardAndMouseDeviceObject::DisplayKeyboardAndMouseKeyStates(char const * title, char const * table_title, bool hide_cold_keys, KeyType key_type) const
	{
		KeyboardAndMouseDevice const * keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();
		if (keyboard_and_mouse_device == nullptr)
			return;

		ImGui::SeparatorText(title);

		DisplayAllKeyInfo(table_title, keyboard_and_mouse_device, [&](Key key, KeyState const & state)
		{
			if (GetKeyType(key) != key_type)
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
		DisplayKeyboardAndMouseKeyStates("Mouse", "Mouse Table", false, KeyType::MOUSE);
		DisplayKeyboardAndMouseKeyStates("Keyboard", "Keyboard Table", true, KeyType::KEYBOARD);
	}

}; // namespace chaos