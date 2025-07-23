#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInputStateObjectBase::DisplayKeyInfo(Key key, KeyState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (!key.IsValid())
			return;

		// ImGui display code
		if (char const * key_name = key.GetName())
		{
			ImGui::PushID(int(key.GetType()));
			ImGui::PushID(int(key.GetKeyboardButton()));

			bool pressed = state.GetValue();

			ImVec4 color = (pressed)? 
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", key_name);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", pressed? "pressed" : "released");

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%f", state.GetSameValueTimer());

			ImGui::PopID();
			ImGui::PopID();
		}
	}

	void ImGuiInputStateObjectBase::DisplayKeysInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(Key, KeyState const &)> filter_func) const
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

	void ImGuiInputStateObjectBase::DisplayAxisInfo(GamepadAxis axis, AxisState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (axis == GamepadAxis::UNKNOWN)
			return;

		// ImGui display code
		if (char const * axis_name = EnumToString(axis))
		{
			ImGui::PushID(int(axis));

			float value = state.GetValue();

			ImVec4 color = (value != 0.0f)?
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", axis_name);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", value);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", state.min_value);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", state.max_value);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%f", state.GetSameValueTimer());

			ImGui::PopID();
		}
	}

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

			in_device_user->ForAllAxes([&](GamepadAxis axis, AxisState const & state)
			{
				if (filter_func(axis, state))
					DisplayAxisInfo(axis, state);
				return false; // don't stop
			});

			ImGui::EndTable();
		}
	}

	void ImGuiInputStateObjectBase::DisplayStickInfo(GamepadStick stick, StickState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (stick == GamepadStick::UNKNOWN)
			return;

		// ImGui display code
		if (char const * stick_name = EnumToString(stick))
		{
			ImGui::PushID(int(stick));

			glm::vec2 value = state.GetValue();

			ImVec4 color = (value.x != 0.0f || value.y != 0.0f)?
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", stick_name);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", value.x);
			ImGui::TextColored(color, "%+f", value.y);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", state.min_value.x);
			ImGui::TextColored(color, "%+f", state.min_value.y);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%+f", state.max_value.x);
			ImGui::TextColored(color, "%+f", state.max_value.y);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%f", state.GetSameValueTimer());

			ImGui::PopID();
		}
	}

	void ImGuiInputStateObjectBase::DisplaySticksInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(GamepadStick, StickState const &)> filter_func) const
	{
		if (ImGui::BeginTable(title, 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Key", 0);
			ImGui::TableSetupColumn("State", 0);
			ImGui::TableSetupColumn("Min", 0);
			ImGui::TableSetupColumn("Max", 0);
			ImGui::TableSetupColumn("Repeat Timer", 0);
			ImGui::TableHeadersRow();

			in_device_user->ForAllSticks([&](GamepadStick stick, StickState const & state)
			{
				if (filter_func(stick, state))
					DisplayStickInfo(stick, state);
				return false; // don't stop
			});

			ImGui::EndTable();
		}
	}

}; // namespace chaos