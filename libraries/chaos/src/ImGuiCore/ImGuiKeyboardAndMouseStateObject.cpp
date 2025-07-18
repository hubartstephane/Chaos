#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	template<typename ENUM_TYPE>
	void ImGuiKeyboardAndMouseStateObject::DisplayKeyStates(char const * title, char const * table_title, bool hide_cold_keys)
	{
		EnumMetaData<ENUM_TYPE> const * meta = GetEnumMetaData(boost::mpl::identity<ENUM_TYPE>());
		if (meta == nullptr)
			return;

		ImGui::SeparatorText(title);

		WithImGuiInputTable(table_title, [&]()
		{
			meta->ForEachEnumValue([&](ENUM_TYPE key)
			{
				if (ButtonState const * button_state = KeyboardAndMouseState::GetKeyState(key))
				{
					// do not bother display keys up for more than 10s
					if (hide_cold_keys)
						if (!button_state->GetValue() && button_state->GetSameValueTimer() > 10.0f)
							return;

					DisplayImGuiKeyInfo(key, *button_state);
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