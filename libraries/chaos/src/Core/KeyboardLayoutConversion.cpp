#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	std::vector<GLFWKeyScancodePair> const& KeyboardLayoutConversion::GetQwertyGLFWKeyScancodeTable()
	{
		static std::optional<std::vector<GLFWKeyScancodePair>> cached_table;

		if (!cached_table.has_value())
		{
			cached_table = std::vector<GLFWKeyScancodePair>();

			std::vector<GLFWKeyScancodePair> & table = cached_table.value();

			for (int keycode = 0; keycode < GLFW_KEY_LAST; ++keycode)
			{
				int scancode = glfwGetKeyScancode(keycode);
				if (scancode > 0)
				{
					GLFWKeyScancodePair info;
					info.keycode = keycode;
					info.scancode = (unsigned int)scancode; // while WINDOWS scancodes are UINT, use the same type here
					table.push_back(info);
				}
			}
		}
		return cached_table.value();
	}

	//
	// GLFW ---> qwerty SCANCODE ---> current layout VK ---> qwerty scancode ---> new GLFW
	//

	Key KeyboardLayoutConversion::ConvertToCurrentLayout(Key src)
	{
#if _WIN32 || _WIN64

		if (src.GetType() == KeyType::KEYBOARD)
		{
			KeyboardLayout const& current_layout = KeyboardLayout::GetCurrentLayout();
			KeyboardLayout const& qwerty_layout  = KeyboardLayout::GetKnownLayout(KeyboardLayoutType::QWERTY);

			for (GLFWKeyScancodePair const& scancode_pair : GetQwertyGLFWKeyScancodeTable()) // get the corresponding Scancode on QWERTY layout
			{
				if (scancode_pair.keycode == src.GetRawValue())
				{
					// search the possible VK in current keyboard layout with the same position (same scancode)
					for (ScancodeInformation const& current_scancode_info : current_layout.GetScancodeTable())
					{
						if (current_scancode_info.scancode == scancode_pair.scancode)
						{
							// search in QWERTY layout the scancodes for the same VK
							for (ScancodeInformation const& qwerty_scancode_info : qwerty_layout.GetScancodeTable())
							{
								if (qwerty_scancode_info.vk == current_scancode_info.vk)
								{
									for (GLFWKeyScancodePair const& final_scancode_pair : GetQwertyGLFWKeyScancodeTable()) // get the corresponding GLFW keycode on QWERTY layout
									{
										if (final_scancode_pair.scancode == qwerty_scancode_info.scancode)
										{
											return Key(KeyboardButton(final_scancode_pair.keycode));
										}
									}
								}
							}
						}
					}
				}
			}
		}

#endif // #if _WIN32 || _WIN64

		return src; // valid for non-keyboard keys, non-windows code, and when conversion has failed
	}

}; // namespace chaos
