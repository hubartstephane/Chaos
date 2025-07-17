#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	bool ImGuiInputStateObjectBase::WithImGuiInputTable(char const * table_title, LightweightFunction<void()> func)
	{
		if (ImGui::BeginTable(table_title, 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Key", 0);
			ImGui::TableSetupColumn("State", 0);
			ImGui::TableSetupColumn("Repeat Timer", 0);
			ImGui::TableHeadersRow();

			func();

			ImGui::EndTable();
			return true;
		}
		return false;
	}

	bool ImGuiInputStateObjectBase::DisplayImGuiKeyInfo(Key key, ButtonState const & button_state)
	{
		// early exit for unknown key (whatever type is)
		if (!key.IsValid())
			return false;

		// gets an ID for the key depending of key type and Key value
		int ID = 0;
		switch (key.GetType())
		{
		case KeyType::KEYBOARD:
			ID = int(key.GetKeyboardButton()) * 10 + 0; break;
		case KeyType::MOUSE:
			ID = int(key.GetMouseButton()) * 10 + 1; break;
		case KeyType::GAMEPAD:
			ID = int(key.GetGamepadButton()) * 10 + 2; break;
		default:
			assert(0);
			return false;
		}

		// ImGui display code
		if (char const * key_name = key.GetName())
		{
			ImGui::PushID(ID);

			bool pressed = button_state.GetValue();

			ImVec4 color = (pressed)? 
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", key_name);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", pressed? "pressed" : "released");

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%f", button_state.GetSameValueTimer());

			ImGui::PopID();

			return true;
		}
		return false;
	}

}; // namespace chaos