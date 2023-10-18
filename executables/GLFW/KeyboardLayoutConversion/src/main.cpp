#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

public:

	virtual void OnDrawWindowImGuiMenu() override
	{
		// Window::OnDrawImGuiMenu();
		// do not call super so that the application items are not inserted here
	}

	virtual void OnDrawWindowImGuiContent()
	{



		ImGuiDrawableInterface::FullscreenWindow("conversion", false, [this]()
		{
			auto ImGui_DisplayConversion = [](int src_vk, chaos::KeyboardLayout src_layout)
			{
				chaos::VirtualKeyScancodePair const* src_entry = nullptr;

				int new_vk = chaos::KeyboardLayoutConversion::ConvertVirtualKey(src_vk, src_layout, &src_entry);

				int new_scancode = ::MapVirtualKey(new_vk, MAPVK_VK_TO_VSC);
				std::string name = chaos::KeyboardLayoutConversion::ScancodeToName(new_scancode);

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

			int vk_1 = ::VkKeyScan('a') & 0xFF;
			int vk_2 = ::VkKeyScan('A') & 0xFF;
			int vk_3 = ::VkKeyScan('*') & 0xFF;
			int vk_4 = ::VkKeyScan('µ') & 0xFF;

			int scancode_1 = ::MapVirtualKey(vk_1, MAPVK_VK_TO_VSC);
			int scancode_2 = ::MapVirtualKey(vk_2, MAPVK_VK_TO_VSC);
			int scancode_3 = ::MapVirtualKey(vk_3, MAPVK_VK_TO_VSC);
			int scancode_4 = ::MapVirtualKey(vk_4, MAPVK_VK_TO_VSC);

			std::string name1 = chaos::KeyboardLayoutConversion::ScancodeToName(scancode_1);
			std::string name2 = chaos::KeyboardLayoutConversion::ScancodeToName(scancode_2);
			std::string name3 = chaos::KeyboardLayoutConversion::ScancodeToName(scancode_3);
			std::string name4 = chaos::KeyboardLayoutConversion::ScancodeToName(scancode_4);

			int vk1 = ::VkKeyScan('*') & 0xFF;
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(vk1, chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();



			int vk2 = ::VkKeyScan('$') & 0xFF;
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(vk2, chaos::KeyboardLayout::QWERTY);
			ImGui::Separator();
		});
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}