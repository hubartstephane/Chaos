#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGamepadObject::OnDrawImGuiContent(Window * window)
	{
		GameApplication const * game_application = Application::GetConstInstance();
		if (game_application == nullptr)
			return;

		Game const * game = game_application->GetGame();
		if (game == nullptr)
			return;

		GamepadManager const * gamepad_manager = game->GetGamepadManager();
		if (gamepad_manager == nullptr)
			return;

		size_t gamepad_count = gamepad_manager->GetPhysicalGamepadCount();

		for (size_t i = 0 ; i < gamepad_count ; ++i)
		{
			PhysicalGamepad const * physical_gamepad = gamepad_manager->GetPhysicalGamepad(i);
			if (physical_gamepad == nullptr)
				continue;

			if (!physical_gamepad->IsPresent())
				continue;

			GamepadState const * gamepad_state = physical_gamepad->GetGamepadState();

			ImGui::PushID(int(i));

			ImGui::SeparatorText("Gamepad");

			// generic information
			ImGui::Dummy({0.0f, 20.0f});
			if (ImGui::BeginTable("Gamepad State", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Index");
				ImGui::TableNextColumn();
				ImGui::Text("%d", physical_gamepad->GetGamepadIndex());

				ImGui::TableNextColumn();
				ImGui::Text("Allocated");
				ImGui::TableNextColumn();
				ImGui::Text("%s", physical_gamepad->IsAllocated()? "true" : "false");

				ImGui::TableNextColumn();
				ImGui::Text("Buttons");
				ImGui::TableNextColumn();
				ImGui::Text("%d", gamepad_state->GetButtonCount());

				ImGui::TableNextColumn();
				ImGui::Text("Axes");
				ImGui::TableNextColumn();
				ImGui::Text("%d", gamepad_state->GetAxisCount());

				ImGui::EndTable();
			}

			ImGui::Dummy({0.0f, 20.0f});

			// buttons
			EnumMetaData<GamepadButton> const * gamepad_button_metadata = GetEnumMetaData(boost::mpl::identity<GamepadButton>());
			if (gamepad_button_metadata != nullptr)
			{
				WithImGuiInputTable("buttons", [&]()
				{
					gamepad_button_metadata->ForEachEnumValue([&](GamepadButton key)
					{


						DisplayImGuiKeyInfo(key, gamepad_state->GetButtonState(key));
					});
				});
			}

			// axes
			EnumMetaData<GamepadAxis> const * gamepad_axis_metadata = GetEnumMetaData(boost::mpl::identity<GamepadAxis>());
			if (gamepad_axis_metadata != nullptr)
			{
				WithImGuiInputTable("axes", [&]()
				{
					gamepad_axis_metadata->ForEachEnumValue([](GamepadAxis axis)
					{

						
					});
				});
			}
			ImGui::PopID();
		}
	}

}; // namespace chaos