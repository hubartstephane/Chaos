#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

public:

	virtual void OnDrawWindowImGuiMenu() override
	{
		 chaos::Window::OnDrawWindowImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		if (event.action == GLFW_PRESS)
		{
			last_scancode = event.scancode;
			last_button  = event.button;
			last_key_pressed = chaos::KeyboardLayout::GetKnownLayout(chaos::KeyboardLayoutType::AZERTY).GetInformationFromScancode(event.scancode);
		}

		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual void OnDrawWindowImGuiContent()
	{

		ImGuiDrawableInterface::FullscreenWindow("conversion", false, [this]()
		{
			ImGui::Text("LAST BUTTON (%s)", chaos::EnumToString(last_button));

			ImGui::Dummy({ 0.0f, 20.0f });
			ImGui::Separator();

			auto ImGui_DisplayConversion = [](chaos::Key src)
			{
				for (chaos::KeyboardLayoutType src_layout : {chaos::KeyboardLayoutType::QWERTY, chaos::KeyboardLayoutType::AZERTY, chaos::KeyboardLayoutType::CURRENT})
				{
					char const* src_layout_name = EnumToString(src_layout);
					chaos::Key dst = chaos::KeyboardLayoutConversion::ConvertKey(src, src_layout, chaos::KeyboardLayoutType::CURRENT);

					ImVec4 color = (src == dst) ?
						ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }:
						ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f };

					ImGui::TextColored(color, "%s (%s) -> %s (current)", src.GetName(), src_layout_name, dst.GetName());
				}
				ImGui::Separator();
			};

			ImGui_DisplayConversion(chaos::KeyboardButton::Q);
			ImGui_DisplayConversion(chaos::KeyboardButton::W);
			ImGui_DisplayConversion(chaos::KeyboardButton::E);
			ImGui_DisplayConversion(chaos::KeyboardButton::R);
			ImGui_DisplayConversion(chaos::KeyboardButton::T);
			ImGui_DisplayConversion(chaos::KeyboardButton::Y);
			ImGui_DisplayConversion(chaos::KeyboardButton::A);
			ImGui_DisplayConversion(chaos::KeyboardButton::Z);
			ImGui_DisplayConversion(chaos::KeyboardButton::COMMA);
			ImGui_DisplayConversion(chaos::KeyboardButton::SPACE);
			ImGui_DisplayConversion(chaos::KeyboardButton::NUM_1);
			ImGui_DisplayConversion(chaos::KeyboardButton::KP_1);
			ImGui_DisplayConversion(chaos::KeyboardButton::UP);
			ImGui_DisplayConversion(chaos::KeyboardButton::PAGE_UP);

			ImGui_DisplayConversion(chaos::Key("SEMICOLON"));
			ImGui_DisplayConversion(chaos::Key("A"));
			ImGui_DisplayConversion(chaos::Key("Q"));
			ImGui_DisplayConversion(chaos::Key("MOUSE_BUTTON_1"));
		});

	}

protected:

	chaos::ScancodeInformation const* last_key_pressed = nullptr;

	int last_scancode = -1;

	chaos::KeyboardButton last_button = chaos::KeyboardButton::UNKNOWN;
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.width = 800;
	create_params.height = 800;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}