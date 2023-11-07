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
			int i = 0;
			if (event.button == chaos::KeyboardButton::A)
				++i;
			if (event.button == chaos::KeyboardButton::Q)
				++i;
			if (event.button == chaos::KeyboardButton::UP)
				++i;
			if (event.button == chaos::KeyboardButton::DOWN)
				++i;
			if (event.button == chaos::KeyboardButton::COMMA)
				++i;
			if (event.button == chaos::KeyboardButton::M)
				++i;
			if (event.button == chaos::KeyboardButton::NUM_0)
				++i;
			if (event.button == chaos::KeyboardButton::KP_0)
				++i;
			if (event.button == chaos::KeyboardButton::Z)
				++i;
			if (event.button == chaos::KeyboardButton::W)
				++i;

			//last_button = event.button;



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
			ImGui::Text("last_button (%s) (%s)", chaos::EnumToString(last_button), chaos::Key(chaos::EnumToString(last_button)).GetName());

#if 0
			if (last_scancode != 0)
			{
				ImGui::Text("last_scancode (0x%03x) -> last_button (0x%02x)", last_scancode, (int)last_button);
			}

			auto ImGui_DisplayConversion = [](chaos::Key src)
			{
				for (chaos::KeyboardLayoutType layout : {chaos::KeyboardLayoutType::QWERTY, chaos::KeyboardLayoutType::AZERTY, chaos::KeyboardLayoutType::CURRENT})
				{
					char const* src_name = EnumToString(layout);
					chaos::Key dst = chaos::KeyboardLayoutConversion::ConvertKey(src, layout);

					ImGui::Text("%s (%s) -> %s (current)", src.GetName(), src_name, dst.GetName());
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
#endif
		});

	}

protected:

	chaos::ScancodeInformation const* last_key_pressed = nullptr;

	int last_scancode = -1;

	chaos::KeyboardButton last_button = chaos::KeyboardButton::UNKNOWN;
};





void GenerateKeyboardLayoutFiles()
{
	int layout_count = ::GetKeyboardLayoutList(0, nullptr);
	if (HKL* hkl = new HKL[layout_count])
	{
		if (::GetKeyboardLayoutList(layout_count, hkl) == layout_count)
		{
			for (int i = 0; i < layout_count; ++i)
			{
				::ActivateKeyboardLayout(hkl[i], KLF_SETFORPROCESS);

				char buffer[KL_NAMELENGTH];
				::GetKeyboardLayoutName(buffer);

				chaos::KeyboardLayout information = chaos::KeyboardLayout::Collect(true);

				char const* filename = nullptr;
				char const* variable_name = nullptr;
				if (information.GetInformationFromScancode(0x15)->vk == 'Z')
				{
					filename = "qwertz.txt";
					variable_name = "QwertzKeyboardLayout";
				}
				else if (information.GetInformationFromScancode(0x10)->vk == 'A')
				{
					filename = "azerty.txt";
					variable_name = "AzertyKeyboardLayout";
				}
				else if (information.GetInformationFromScancode(0x10)->vk == 'Q')
				{
					filename = "qwerty.txt";
					variable_name = "QwertyKeyboardLayout";
				}
				else if (information.GetInformationFromScancode(0x13)->vk == 'P')
				{
					filename = "dvorak.txt";
					variable_name = "DvorakKeyboardLayout";
				}

				if (filename != nullptr && variable_name != nullptr)
				{
					chaos::DumpKeyboardLayoutToFile(filename, variable_name, information);
					chaos::WinTools::ShowFile(filename);
				}
			}
		}
		delete[](hkl);
	}
}



int main(int argc, char ** argv, char ** env)
{
	//GenerateKeyboardLayoutFiles();
	//return 0;

	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;
	 
	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}