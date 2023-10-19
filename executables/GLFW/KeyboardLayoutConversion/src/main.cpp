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
			last_key_pressed = chaos::KeyboardLayoutInformation::GetKeyboardInformation(chaos::KeyboardLayout::AZERTY).GetInformationFromScancode(event.scancode);
		}

		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual void OnDrawWindowImGuiContent()
	{
		ImGuiDrawableInterface::FullscreenWindow("conversion", false, [this]()
		{
			auto ImGui_DisplayConversion = [](int src_vk, chaos::KeyboardLayout src_layout)
			{
				chaos::ScancodeInformation const* src_entry = nullptr;

				int new_vk = chaos::KeyboardLayoutConversion::ConvertVirtualKeyToCurrentLayout(src_vk, src_layout, &src_entry);

				int new_scancode = ::MapVirtualKey(new_vk, MAPVK_VK_TO_VSC);
				std::string name = chaos::KeyboardLayoutInformation::ScancodeToName(new_scancode);

				char const* layout_name = (src_layout == chaos::KeyboardLayout::AZERTY) ? "azerty" : "qwerty";

				char const * src_entry_name = (src_entry != nullptr) ? src_entry->name.c_str() : "unknown";

				ImGui::Text("%s (%s) -> %s (current)", src_entry_name, layout_name, name.c_str());
			};

			auto xxx = glfwGetKeyScancode(GLFW_KEY_A);

			auto ppp = ::MapVirtualKey('A', MAPVK_VK_TO_VSC);

			ImGui_DisplayConversion('A', chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('A', chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('Q', chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('Q', chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('M', chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('M', chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SPACE, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(VK_SPACE, chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SHIFT, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(VK_SHIFT, chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('0', chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('0', chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			int vk1 = ::VkKeyScan('*') & 0xFF;
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();

			int vk2 = ::VkKeyScan('$') & 0xFF;
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayout::QWERTY);
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



int main(int argc, char ** argv, char ** env)
{

	//chaos::KeyboardLayoutInformation information = chaos::KeyboardLayoutInformation::Collect();
	//chaos::DumpKeyboardLayoutInformationToFile("azerty2.txt", "AzertyKeyboardLayoutInformation", information);
	//chaos::DumpKeyboardLayoutInformationToFile("qwerty2.txt", "QwertyKeyboardLayoutInformation", information);

	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}