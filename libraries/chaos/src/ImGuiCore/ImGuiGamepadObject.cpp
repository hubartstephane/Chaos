#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGamepadObject::OnDrawImGuiContent(Window * window)
	{
		WindowApplication const * window_application = Application::GetConstInstance();
		if (window_application == nullptr)
			return;

		GamepadManager const * gamepad_manager = window_application->GetGamepadManager();
		if (gamepad_manager == nullptr)
			return;

		size_t gamepad_count = gamepad_manager->GetPhysicalGamepadCount();
		if (gamepad_count == 0)
			return;

		// generate a table with a header lambda, calling delegate to fill the content
		auto DisplayTable = [&](char const * column0_title, auto const & fill_content_func)
		{
#define CHAOS_GENERATE_GAMEPAD_TITLE(z, n, data) "Gamepad " #n,
			std::string_view titles[] =
			{
				column0_title,
				BOOST_PP_REPEAT(GLFW_JOYSTICK_LAST, CHAOS_GENERATE_GAMEPAD_TITLE, ~)
			};
#undef CHAOS_GENERATE_GAMEPAD_TITLE

			std::span<std::string_view> column_titles = { &titles[0], gamepad_count + 1 };

			ImGuiTools::DrawImGuiTable(column0_title, {}, column_titles)([&]()
			{
				fill_content_func();
			});
		};

		// generate a table lambda, that display all inputs of one type
		auto DisplayInputs = [&](auto input_first, auto input_last, char const * column0_title, auto const & display_state_func)
		{
			DisplayTable(column0_title, [&]()
			{
				using input_type = decltype(input_first);

				for (auto input = input_first; input <= input_last; input = input_type(int(input) + 1))
				{
					ImGui::TableNextColumn();
					ImGui::Text("%s", InputTools::GetBasicInputName(input));

					for (size_t i = 0; i < gamepad_count ; ++i)
					{
						PhysicalGamepad const & gamepad = gamepad_manager->GetPhysicalGamepad(i);

						auto input_state = gamepad.GetInputState(input);

						ImGui::TableNextColumn();
						if (input_state.has_value())
						{
							ImVec4 white_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							ImVec4 red_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

							ImVec4 const& color = (input_state->IsActive()) ? red_color : white_color;
							display_state_func(input_state.value(), color);
						}
					}
				}
			});
		};

		// display all properties
		DisplayTable("Properties", [&]()
		{
			auto DisplayProperty = [&](char const * property_name, auto const & fill_property_func)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%s", property_name);

				for (size_t i = 0; i < gamepad_count; ++i)
				{
					PhysicalGamepad const& gamepad = gamepad_manager->GetPhysicalGamepad(i);

					ImGui::TableNextColumn();
					fill_property_func(gamepad);
				}
			};

			DisplayProperty("Allocated", [](PhysicalGamepad const & gamepad)
			{
				ImGui::Text("%s", gamepad.IsAllocated() ? "true" : "false");
			});

			DisplayProperty("Button count", [](PhysicalGamepad const & gamepad)
			{
				GamepadState const* gamepad_state = gamepad.GetGamepadState();

				ImGui::Text("%d", gamepad_state->GetButtonCount());
			});

			DisplayProperty("Axis count", [](PhysicalGamepad const & gamepad)
			{
				GamepadState const* gamepad_state = gamepad.GetGamepadState();

				ImGui::Text("%d", gamepad_state->GetAxisCount());
			});

		});

		ImGui::Dummy({ 0.0f, 20.0f });

		// display Keys
		DisplayInputs(Key::GamepadFirst, Key::GamepadLast, "Buttons", [&](KeyState const & input_state, ImVec4 const & color)
		{
			ImGui::TextColored(color, "%s", input_state.GetValue() ? "pressed" : "released");
		});

		ImGui::Dummy({ 0.0f, 20.0f });

		// display Input1D
		DisplayInputs(Input1D::GamepadFirst, Input1D::GamepadLast, "Axes", [&](Input1DState const & input_state, ImVec4 const& color)
		{
			ImGui::TextColored(color, "%f", input_state.GetValue());
		});

		ImGui::Dummy({ 0.0f, 20.0f });

		// display Input2D
		DisplayInputs(Input2D::GamepadFirst, Input2D::GamepadLast, "Sticks", [&](Input2DState const & input_state, ImVec4 const& color)
		{
			glm::vec2 value = input_state.GetValue();

			ImGui::TextColored(color, "%+f", value.x);
			ImGui::TextColored(color, "%+f", value.y);
		});
	}

}; // namespace chaos