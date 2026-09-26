#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiKeyboardAndMouseDeviceObject::DisplayKeyboardAndMouseKeyStates(KeyboardAndMouseDevice const* keyboard_and_mouse_device, char const* table_title, char const * title, bool ignore_unknown_keys, InputDeviceType key_type) const
	{
		DisplayAllKeyInfo(keyboard_and_mouse_device, table_title, title, [&](Key key, KeyState const & state)
		{
			if (GetDeviceForInput(key) != key_type)
				return false;

			if (ignore_unknown_keys)
			{
				double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

				if (state.IsActive())
				{
					last_active_key_times[key] = frame_time;
					return true;
				}

				auto it = last_active_key_times.find(key);
				if (it == last_active_key_times.end())
				{
					return false;
				}

				if (ignore_cold_keys)
				{
					if (frame_time - it->second > 10.0f)
					{
						last_active_key_times.erase(it);
						return false;
					}
				}
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
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Mouse Table", "Buttons", false, InputDeviceType::Mouse);
		DisplayAllInput1DInfo(keyboard_and_mouse_device, "MouseInput1D", "Input1D");
		DisplayAllInput2DInfo(keyboard_and_mouse_device, "MouseInput2D", "Input2D");

		ImGui::Dummy({ 0.0f, 20.0f });

		ImGui::SeparatorText("Keyboard");
		ImGui::Checkbox("ignore cold keys", &ignore_cold_keys);
		DisplayKeyboardAndMouseKeyStates(keyboard_and_mouse_device, "Keyboard Table", "Keys", true, InputDeviceType::Keyboard);
	}

}; // namespace chaos