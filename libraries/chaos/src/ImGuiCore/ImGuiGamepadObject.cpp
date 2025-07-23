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

			// features
			DisplayKeysInfo("buttons", gamepad_state, [&](Key key, KeyState const & state)
			{
				return (key.GetType() == KeyType::GAMEPAD);
			});
			DisplayAxesInfo("axes", physical_gamepad,[&](GamepadAxis axis, AxisState const & state)
			{
				return true;
			});
			DisplaySticksInfo("sticks", physical_gamepad, [&](GamepadStick stick, StickState const & state)
			{
				return true;
			});

			ImGui::PopID();
		}
	}

}; // namespace chaos