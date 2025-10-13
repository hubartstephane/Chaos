#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInputStateObjectBase::DisplayKeyInfo(Key key, KeyState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (key == Key::UNKNOWN)
			return;

		// ImGui display code
		if (char const * key_name = EnumToString(key))
		{
			ImGui::PushID(int(key));

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
		}
	}

	void ImGuiInputStateObjectBase::DisplayAllKeyInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllKeysFunction filter_func) const
	{
		ImGuiTools::DrawImGuiTable(title, {}, "Key", "State", "Repeat Timer")([&]()
		{
			in_input_device->ForAllKeys([&](Key key, KeyState const & state)
			{
				if (filter_func(key, state))
					DisplayKeyInfo(key, state);
				return false; // don't stop
			});
		});
	}

	void ImGuiInputStateObjectBase::DisplayInput1DInfo(Input1D input, Input1DState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (input == Input1D::UNKNOWN)
			return;

		// ImGui display code
		if (char const * input_name = EnumToString(input))
		{
			ImGui::PushID(int(input));

			float value = state.GetValue();

			ImVec4 color = (value != 0.0f)?
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", input_name);

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

	void ImGuiInputStateObjectBase::DisplayAllInput1DInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllInput1DFunction filter_func) const
	{
		ImGuiTools::DrawImGuiTable(title, {}, "Axis", "State", "Min", "Max", "Repeat Timer")([&]()
		{
			in_input_device->ForAllAxes([&](Input1D input, Input1DState const & state)
			{
				if (filter_func(input, state))
					DisplayInput1DInfo(input, state);
				return false; // don't stop
			});
		});
	}

	void ImGuiInputStateObjectBase::DisplayInput2DInfo(Input2D input, Input2DState const & state) const
	{
		// early exit for unknown key (whatever type is)
		if (input == Input2D::UNKNOWN)
			return;

		// ImGui display code
		if (char const * input_name = EnumToString(input))
		{
			ImGui::PushID(int(input));

			glm::vec2 value = state.GetValue();

			ImVec4 color = (value.x != 0.0f || value.y != 0.0f)?
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::TextColored(color, "%s", input_name);

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

	void ImGuiInputStateObjectBase::DisplayAllInput2DInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllInput2DFunction filter_func) const
	{
		ImGuiTools::DrawImGuiTable(title, {}, "Stick", "State", "Min", "Max", "Repeat Timer")([&]()
		{
			in_input_device->ForAllSticks([&](Input2D input, Input2DState const & state)
			{
				if (filter_func(input, state))
					DisplayInput2DInfo(input, state);
				return false; // don't stop
			});
		});
	}

}; // namespace chaos