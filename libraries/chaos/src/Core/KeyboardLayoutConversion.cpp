#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
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

	unsigned int KeyboardLayoutConversion::ConvertVirtualKeyToCurrentLayout(unsigned int vk, KeyboardLayout layout, ScancodeInformation const** result_pair = nullptr)
	{
#if _WIN32 || _WIN64
		if (ScancodeInformation const* info = KeyboardLayoutInformation::GetKeyboardInformation(layout).GetInformationFromVK(vk))
		{
			if (result_pair != nullptr)
				*result_pair = info;
			return ::MapVirtualKey(info->scancode, MAPVK_VSC_TO_VK);
		}
#endif // #if _WIN32 || _WIN64
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

}; // namespace chaos
