#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
#if _WIN32 || _WIN64

	KeyboardButton KeyboardLayoutConversion::QwertyVKToGLFWKeycode(unsigned int vk)
	{
		if (ScancodeInformation const* scancode_info = KeyboardLayout::GetKnownLayout(KeyboardLayoutType::QWERTY).GetInformationFromVK(vk))
			return QwertyScancodeToGLFWKeycode(scancode_info->scancode);
		return KeyboardButton::UNKNOWN;
	}

	unsigned int KeyboardLayoutConversion::QwertyGLFWKeycodeToVK(KeyboardButton keycode)
	{
		if (unsigned int scancode = QwertyGLFWKeycodeToScancode(keycode))
			if (ScancodeInformation const* scancode_info = KeyboardLayout::GetKnownLayout(KeyboardLayoutType::QWERTY).GetInformationFromScancode(scancode))
				return scancode_info->vk;
		return 0;
	}

	unsigned int KeyboardLayoutConversion::QwertyGLFWKeycodeToScancode(KeyboardButton keycode)
	{
		for (GLFWKeyScancodePair const& scancode_pair : GetQwertyGLFWKeyScancodeTable())
			if (scancode_pair.keycode == keycode)
				return scancode_pair.scancode;
		return 0;
	}

	KeyboardButton KeyboardLayoutConversion::QwertyScancodeToGLFWKeycode(unsigned int scancode)
	{
		for (GLFWKeyScancodePair const& scancode_pair : GetQwertyGLFWKeyScancodeTable())
			if (scancode_pair.scancode == scancode)
				return scancode_pair.keycode;
		return KeyboardButton::UNKNOWN;
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
					info.keycode = KeyboardButton(keycode);
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

	KeyboardButton KeyboardLayoutConversion::ConvertKeyboardButton(KeyboardButton button, KeyboardLayoutType layout)
	{
#if _WIN32 || _WIN64

		if (layout == KeyboardLayoutType::QWERTY)
		{
			if (unsigned int scancode = QwertyGLFWKeycodeToScancode(button))
				if (ScancodeInformation const* scancode_info = KeyboardLayout::GetCurrentLayout().GetInformationFromScancode(scancode))
					return QwertyVKToGLFWKeycode(scancode_info->vk);
		}
		else
		{
			if (unsigned int vk = QwertyGLFWKeycodeToVK(button))
				if (ScancodeInformation const* scancode_info = KeyboardLayout::GetKnownLayout(layout).GetInformationFromVK(vk))
					return QwertyScancodeToGLFWKeycode(scancode_info->scancode);
		}

#endif // #if _WIN32 || _WIN64

		return button;
	}

	Key KeyboardLayoutConversion::ConvertKey(Key key, KeyboardLayoutType layout)
	{
#if _WIN32 || _WIN64

		if (key.IsKeyboardKey())
			return Key(ConvertKeyboardButton(key.GetKeyboardButton(), layout));

#endif
		return key; // valid for non-keyboard keys, non-windows code, and when conversion has failed
	}

}; // namespace chaos
