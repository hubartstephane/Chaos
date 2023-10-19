#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	// this is the table obtained by running GetCurrentVirtualKeyScancodeTable(...) with an azerty keyboard
	// the strings are modified by hand so that we can easily search in the table
	// there are some values that have been inserted by hand, due to manual search
	std::vector<VirtualKeyScancodePair> AzertyVirtualKeyScancodeTable =
	{
	  {0x1b, 0x1, "Esc"},
	  {0x31, 0x2, "&"},
	  {0x32, 0x3, "é"},
	  {0x33, 0x4, "\""},
	  {0x34, 0x5, "'"},
	  {0x35, 0x6, "("},
	  {0x36, 0x7, "-"},
	  {0x37, 0x8, "è"},
	  {0x38, 0x9, "_"},
	  {0x39, 0xa, "ç"},
	  {0x30, 0xb, "à"},
	  {0xdb, 0xc, ")"},
	  {0xbb, 0xd, "="},
	  {0x8, 0xe, "Backspace"},
	  {0x9, 0xf, "Tab"},
	  {0x41, 0x10, "A"},
	  {0x5a, 0x11, "Z"},
	  {0x45, 0x12, "E"},
	  {0x52, 0x13, "R"},
	  {0x54, 0x14, "T"},
	  {0x59, 0x15, "Y"},
	  {0x55, 0x16, "U"},
	  {0x49, 0x17, "I"},
	  {0x4f, 0x18, "O"},
	  {0x50, 0x19, "P"},
	  {0xdd, 0x1a, "^"},
	  {0xba, 0x1b, "$"},
	  {0xd, 0x1c, "Enter"},
	  {0x11, 0x1d, "Ctrl"},
	  {0x51, 0x1e, "Q"},
	  {0x53, 0x1f, "S"},
	  {0x44, 0x20, "D"},
	  {0x46, 0x21, "F"},
	  {0x47, 0x22, "G"},
	  {0x48, 0x23, "H"},
	  {0x4a, 0x24, "J"},
	  {0x4b, 0x25, "K"},
	  {0x4c, 0x26, "L"},
	  {0x4d, 0x27, "M"},
	  {0xc0, 0x28, "ù"},
	  {0xde, 0x29, "²"},
	  {0x10, 0x2a, "Shift"},
	  {0xdc, 0x2b, "*"},
	  {0x57, 0x2c, "W"},
	  {0x58, 0x2d, "X"},
	  {0x43, 0x2e, "C"},
	  {0x56, 0x2f, "V"},
	  {0x42, 0x30, "B"},
	  {0x4e, 0x31, "N"},
	  {0xbc, 0x32, ","},
	  {0xbe, 0x33, ";"},
	  {0xbf, 0x34, ":"},
	  {0xdf, 0x35, "!"},
	  {0x10, 0x36, "Right Shift"},
	  {0x6a, 0x37, "Num *"},
	  {0x12, 0x38, "Alt"},
	  {0x20, 0x39, "Space"},
	  {0x14, 0x3a, "Caps Lock"},
	  {0x70, 0x3b, "F1"},
	  {0x71, 0x3c, "F2"},
	  {0x72, 0x3d, "F3"},
	  {0x73, 0x3e, "F4"},
	  {0x74, 0x3f, "F5"},
	  {0x75, 0x40, "F6"},
	  {0x76, 0x41, "F7"},
	  {0x77, 0x42, "F8"},
	  {0x78, 0x43, "F9"},
	  {0x79, 0x44, "F10"},
	  {0x90, 0x45, "Pause"},
	  {0x91, 0x46, "Scroll Lock"},
	  {0x24, 0x47, "Num 7"},
	  {0x26, 0x48, "Num 8"},
	  {0x21, 0x49, "Num 9"},
	  {0x6d, 0x4a, "Num -"},
	  {0x25, 0x4b, "Num 4"},
	  {0xc, 0x4c, "Num 5"},
	  {0x27, 0x4d, "Num 6"},
	  {0x6b, 0x4e, "Num +"},
	  {0x23, 0x4f, "Num 1"},
	  {0x28, 0x50, "Num 2"},
	  {0x22, 0x51, "Num 3"},
	  {0x2d, 0x52, "Num 0"},
	  {0x2e, 0x53, "Num Del"},
	  {0x2c, 0x54, ""},
	  {0xe2, 0x56, "<"},
	  {0x7a, 0x57, "F11"},
	  {0x7b, 0x58, "F12"},
	  {0xc, 0x59, ""},
	  {0xee, 0x5a, ""},
	  {0xf1, 0x5b, ""},
	  {0xea, 0x5c, ""},
	  {0xf9, 0x5d, ""},
	  {0xf5, 0x5e, ""},
	  {0xf3, 0x5f, ""},
	  {0xfb, 0x62, ""},
	  {0x2f, 0x63, ""},
	  {0x7c, 0x64, ""},
	  {0x7d, 0x65, ""},
	  {0x7e, 0x66, ""},
	  {0x7f, 0x67, ""},
	  {0x80, 0x68, ""},
	  {0x81, 0x69, ""},
	  {0x82, 0x6a, ""},
	  {0x83, 0x6b, ""},
	  {0x84, 0x6c, ""},
	  {0x85, 0x6d, ""},
	  {0x86, 0x6e, ""},
	  {0xed, 0x6f, ""},
	  {0xe9, 0x71, ""},
	  {0xc1, 0x73, ""},
	  {0x87, 0x76, ""},
	  {0xeb, 0x7b, ""},
	  {0x9, 0x7c, "	"},
	  {0xc2, 0x7e, ""},

	  // manually inserted values
	  {0x26, 0x148, "Up"},
	  {0x25, 0x14b, "Left"},
	  {0x28, 0x150, "Down"},
	  {0x27, 0x14d, "Right"},

	  {0x6a, 0x137, "Print"},
	  {0x24, 0x147, "Home"},
	  {0x21, 0x149, "Page Up"},
	  {0x2e, 0x153, "Delete"},
	  {0x22, 0x151, "Page Down"},
	  {0x90, 0x145, "Num Lock"},
	  {0x2d, 0x152, "Insert"},
	  {0x23, 0x14f, "End"},

	  {0x12, 0x138, "Right Alt"},
	  {0x11, 0x11d, "Right Ctrl"},

	  {0xf1, 0x15b, "Windows"},
	  {0xea, 0x15c, "Right Windows"},

	  {0x0c, 0x11c, "Num Enter"},
	  {0xdf, 0x135, "Num /" }
	};

	// this is the table obtained by running GetCurrentVirtualKeyScancodeTable(...) with an qwerty keyboard
	// the strings are modified by hand so that we can easily search in the table
	// there are some values that have been inserted by hand, due to manual search
	std::vector<VirtualKeyScancodePair> QwertyVirtualKeyScancodeTable =
	{
	  {0x1b, 0x1, "Esc"},
	  {0x31, 0x2, "1"},
	  {0x32, 0x3, "2"},
	  {0x33, 0x4, "3"},
	  {0x34, 0x5, "4"},
	  {0x35, 0x6, "5"},
	  {0x36, 0x7, "6"},
	  {0x37, 0x8, "7"},
	  {0x38, 0x9, "8"},
	  {0x39, 0xa, "9"},
	  {0x30, 0xb, "0"},
	  {0xbd, 0xc, "-"},
	  {0xbb, 0xd, "="},
	  {0x8, 0xe, "Backspace"},
	  {0x9, 0xf, "Tab"},
	  {0x51, 0x10, "Q"},
	  {0x57, 0x11, "W"},
	  {0x45, 0x12, "E"},
	  {0x52, 0x13, "R"},
	  {0x54, 0x14, "T"},
	  {0x59, 0x15, "Y"},
	  {0x55, 0x16, "U"},
	  {0x49, 0x17, "I"},
	  {0x4f, 0x18, "O"},
	  {0x50, 0x19, "P"},
	  {0xdb, 0x1a, "["},
	  {0xdd, 0x1b, "]"},
	  {0xd, 0x1c, "Enter"},
	  {0x11, 0x1d, "Ctrl"},
	  {0x41, 0x1e, "A"},
	  {0x53, 0x1f, "S"},
	  {0x44, 0x20, "D"},
	  {0x46, 0x21, "F"},
	  {0x47, 0x22, "G"},
	  {0x48, 0x23, "H"},
	  {0x4a, 0x24, "J"},
	  {0x4b, 0x25, "K"},
	  {0x4c, 0x26, "L"},
	  {0xba, 0x27, ";"},
	  {0xde, 0x28, "'"},
	  {0xc0, 0x29, "`"},
	  {0x10, 0x2a, "Shift"},
	  {0xdc, 0x2b, "\\"},
	  {0x5a, 0x2c, "Z"},
	  {0x58, 0x2d, "X"},
	  {0x43, 0x2e, "C"},
	  {0x56, 0x2f, "V"},
	  {0x42, 0x30, "B"},
	  {0x4e, 0x31, "N"},
	  {0x4d, 0x32, "M"},
	  {0xbc, 0x33, ","},
	  {0xbe, 0x34, "."},
	  {0xbf, 0x35, "/"},
	  {0x10, 0x36, "Right Shift"},
	  {0x6a, 0x37, "Num *"},
	  {0x12, 0x38, "Alt"},
	  {0x20, 0x39, "Space"},
	  {0x14, 0x3a, "Caps Lock"},
	  {0x70, 0x3b, "F1"},
	  {0x71, 0x3c, "F2"},
	  {0x72, 0x3d, "F3"},
	  {0x73, 0x3e, "F4"},
	  {0x74, 0x3f, "F5"},
	  {0x75, 0x40, "F6"},
	  {0x76, 0x41, "F7"},
	  {0x77, 0x42, "F8"},
	  {0x78, 0x43, "F9"},
	  {0x79, 0x44, "F10"},
	  {0x90, 0x45, "Pause"},
	  {0x91, 0x46, "Scroll Lock"},
	  {0x24, 0x47, "Num 7"},
	  {0x26, 0x48, "Num 8"},
	  {0x21, 0x49, "Num 9"},
	  {0x6d, 0x4a, "Num -"},
	  {0x25, 0x4b, "Num 4"},
	  {0xc, 0x4c, "Num 5"},
	  {0x27, 0x4d, "Num 6"},
	  {0x6b, 0x4e, "Num +"},
	  {0x23, 0x4f, "Num 1"},
	  {0x28, 0x50, "Num 2"},
	  {0x22, 0x51, "Num 3"},
	  {0x2d, 0x52, "Num 0"},
	  {0x2e, 0x53, "Num Del"},
	  {0x2c, 0x54, "Sys Req"},
	  {0xe2, 0x56, "\\"},
	  {0x7a, 0x57, "F11"},
	  {0x7b, 0x58, "F12"},
	  {0xc, 0x59, ""},
	  {0xee, 0x5a, ""},
	  {0xf1, 0x5b, ""},
	  {0xea, 0x5c, ""},
	  {0xf9, 0x5d, ""},
	  {0xf5, 0x5e, ""},
	  {0xf3, 0x5f, ""},
	  {0xfb, 0x62, ""},
	  {0x2f, 0x63, ""},
	  {0x7c, 0x64, ""},
	  {0x7d, 0x65, ""},
	  {0x7e, 0x66, ""},
	  {0x7f, 0x67, ""},
	  {0x80, 0x68, ""},
	  {0x81, 0x69, ""},
	  {0x82, 0x6a, ""},
	  {0x83, 0x6b, ""},
	  {0x84, 0x6c, ""},
	  {0x85, 0x6d, ""},
	  {0x86, 0x6e, ""},
	  {0xed, 0x6f, ""},
	  {0xe9, 0x71, ""},
	  {0xc1, 0x73, ""},
	  {0x87, 0x76, ""},
	  {0xeb, 0x7b, ""},
	  {0x9, 0x7c, "F13"},
	  {0xc2, 0x7e, "F15"},

	  // manually inserted values
	  {0x26, 0x148, "Up"},
	  {0x25, 0x14b, "Left"},
	  {0x28, 0x150, "Down"},
	  {0x27, 0x14d, "Right"},

	  {0x6a, 0x137, "Print"},
	  {0x24, 0x147, "Home"},
	  {0x21, 0x149, "Page Up"},
	  {0x2e, 0x153, "Delete"},
	  {0x22, 0x151, "Page Down"},
	  {0x90, 0x145, "Num Lock"},
	  {0x2d, 0x152, "Insert"},
	  {0x23, 0x14f, "End"},

	  {0x12, 0x138, "Right Alt"},
	  {0x11, 0x11d, "Right Ctrl"},

	  {0xf1, 0x15b, "Windows"},
	  {0xea, 0x15c, "Right Windows"},

	  { 0x0c, 0x11c, "Num Enter"},
	  { 0xdf, 0x135, "Num /" }
	};

	// here is the function that is used to get the tables above
#if 0
	bool PrintVirtualKeyScancodeTableToFile(char const* filename, char const* table_name)
	{
		FILE* f = NULL;
		fopen_s(&f, filename, "w");
		if (f != NULL)
		{
			std::vector<VirtualKeyScancodePair> conversion_table = KeyboardLayoutConversion::GetCurrentVirtualKeyScancodeTable();

			fprintf(f, "std::vector<VirtualKeyScancodePair> %s = \n{\n", table_name);
			size_t count = 0;
			for (VirtualKeyScancodePair const & info : conversion_table)
			{
				if (count > 0)
					fprintf(f, ",\n  {0x%x, 0x%x, \"%s\"}", info.vk, info.scancode, info.name.c_str());
				else
					fprintf(f, "  {0x%x, 0x%x, \"%s\"}", info.vk, info.scancode, info.name.c_str());
				++count;
			}
			fprintf(f, "\n};\n\n");

			fclose(f);
			return true;
		}
		return false;
	}

#endif

	// GLFWKeyScancodePair: describe a relation between a GLFW keycode and a scancode. This is only valid for QWERTY while it is HARD-CODED inside GLFW
	class GLFWKeyScancodePair
	{
	public:

		/** the GLFW keycode */
		int keycode = 0;
		/** the corresponding scancode */
		unsigned int scancode = 0;
	};

	// the table from GLFW's HARD-CODED values that helps conversion from glfw_keycode <---> scancode
	// the following table is necessary while GLFW has a function glfwGetKeyScancode(...) but no reverse function
	std::vector<GLFWKeyScancodePair> QwertyGLFWKeyScancodeTable =
	{
	  {32, 0x39},
	  {39, 0x28},
	  {44, 0x33},
	  {45, 0xc},
	  {46, 0x34},
	  {47, 0x35},
	  {48, 0xb},
	  {49, 0x2},
	  {50, 0x3},
	  {51, 0x4},
	  {52, 0x5},
	  {53, 0x6},
	  {54, 0x7},
	  {55, 0x8},
	  {56, 0x9},
	  {57, 0xa},
	  {59, 0x27},
	  {61, 0xd},
	  {65, 0x1e},
	  {66, 0x30},
	  {67, 0x2e},
	  {68, 0x20},
	  {69, 0x12},
	  {70, 0x21},
	  {71, 0x22},
	  {72, 0x23},
	  {73, 0x17},
	  {74, 0x24},
	  {75, 0x25},
	  {76, 0x26},
	  {77, 0x32},
	  {78, 0x31},
	  {79, 0x18},
	  {80, 0x19},
	  {81, 0x10},
	  {82, 0x13},
	  {83, 0x1f},
	  {84, 0x14},
	  {85, 0x16},
	  {86, 0x2f},
	  {87, 0x11},
	  {88, 0x2d},
	  {89, 0x15},
	  {90, 0x2c},
	  {91, 0x1a},
	  {92, 0x2b},
	  {93, 0x1b},
	  {96, 0x29},
	  {162, 0x56},
	  {256, 0x1},
	  {257, 0x1c},
	  {258, 0xf},
	  {259, 0xe},
	  {260, 0x152},
	  {261, 0x153},
	  {262, 0x14d},
	  {263, 0x14b},
	  {264, 0x150},
	  {265, 0x148},
	  {266, 0x149},
	  {267, 0x151},
	  {268, 0x147},
	  {269, 0x14f},
	  {280, 0x3a},
	  {281, 0x46},
	  {282, 0x145},
	  {283, 0x137},
	  {284, 0x146},
	  {290, 0x3b},
	  {291, 0x3c},
	  {292, 0x3d},
	  {293, 0x3e},
	  {294, 0x3f},
	  {295, 0x40},
	  {296, 0x41},
	  {297, 0x42},
	  {298, 0x43},
	  {299, 0x44},
	  {300, 0x57},
	  {301, 0x58},
	  {302, 0x64},
	  {303, 0x65},
	  {304, 0x66},
	  {305, 0x67},
	  {306, 0x68},
	  {307, 0x69},
	  {308, 0x6a},
	  {309, 0x6b},
	  {310, 0x6c},
	  {311, 0x6d},
	  {312, 0x6e},
	  {313, 0x76},
	  {320, 0x52},
	  {321, 0x4f},
	  {322, 0x50},
	  {323, 0x51},
	  {324, 0x4b},
	  {325, 0x4c},
	  {326, 0x4d},
	  {327, 0x47},
	  {328, 0x48},
	  {329, 0x49},
	  {330, 0x53},
	  {331, 0x135},
	  {332, 0x37},
	  {333, 0x4a},
	  {334, 0x4e},
	  {335, 0x11c},
	  {336, 0x59},
	  {340, 0x2a},
	  {341, 0x1d},
	  {342, 0x38},
	  {343, 0x15b},
	  {344, 0x36},
	  {345, 0x11d},
	  {346, 0x138},
	  {347, 0x15c}
	};

#if 0

	// the function that generated the table above
	std::vector<GLFWKeyScancodePair> GetGLFWKeyScancodeTable()
	{
		// glfwInit();

		std::vector<GLFWKeyScancodePair> result;

		for (int keycode = 0; keycode < GLFW_KEY_LAST; ++keycode)
		{
			int scancode = glfwGetKeyScancode(keycode);
			if (scancode > 0)
			{
				GLFWKeyScancodePair info;
				info.keycode  = keycode;
				info.scancode = (unsigned int)scancode; // while WINDOWS scancodes are UINT, use the same type here
				result.push_back(info);
			}
		}

		// glfwTerminate();

		return result;
	}

	// the function that created the source code for the table above
	bool PrintGLFWKeyScancodeTableToFile(char const* filename, char const* table_name)
	{
		FILE* f = NULL;
		fopen_s(&f, filename, "w");
		if (f != NULL)
		{
			std::vector<GLFWKeyScancodePair> conversion_table = GetGLFWKeyScancodeTable();

			fprintf(f, "std::vector<GLFWKeyScancodePair> %s = \n{\n", table_name);
			size_t count = 0;
			for (GLFWKeyScancodePair const& info : conversion_table)
			{
				if (count > 0)
					fprintf(f, ",\n  {%d, 0x%x}", info.keycode, info.scancode);
				else
					fprintf(f, "  {%d, 0x%x}", info.keycode, info.scancode);
				++count;
			}
			fprintf(f, "\n};\n\n");

			fclose(f);
			return true;
		}
		return false;
	}

#endif

#if _WIN32 || _WIN64

	std::vector<VirtualKeyScancodePair> const* KeyboardLayoutConversion::GetVirtualKeyScancodeTable(KeyboardLayout layout)
	{
		switch (layout)
		{
		case KeyboardLayout::AZERTY: return &AzertyVirtualKeyScancodeTable;
		case KeyboardLayout::QWERTY: return &QwertyVirtualKeyScancodeTable;
		default:assert(0); return nullptr;
		}
	}

	VirtualKeyScancodePair const* KeyboardLayoutConversion::GetVirtualKeyScancodeFromScancode(unsigned int scancode, std::vector<VirtualKeyScancodePair> const& table)
	{
		for (VirtualKeyScancodePair const& entry : table)
			if (entry.scancode == scancode)
				return &entry;
		return nullptr;
	}

	VirtualKeyScancodePair const* KeyboardLayoutConversion::GetVirtualKeyScancodeFromVK(unsigned int vk, std::vector<VirtualKeyScancodePair> const& table)
	{
		for (VirtualKeyScancodePair const& entry : table)
			if (entry.vk == vk)
				return &entry;
		return nullptr;
	}

	VirtualKeyScancodePair const* KeyboardLayoutConversion::GetVirtualKeyScancodeFromScancode(unsigned int scancode, KeyboardLayout layout)
	{
		if (std::vector<VirtualKeyScancodePair> const* table = GetVirtualKeyScancodeTable(layout))
			return GetVirtualKeyScancodeFromScancode(scancode, *table);
		return nullptr;
	}

	VirtualKeyScancodePair const* KeyboardLayoutConversion::GetVirtualKeyScancodeFromVK(unsigned int vk, KeyboardLayout layout)
	{
		if (std::vector<VirtualKeyScancodePair> const* table = GetVirtualKeyScancodeTable(layout))
			return GetVirtualKeyScancodeFromVK(vk, *table);
		return nullptr;
	}

	std::vector<VirtualKeyScancodePair> KeyboardLayoutConversion::GetCurrentVirtualKeyScancodeTable()
	{
		std::vector<VirtualKeyScancodePair> result;

		for (unsigned int scancode = 0; scancode < 0xFF; ++scancode)
		{
			unsigned int vk = ::MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
			if (vk != 0)
				result.push_back({ vk, scancode, ScancodeToName(scancode) });
		}
		return result;
	}

	std::string KeyboardLayoutConversion::ScancodeToName(unsigned int scancode)
	{
		char name[256];
		if (::GetKeyNameTextA(scancode << 16, name, sizeof(name)))
			return name;
		return {};
	}

	unsigned int KeyboardLayoutConversion::ConvertVirtualKeyToCurrentLayout(unsigned int vk, KeyboardLayout layout, VirtualKeyScancodePair const** result_pair = nullptr)
	{
		if (VirtualKeyScancodePair const* info = GetVirtualKeyScancodeFromVK(vk, layout))
		{
			if (result_pair != nullptr)
				*result_pair = info;

			return ::MapVirtualKey(info->scancode, MAPVK_VSC_TO_VK);
		}
		// error
		if (result_pair != nullptr)
			*result_pair = nullptr;
		return -1;
	}

	int KeyboardLayoutConversion::ScancodeToGLFWKey(int scancode)
	{
		for (GLFWKeyScancodePair const & info : QwertyGLFWKeyScancodeTable)
			if (info.scancode == scancode)
				return info.keycode;
		return -1; // failure
	}

#endif


}; // namespace chaos
