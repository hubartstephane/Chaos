#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInputStateObjectBase::DisplayAxesInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(GamepadAxis, AxisState const &)> filter_func) const
	{
		if (ImGui::BeginTable(title, 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Key", 0);
			ImGui::TableSetupColumn("State", 0);
			ImGui::TableSetupColumn("Min", 0);
			ImGui::TableSetupColumn("Max", 0);
			ImGui::TableSetupColumn("Repeat Timer", 0);
			ImGui::TableHeadersRow();

			in_device_user->ForAllAxes([](GamepadAxis axis, AxisState const & state)
			{
				return false; // don't stop
			});

			ImGui::EndTable();
		}
	}

	void ImGuiInputStateObjectBase::DisplayKeyInfo(Key key, KeyState const & key_state) const
	{
		// early exit for unknown key (whatever type is)
		if (!key.IsValid())
			return;

		// ImGui display code
		if (char const * key_name = key.GetName())
		{
			ImGui::PushID(int(key.GetType()));
			ImGui::PushID(int(key.GetKeyboardButton()));

			bool pressed = key_state.GetValue();

			ImVec4 color = (pressed)? 
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", key_name);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", pressed? "pressed" : "released");

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%f", key_state.GetSameValueTimer());

			ImGui::PopID();
			ImGui::PopID();
		}
	}

	void ImGuiInputStateObjectBase::DisplayKeysInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(Key key, KeyState const &)> filter_func) const
	{
		if (ImGui::BeginTable(title, 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Key", 0);
			ImGui::TableSetupColumn("State", 0);
			ImGui::TableSetupColumn("Repeat Timer", 0);
			ImGui::TableHeadersRow();

			in_device_user->ForAllKeys([&](Key key, KeyState const & state)
			{
				if (filter_func(key, state))
					DisplayKeyInfo(key, state);
				return false; // don't stop
			});

			ImGui::EndTable();
		}
	}

}; // namespace chaos