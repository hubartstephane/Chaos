#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	template<typename ENUM_TYPE>
	void ImGuiKeyboardAndMouseStateObject::DisplayKeyStates(char const * title, char const * table_title, bool hide_cold_keys)
	{
		KeyboardAndMouseState const * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance();
		if (keyboard_and_mouse_state == nullptr)
			return;

		EnumMetaData<ENUM_TYPE> const * meta = GetEnumMetaData(boost::mpl::identity<ENUM_TYPE>());
		if (meta == nullptr)
			return;

		ImGui::SeparatorText(title);

		WithImGuiInputTable(table_title, [&]()
		{
			meta->ForEachEnumValue([&](ENUM_TYPE key)
			{
				if (ButtonState const * button_state = keyboard_and_mouse_state->GetKeyState(key))
				{
					if (hide_cold_keys)
					{
						if (!button_state->IsStateInitialized()) // only show values that as been set at least once
							return;
						if (!button_state->GetValue() && button_state->GetSameValueTimer() > 10.0f) // do not bother display up keys for more than 10s
							return;
					}
					DisplayImGuiKeyInfo(key, button_state);
				}
			});
		});
	};

	void ImGuiKeyboardAndMouseStateObject::OnDrawImGuiContent(Window * window)
	{
		DisplayKeyStates<MouseButton>("Mouse", "Mouse Table", false);
		DisplayKeyStates<KeyboardButton>("Keyboard", "Keyboard Table", true);
	}

}; // namespace chaos