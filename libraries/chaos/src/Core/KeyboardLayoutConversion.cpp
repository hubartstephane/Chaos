#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
#if _WIN32 || _WIN64

	int KeyboardLayoutConversion::QwertyVKToGLFWKeycode(unsigned int vk)
	{
		if (ScancodeInformation const* scancode_info = KeyboardLayout::GetKnownLayout(KeyboardLayoutType::QWERTY).GetInformationFromVK(vk))
			return QwertyScancodeToGLFWKeycode(scancode_info->scancode);
		return 0;
	}

	unsigned int KeyboardLayoutConversion::QwertyGLFWKeycodeToVK(int keycode)
	{
		if (unsigned int scancode = QwertyGLFWKeycodeToScancode(keycode))
			if (ScancodeInformation const* scancode_info = KeyboardLayout::GetKnownLayout(KeyboardLayoutType::QWERTY).GetInformationFromScancode(scancode))
				return scancode_info->vk;
		return 0;
	}

	unsigned int KeyboardLayoutConversion::QwertyGLFWKeycodeToScancode(int keycode)
	{
		for (GLFWKeyScancodePair const& scancode_pair : GetQwertyGLFWKeyScancodeTable())
			if (scancode_pair.keycode == keycode)
				return scancode_pair.scancode;
		return 0;
	}

	int KeyboardLayoutConversion::QwertyScancodeToGLFWKeycode(unsigned int scancode)
	{
		for (GLFWKeyScancodePair const& scancode_pair : GetQwertyGLFWKeyScancodeTable())
			if (scancode_pair.scancode == scancode)
				return scancode_pair.keycode;
		return 0;
	}

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

	unsigned int KeyboardLayoutConversion::ConvertVK(unsigned int vk, KeyboardLayoutType src_layout, KeyboardLayoutType dst_layout)
	{
		assert(src_layout != dst_layout);
		if (ScancodeInformation const* src_scancode = KeyboardLayout::GetKnownLayout(src_layout).GetInformationFromVK(vk))
			if (ScancodeInformation const* dst_scancode = KeyboardLayout::GetKnownLayout(dst_layout).GetInformationFromScancode(src_scancode->scancode))
				return dst_scancode->vk;
		return 0;
	}

#endif // #if _WIN32 || _WIN64

	Key KeyboardLayoutConversion::ConvertKey(Key src, KeyboardLayoutType src_layout, KeyboardLayoutType dst_layout)
	{
		assert(src_layout != dst_layout);

#if _WIN32 || _WIN64

#endif

		return src; // valid for non-keyboard keys, non-windows code, and when conversion has failed
	}




	//
	// GLFW ---> qwerty SCANCODE ---> current layout VK (same SCANCODE) ---> qwerty scancode (same VK) ---> new GLFW
	//

	Key KeyboardLayoutConversion::ConvertToCurrentLayout(Key src)
	{
#if _WIN32 || _WIN64

		if (src.IsKeyboardKey())
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
