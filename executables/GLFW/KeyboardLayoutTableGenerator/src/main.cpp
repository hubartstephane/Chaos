#include "chaos/Chaos.h"

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
	GenerateKeyboardLayoutFiles();
	return 0;
}