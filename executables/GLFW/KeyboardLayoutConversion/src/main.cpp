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
			last_key_pressed = chaos::KeyboardLayout::GetKeyboardInformation(chaos::KeyboardLayoutType::AZERTY).GetInformationFromScancode(event.scancode);
		}

		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual void OnDrawWindowImGuiContent()
	{
		ImGuiDrawableInterface::FullscreenWindow("conversion", false, [this]()
		{
			auto ImGui_DisplayConversion = [](int src_vk, chaos::KeyboardLayoutType src_layout)
			{
				chaos::ScancodeInformation const* src_entry = nullptr;

				int new_vk = chaos::KeyboardLayoutConversion::ConvertVirtualKeyToCurrentLayout(src_vk, src_layout, &src_entry);

				int new_scancode = ::MapVirtualKey(new_vk, MAPVK_VK_TO_VSC);
				std::string name = chaos::KeyboardLayout::ScancodeToName(new_scancode);

				char const* layout_name = (src_layout == chaos::KeyboardLayoutType::AZERTY) ? "azerty" : "qwerty";

				char const * src_entry_name = (src_entry != nullptr) ? src_entry->name.c_str() : "unknown";

				ImGui::Text("%s (%s) -> %s (current)", src_entry_name, layout_name, name.c_str());
			};

			auto xxx = glfwGetKeyScancode(GLFW_KEY_A);

			auto ppp = ::MapVirtualKey('A', MAPVK_VK_TO_VSC);

			ImGui_DisplayConversion('A', chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion('A', chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('Q', chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion('Q', chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('M', chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion('M', chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SPACE, chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion(VK_SPACE, chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SHIFT, chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion(VK_SHIFT, chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('0', chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion('0', chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			int vk1 = ::VkKeyScan('*') & 0xFF;
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			int vk2 = ::VkKeyScan('$') & 0xFF;
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayoutType::AZERTY);
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayoutType::QWERTY);
			ImGui::Separator();

			if (last_key_pressed)
			{
				ImGui::Text("SCANCODE: [0x%x]", last_key_pressed->scancode);
				ImGui::Text("VK:       [0x%x]", last_key_pressed->vk);
				ImGui::Text("NAME:     [%s]", last_key_pressed->name.c_str());
			}
			else if (last_scancode > 0)
			{
				ImVec4 new_color = { 1.0f, 0.0f, 0.0f, 1.0f };
				ImGui::PushStyleColor(ImGuiCol_Text, new_color);
				ImGui::Text("SCANCODE: [0x%x]", last_scancode);
				ImGui::Text("VK:       [0x%x]", ::MapVirtualKeyA(last_scancode & 0xFF, MAPVK_VSC_TO_VK));
				ImGui::Text("NAME:     [UNKNOWN] !!!");
				ImGui::PopStyleColor(1);
			}
		});
	}

protected:

	chaos::ScancodeInformation const* last_key_pressed = nullptr;

	int last_scancode = -1;
};



#if 0
// WM_INPUTLANGCHANGE

int layout_count = ::GetKeyboardLayoutList(0, nullptr);
if (HKL* hkl = new HKL[layout_count])
{
	if (::GetKeyboardLayoutList(layout_count, hkl) == layout_count)
	{
		for (int i = 0; i < layout_count; ++i)
		{
			char buffer[KL_NAMELENGTH];
			::GetKeyboardLayoutName()

		}

	}
	delete[](hkl);
}


std::vector<chaos::ScancodeInformation> table = chaos::KeyboardLayoutConversion::GetCurrentVirtualKeyScancodeTable();

#endif

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

				chaos::KeyboardLayout information = chaos::KeyboardLayout::Collect();

				if (information.GetInformationFromScancode(0x10)->vk == 'A')
					chaos::DumpKeyboardLayoutToFile("azerty2.txt", "AzertyKeyboardLayout", information);
				if (information.GetInformationFromScancode(0x10)->vk == 'Q')
					chaos::DumpKeyboardLayoutToFile("qwerty2.txt", "QwertyKeyboardLayout", information);
			}

		}
		delete[](hkl);
	}
}


unsigned int GetVirtualKeyFromName(char const* name)
{
	for (chaos::KeyboardLayoutType type : {chaos::KeyboardLayoutType::AZERTY, chaos::KeyboardLayoutType::QWERTY})
	{
		chaos::KeyboardLayout const& layout = chaos::KeyboardLayout::GetKeyboardInformation(type);

		for (chaos::ScancodeInformation const& scancode_info : layout.key_informations)
			if (chaos::StringTools::Stricmp(name, scancode_info.name) == 0)
				return scancode_info.vk;
	}
	return 0;
}

unsigned int GetScancodeFromName(char const* name)
{
	if (unsigned int vk = GetVirtualKeyFromName(name))
	{
		return ::MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	}
	return 0;
}

unsigned int GetScancodeFromGLFWKeycode(int glfw_keycode)
{
	if (unsigned int qwerty_scancode = chaos::KeyboardLayoutConversion::QwertyGLFWKeycodeToScancode(glfw_keycode))
	{
		chaos::KeyboardLayout const & layout = chaos::KeyboardLayout::GetKeyboardInformation(chaos::KeyboardLayoutType::QWERTY);

		if (chaos::ScancodeInformation const * qwerty_scancode_info = layout.GetInformationFromScancode(qwerty_scancode))
			return ::MapVirtualKey(qwerty_scancode_info->vk, MAPVK_VK_TO_VSC); // convert with the current layout whatever it is
	}
	return 0;
}

#if 0
chaos::Key KeyFromName(char const* name)
{
	if (unsigned int GetScancodeFromName)


}
#endif

int main(int argc, char ** argv, char ** env)
{

	auto a = GetScancodeFromGLFWKeycode(GLFW_KEY_A);
	auto q = GetScancodeFromGLFWKeycode(GLFW_KEY_Q);
	auto y = GetScancodeFromGLFWKeycode(GLFW_KEY_Y);
	auto z = GetScancodeFromGLFWKeycode(GLFW_KEY_Z);
	auto m = GetScancodeFromGLFWKeycode(GLFW_KEY_M);
	auto comma = GetScancodeFromGLFWKeycode(GLFW_KEY_COMMA);



	//GenerateKeyboardLayoutFiles();
	//chaos::KeyboardLayout information = chaos::KeyboardLayout::Collect();
	//chaos::DumpKeyboardLayoutToFile("qwertz2.txt", "QwertzKeyboardLayout", information);
	//chaos::DumpKeyboardLayoutToFile("dvorak.txt", "DvorakKeyboardLayout", information);


	return 0;

	//chaos::KeyboardLayout information = chaos::KeyboardLayout::Collect();
	//chaos::DumpKeyboardLayoutToFile("azerty2.txt", "AzertyKeyboardLayout", information);
	//chaos::DumpKeyboardLayoutToFile("qwerty2.txt", "QwertyKeyboardLayout", information);

	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;
	 
	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}