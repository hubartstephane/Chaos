#include "chaos/Chaos.h"

enum class KeyboardLayout
{
	AZERTY,
	QWERTY
};

class KeyboardLayoutConversionEntry
{
public:

	unsigned int vk = 0;
	unsigned int scancode = 0;
	std::string name;
};

std::vector<KeyboardLayoutConversionEntry> AzertyKeyboardLayoutConversionTable =
{
  {0x1b, 0x1, "ECHAP"},
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
  {0x8, 0xe, "RET.ARR"},
  {0x9, 0xf, "TAB"},
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
  {0xdd, 0x1a, "ACCENT CIRCONFLEXE"},
  {0xba, 0x1b, "$"},
  {0xd, 0x1c, "ENTREE"},
  {0x11, 0x1d, "CTRL"},
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
  {0x10, 0x2a, "MAJ"},
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
  {0x10, 0x36, "MAJ DROITE"},
  {0x6a, 0x37, "* (PAVE NUM.)"},
  {0x12, 0x38, "ALT"},
  {0x20, 0x39, "ESPACE"},
  {0x14, 0x3a, "VERR.MAJ"},
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
  {0x91, 0x46, "DEFIL"},
  {0x24, 0x47, "7 (PAVE NUM.)"},
  {0x26, 0x48, "8 (PAVE NUM.)"},
  {0x21, 0x49, "9 (PAVE NUM.)"},
  {0x6d, 0x4a, "- (PAVE NUM.)"},
  {0x25, 0x4b, "4 (PAVE NUM.)"},
  {0xc, 0x4c, "5 (PAVE NUM.)"},
  {0x27, 0x4d, "6 (PAVE NUM.)"},
  {0x6b, 0x4e, "+ (PAVE NUM.)"},
  {0x23, 0x4f, "1 (PAVE NUM.)"},
  {0x28, 0x50, "2 (PAVE NUM.)"},
  {0x22, 0x51, "3 (PAVE NUM.)"},
  {0x2d, 0x52, "0 (PAVE NUM.)"},
  {0x2e, 0x53, ". (PAVE NUM.)"},
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
  {0xc2, 0x7e, ""}
};



std::vector<KeyboardLayoutConversionEntry> QwertyKeyboardLayoutConversionTable =
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
  {0xc2, 0x7e, "F15"}
};

std::vector<KeyboardLayoutConversionEntry> GetKeyboardLayoutConversionTable()
{
	std::vector<KeyboardLayoutConversionEntry> result;

	for (unsigned int scancode = 0; scancode < 0xFF; ++scancode)
	{
		unsigned int vk = ::MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
		if (vk != 0)
		{
			char name[256];
			::GetKeyNameTextA(scancode << 16, name, sizeof(name));
			result.push_back({ vk, scancode, name });
		}
	}
	return result;
 }

bool PrintScancodeVKConversionToFile(char const* filename, char const* table_name)
{
	auto PrintToFile = [](FILE *f, char const * table_name, std::vector<KeyboardLayoutConversionEntry> const & conversion_table)
	{
		fprintf(f, "std::vector<KeyboardLayoutConversionEntry> %s = \n{\n", table_name);
		size_t count = 0;
		for (auto [vk, scancode, name] : conversion_table)
		{
			if (count > 0)
				fprintf(f, ",\n  {0x%x, 0x%x, \"%s\"}", vk, scancode, name.c_str());
			else
				fprintf(f, "  {0x%x, 0x%x, \"%s\"}", vk, scancode, name.c_str());
			++count;
		}
		fprintf(f, "\n};\n\n");
	};

	FILE* f = NULL;
	fopen_s(&f, filename, "w");
	if (f != NULL)
	{
		std::vector<KeyboardLayoutConversionEntry> conversion_table = GetKeyboardLayoutConversionTable();
		PrintToFile(f, table_name, conversion_table);
		fclose(f);
		return true;
	}
	return false;
}


int ConvertVirtualKeyIntoCurrentLayout(int src_vk, KeyboardLayout src_layout, KeyboardLayoutConversionEntry const ** src_entry = nullptr)
{
	// get the conversion table
	std::vector<KeyboardLayoutConversionEntry> const* conversion_table = nullptr;
	switch (src_layout)
	{
	case KeyboardLayout::AZERTY: conversion_table = &AzertyKeyboardLayoutConversionTable; break;
	case KeyboardLayout::QWERTY: conversion_table = &QwertyKeyboardLayoutConversionTable; break;
	default:assert(0); break;
	}
	// search int table the VK code corresponding, and translate scancode to new VK code
	for (KeyboardLayoutConversionEntry const& entry : *conversion_table)
	{
		if (entry.vk == src_vk)
		{
			if (src_entry != nullptr)
				*src_entry = &entry;
			return ::MapVirtualKey(entry.scancode, MAPVK_VSC_TO_VK);
		}
	}
	return src_vk;
}

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
			auto ImGui_DisplayConversion = [](int src_vk, KeyboardLayout src_layout)
			{
				KeyboardLayoutConversionEntry const* src_entry = nullptr;

				int new_vk = ConvertVirtualKeyIntoCurrentLayout(src_vk, src_layout, &src_entry);

				int new_scancode = ::MapVirtualKey(new_vk, MAPVK_VK_TO_VSC);

				char name[256];
				::GetKeyNameTextA(new_scancode << 16, name, sizeof(name));

				char const* layout_name = (src_layout == KeyboardLayout::AZERTY) ? "azerty" : "qwerty";

				ImGui::Text("%s (%s) -> %s (current)", src_entry->name.c_str(), layout_name, name);
			};

			ImGui_DisplayConversion('A', KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('A', KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('Q', KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('Q', KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('M', KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('M', KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SPACE, KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(VK_SPACE, KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion(VK_SHIFT, KeyboardLayout::AZERTY);
			ImGui_DisplayConversion(VK_SHIFT, KeyboardLayout::QWERTY);
			ImGui::Separator();

			ImGui_DisplayConversion('0', KeyboardLayout::AZERTY);
			ImGui_DisplayConversion('0', KeyboardLayout::QWERTY);
			ImGui::Separator();
		});
	}
};

int main(int argc, char ** argv, char ** env)
{
	//PrintScancodeVKConversionToFile("azerty.txt", "AzertyKeyboardLayoutConversionTable");
	//PrintScancodeVKConversionToFile("qwerty.txt", "QwertyKeyboardLayoutConversionTable");

	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}