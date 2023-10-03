#include "chaos/Chaos.h"

enum class KeyboardLayout
{
	AZERTY,
	QWERTY
};

class ScancodeVKPair
{
public:

	unsigned int scancode = 0;
	unsigned int vk = 0;
};

std::vector<ScancodeVKPair> AzertyScancodeToVK =
{
	{1, 27},
	{2, 49},
	{3, 50},
	{4, 51},
	{5, 52},
	{6, 53},
	{7, 54},
	{8, 55},
	{9, 56},
	{10, 57},
	{11, 48},
	{12, 189},
	{13, 187},
	{14, 8},
	{15, 9},
	{16, 81},
	{17, 87},
	{18, 69},
	{19, 82},
	{20, 84},
	{21, 89},
	{22, 85},
	{23, 73},
	{24, 79},
	{25, 80},
	{26, 219},
	{27, 221},
	{28, 13},
	{29, 17},
	{30, 65},
	{31, 83},
	{32, 68},
	{33, 70},
	{34, 71},
	{35, 72},
	{36, 74},
	{37, 75},
	{38, 76},
	{39, 186},
	{40, 222},
	{41, 192},
	{42, 16},
	{43, 220},
	{44, 90},
	{45, 88},
	{46, 67},
	{47, 86},
	{48, 66},
	{49, 78},
	{50, 77},
	{51, 188},
	{52, 190},
	{53, 191},
	{54, 16},
	{55, 106},
	{56, 18},
	{57, 32},
	{58, 20},
	{59, 112},
	{60, 113},
	{61, 114},
	{62, 115},
	{63, 116},
	{64, 117},
	{65, 118},
	{66, 119},
	{67, 120},
	{68, 121},
	{69, 144},
	{70, 145},
	{71, 36},
	{72, 38},
	{73, 33},
	{74, 109},
	{75, 37},
	{76, 12},
	{77, 39},
	{78, 107},
	{79, 35},
	{80, 40},
	{81, 34},
	{82, 45},
	{83, 46},
	{84, 44},
	{86, 226},
	{87, 122},
	{88, 123},
	{89, 12},
	{90, 238},
	{91, 241},
	{92, 234},
	{93, 249},
	{94, 245},
	{95, 243},
	{98, 251},
	{99, 47},
	{100, 124},
	{101, 125},
	{102, 126},
	{103, 127},
	{104, 128},
	{105, 129},
	{106, 130},
	{107, 131},
	{108, 132},
	{109, 133},
	{110, 134},
	{111, 237},
	{113, 233},
	{115, 193},
	{118, 135},
	{123, 235},
	{124, 9},
	{126, 194},
	{57360, 177},
	{57369, 176},
	{57372, 13},
	{57373, 17},
	{57376, 173},
	{57377, 183},
	{57378, 179},
	{57380, 178},
	{57390, 174},
	{57392, 175},
	{57394, 172},
	{57397, 111},
	{57399, 44},
	{57400, 18},
	{57414, 3},
	{57415, 36},
	{57416, 38},
	{57417, 33},
	{57419, 37},
	{57421, 39},
	{57423, 35},
	{57424, 40},
	{57425, 34},
	{57426, 45},
	{57427, 46},
	{57435, 91},
	{57436, 92},
	{57437, 93},
	{57439, 95},
	{57445, 170},
	{57446, 171},
	{57447, 168},
	{57448, 169},
	{57449, 167},
	{57450, 166},
	{57451, 182},
	{57452, 180},
	{57453, 181},
	{57629, 19}
};

std::vector<ScancodeVKPair> AzertyVKToScancode =
{
	{70, 3},
	{14, 8},
	{15, 9},
	{76, 12},
	{28, 13},
	{42, 16},
	{29, 17},
	{56, 18},
	{58, 20},
	{1, 27},
	{57, 32},
	{73, 33},
	{81, 34},
	{79, 35},
	{71, 36},
	{75, 37},
	{72, 38},
	{77, 39},
	{80, 40},
	{84, 44},
	{82, 45},
	{83, 46},
	{99, 47},
	{11, 48},
	{2, 49},
	{3, 50},
	{4, 51},
	{5, 52},
	{6, 53},
	{7, 54},
	{8, 55},
	{9, 56},
	{10, 57},
	{30, 65},
	{48, 66},
	{46, 67},
	{32, 68},
	{18, 69},
	{33, 70},
	{34, 71},
	{35, 72},
	{23, 73},
	{36, 74},
	{37, 75},
	{38, 76},
	{50, 77},
	{49, 78},
	{24, 79},
	{25, 80},
	{16, 81},
	{19, 82},
	{31, 83},
	{20, 84},
	{22, 85},
	{47, 86},
	{17, 87},
	{45, 88},
	{21, 89},
	{44, 90},
	{91, 91},
	{92, 92},
	{93, 93},
	{95, 95},
	{82, 96},
	{79, 97},
	{80, 98},
	{81, 99},
	{75, 100},
	{76, 101},
	{77, 102},
	{71, 103},
	{72, 104},
	{73, 105},
	{55, 106},
	{78, 107},
	{74, 109},
	{83, 110},
	{53, 111},
	{59, 112},
	{60, 113},
	{61, 114},
	{62, 115},
	{63, 116},
	{64, 117},
	{65, 118},
	{66, 119},
	{67, 120},
	{68, 121},
	{87, 122},
	{88, 123},
	{100, 124},
	{101, 125},
	{102, 126},
	{103, 127},
	{104, 128},
	{105, 129},
	{106, 130},
	{107, 131},
	{108, 132},
	{109, 133},
	{110, 134},
	{118, 135},
	{69, 144},
	{70, 145},
	{42, 160},
	{54, 161},
	{29, 162},
	{29, 163},
	{56, 164},
	{56, 165},
	{106, 166},
	{105, 167},
	{103, 168},
	{104, 169},
	{101, 170},
	{102, 171},
	{50, 172},
	{32, 173},
	{46, 174},
	{48, 175},
	{25, 176},
	{16, 177},
	{36, 178},
	{34, 179},
	{108, 180},
	{109, 181},
	{107, 182},
	{33, 183},
	{39, 186},
	{13, 187},
	{51, 188},
	{12, 189},
	{52, 190},
	{53, 191},
	{41, 192},
	{115, 193},
	{126, 194},
	{26, 219},
	{43, 220},
	{27, 221},
	{40, 222},
	{86, 226},
	{113, 233},
	{92, 234},
	{123, 235},
	{111, 237},
	{90, 238},
	{91, 241},
	{95, 243},
	{94, 245},
	{93, 249},
	{98, 251}
};

std::vector<ScancodeVKPair> QwertyScancodeToVK =
{
	{1, 27},
	{2, 49},
	{3, 50},
	{4, 51},
	{5, 52},
	{6, 53},
	{7, 54},
	{8, 55},
	{9, 56},
	{10, 57},
	{11, 48},
	{12, 219},
	{13, 187},
	{14, 8},
	{15, 9},
	{16, 65},
	{17, 90},
	{18, 69},
	{19, 82},
	{20, 84},
	{21, 89},
	{22, 85},
	{23, 73},
	{24, 79},
	{25, 80},
	{26, 221},
	{27, 186},
	{28, 13},
	{29, 17},
	{30, 81},
	{31, 83},
	{32, 68},
	{33, 70},
	{34, 71},
	{35, 72},
	{36, 74},
	{37, 75},
	{38, 76},
	{39, 77},
	{40, 192},
	{41, 222},
	{42, 16},
	{43, 220},
	{44, 87},
	{45, 88},
	{46, 67},
	{47, 86},
	{48, 66},
	{49, 78},
	{50, 188},
	{51, 190},
	{52, 191},
	{53, 223},
	{54, 16},
	{55, 106},
	{56, 18},
	{57, 32},
	{58, 20},
	{59, 112},
	{60, 113},
	{61, 114},
	{62, 115},
	{63, 116},
	{64, 117},
	{65, 118},
	{66, 119},
	{67, 120},
	{68, 121},
	{69, 144},
	{70, 145},
	{71, 36},
	{72, 38},
	{73, 33},
	{74, 109},
	{75, 37},
	{76, 12},
	{77, 39},
	{78, 107},
	{79, 35},
	{80, 40},
	{81, 34},
	{82, 45},
	{83, 46},
	{84, 44},
	{86, 226},
	{87, 122},
	{88, 123},
	{89, 12},
	{90, 238},
	{91, 241},
	{92, 234},
	{93, 249},
	{94, 245},
	{95, 243},
	{98, 251},
	{99, 47},
	{100, 124},
	{101, 125},
	{102, 126},
	{103, 127},
	{104, 128},
	{105, 129},
	{106, 130},
	{107, 131},
	{108, 132},
	{109, 133},
	{110, 134},
	{111, 237},
	{113, 233},
	{115, 193},
	{118, 135},
	{123, 235},
	{124, 9},
	{126, 194},
	{57360, 177},
	{57369, 176},
	{57372, 13},
	{57373, 17},
	{57376, 173},
	{57377, 183},
	{57378, 179},
	{57380, 178},
	{57390, 174},
	{57392, 175},
	{57394, 172},
	{57397, 111},
	{57399, 44},
	{57400, 18},
	{57414, 3},
	{57415, 36},
	{57416, 38},
	{57417, 33},
	{57419, 37},
	{57421, 39},
	{57423, 35},
	{57424, 40},
	{57425, 34},
	{57426, 45},
	{57427, 46},
	{57435, 91},
	{57436, 92},
	{57437, 93},
	{57439, 95},
	{57445, 170},
	{57446, 171},
	{57447, 168},
	{57448, 169},
	{57449, 167},
	{57450, 166},
	{57451, 182},
	{57452, 180},
	{57453, 181},
	{57629, 19}
};

std::vector<ScancodeVKPair> QwertyVKToScancode =
{
	{70, 3},
	{14, 8},
	{15, 9},
	{76, 12},
	{28, 13},
	{42, 16},
	{29, 17},
	{56, 18},
	{58, 20},
	{1, 27},
	{57, 32},
	{73, 33},
	{81, 34},
	{79, 35},
	{71, 36},
	{75, 37},
	{72, 38},
	{77, 39},
	{80, 40},
	{84, 44},
	{82, 45},
	{83, 46},
	{99, 47},
	{11, 48},
	{2, 49},
	{3, 50},
	{4, 51},
	{5, 52},
	{6, 53},
	{7, 54},
	{8, 55},
	{9, 56},
	{10, 57},
	{16, 65},
	{48, 66},
	{46, 67},
	{32, 68},
	{18, 69},
	{33, 70},
	{34, 71},
	{35, 72},
	{23, 73},
	{36, 74},
	{37, 75},
	{38, 76},
	{39, 77},
	{49, 78},
	{24, 79},
	{25, 80},
	{30, 81},
	{19, 82},
	{31, 83},
	{20, 84},
	{22, 85},
	{47, 86},
	{44, 87},
	{45, 88},
	{21, 89},
	{17, 90},
	{91, 91},
	{92, 92},
	{93, 93},
	{95, 95},
	{82, 96},
	{79, 97},
	{80, 98},
	{81, 99},
	{75, 100},
	{76, 101},
	{77, 102},
	{71, 103},
	{72, 104},
	{73, 105},
	{55, 106},
	{78, 107},
	{74, 109},
	{83, 110},
	{53, 111},
	{59, 112},
	{60, 113},
	{61, 114},
	{62, 115},
	{63, 116},
	{64, 117},
	{65, 118},
	{66, 119},
	{67, 120},
	{68, 121},
	{87, 122},
	{88, 123},
	{100, 124},
	{101, 125},
	{102, 126},
	{103, 127},
	{104, 128},
	{105, 129},
	{106, 130},
	{107, 131},
	{108, 132},
	{109, 133},
	{110, 134},
	{118, 135},
	{69, 144},
	{70, 145},
	{42, 160},
	{54, 161},
	{29, 162},
	{29, 163},
	{56, 164},
	{56, 165},
	{106, 166},
	{105, 167},
	{103, 168},
	{104, 169},
	{101, 170},
	{102, 171},
	{50, 172},
	{32, 173},
	{46, 174},
	{48, 175},
	{25, 176},
	{16, 177},
	{36, 178},
	{34, 179},
	{108, 180},
	{109, 181},
	{107, 182},
	{33, 183},
	{27, 186},
	{13, 187},
	{50, 188},
	{51, 190},
	{52, 191},
	{40, 192},
	{115, 193},
	{126, 194},
	{12, 219},
	{43, 220},
	{26, 221},
	{41, 222},
	{53, 223},
	{86, 226},
	{113, 233},
	{92, 234},
	{123, 235},
	{111, 237},
	{90, 238},
	{91, 241},
	{95, 243},
	{94, 245},
	{93, 249},
	{98, 251}
};

std::vector<ScancodeVKPair> GetVKToScancodeForCurrentLayout()
{
	std::vector<ScancodeVKPair> result;

	for (unsigned int vk = 0; vk < 100000; ++vk)
	{
		unsigned int scancode = ::MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, nullptr);
		if (scancode != 0)
			result.push_back({ scancode, vk });
	}
	return result;
}

std::vector<ScancodeVKPair> GetScancodeToVKForCurrentLayout()
{
	std::vector<ScancodeVKPair> result;

	for (unsigned int scancode = 0; scancode < 100000; ++scancode)
	{
		unsigned int vk = ::MapVirtualKeyEx(scancode, MAPVK_VSC_TO_VK, nullptr);
		if (vk != 0)
			result.push_back({ scancode, vk });
	}
	return result;
}

bool PrintScancodeVKConversionToFile(char const* filename, char const* prefix)
{
	auto PrintToFile = [](FILE *f, char const * table_name, char const * prefix, std::vector<ScancodeVKPair> const & table)
	{
		fprintf(f, "std::vector<ScancodeVKPair> %s%s = \n{\n", prefix, table_name);
		size_t count = 0;
		for (auto [scan, vk] : table)
		{
			if (count > 0)
				fprintf(f, ",\n	{%d, %d}", scan, vk);
			else
				fprintf(f, "	{%d, %d}", scan, vk);
			++count;
		}
		fprintf(f, "\n};\n\n");
	};

	FILE* f = NULL;
	fopen_s(&f, filename, "w");
	if (f != NULL)
	{
		std::vector<ScancodeVKPair> ScancodeToVK = GetScancodeToVKForCurrentLayout();
		PrintToFile(f, "ScancodeToVK", prefix, ScancodeToVK);

		std::vector<ScancodeVKPair> VKToScancode = GetVKToScancodeForCurrentLayout();
		PrintToFile(f, "VKToScancode", prefix, VKToScancode);

		fclose(f);
		return true;
	}
	return false;
}

int ConvertKeyToCurrentLayout(int key, KeyboardLayout source_layout = KeyboardLayout::QWERTY)
{
	// get the conversion table
	std::vector<ScancodeVKPair> const* conversion_table = nullptr;
	switch (source_layout)
	{
	case KeyboardLayout::AZERTY: conversion_table = &AzertyScancodeToVK; break;
	case KeyboardLayout::QWERTY: conversion_table = &QwertyScancodeToVK; break;
	default:assert(0); break;
	}

	// get scancode on the current layout
	unsigned int scancode = ::MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, nullptr);

	// search on conversion table the entry with same scancode
	for (ScancodeVKPair const& pair : *conversion_table)
		if (pair.scancode == scancode)
			return pair.vk;

	return key;
}

#if 0

int ConvertKey(int K)
{

#if 0
	char buf[KL_NAMELENGTH];

	GetKeyboardLayoutName(buf);

	HKL Layouts[1000];

	int R = GetKeyboardLayoutList(1000, Layouts);

	int ScanCode = glfwGetKeyScancode(K);



	std::map<unsigned int, unsigned int> m1;
	std::map<unsigned int, unsigned int> m2;

	for (unsigned int i = 0; i < 100000; ++i)
	{
		unsigned int scan1 = ::MapVirtualKeyEx(i, MAPVK_VSC_TO_VK, Layouts[0]);
		unsigned int scan2 = ::MapVirtualKeyEx(i, MAPVK_VSC_TO_VK, Layouts[1]);

		if (scan1 != 0)
			m1.insert({ i, scan1 });
		if (scan2 != 0)
			m2.insert({ i, scan2 });

		if (scan1 != scan2)
			scan1 = scan1;

		if (i == std::numeric_limits<unsigned int>::max())
			break;
	}



	for (int i = 0; i < m1.size(); ++i)
	{
		if (m1[i] == K)
			K = K;
		if (m2[i] == K)
			K = K;



	}




	UINT VKCode1 = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK, Layouts[0]);
	UINT VKCode2 = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK, Layouts[1]);

	UINT C1 = ::MapVirtualKeyEx(VKCode1, MAPVK_VK_TO_CHAR, Layouts[0]);
	UINT C2 = ::MapVirtualKeyEx(VKCode2, MAPVK_VK_TO_CHAR, Layouts[1]);

	R = R;
#endif

#if 0

	int GetKeyboardLayoutList(
		[in]  int nBuff,
		[out] HKL * lpList
	);




	HKL hKeyboardLayout = ::GetKeyboardLayout(NULL);

	UINT VKCode = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK, hKeyboardLayout);

	UINT CharCode = ::MapVirtualKeyEx(K, MAPVK_VK_TO_CHAR, hKeyboardLayout);


#endif




	//exit(0);
#if 0
	auto p = VK_LBUTTON;

	HKL hKeyboardLayout = ::GetKeyboardLayout(NULL);

	UINT VKCode = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK, hKeyboardLayout);

	UINT CharCode = ::MapVirtualKeyEx(VKCode, MAPVK_VK_TO_CHAR, hKeyboardLayout);

	UINT a = ::MapVirtualKeyEx(K, MAPVK_VK_TO_VSC, hKeyboardLayout);
	UINT b = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK, hKeyboardLayout);
	UINT c = ::MapVirtualKeyEx(K, MAPVK_VK_TO_CHAR, hKeyboardLayout);
	UINT d = ::MapVirtualKeyEx(K, MAPVK_VSC_TO_VK_EX, hKeyboardLayout);
#endif


	return 0;

#if 0
	HKL hKeyboardLayout = ::GetKeyboardLayout(NULL);
	for (UINT ScanCode = 0; ScanCode <= 0xFF; ++ScanCode)
	{
		UINT VKCode = ::MapVirtualKeyEx(ScanCode, MAPVK_VSC_TO_VK, hKeyboardLayout);
		if (VKCode != 0)
		{
			UINT CharCode = ::MapVirtualKeyEx(VKCode, MAPVK_VK_TO_CHAR, hKeyboardLayout);
			Result.Push({ ScanCode, VKCode, CharCode });
		}
	}
#endif

}



#endif

int main(int argc, char ** argv, char ** env)
{
	//PrintScancodeVKConversionToFile("d://keyboard.txt", "Qwerty");

	auto n1 = ConvertKeyToCurrentLayout(GLFW_KEY_A, KeyboardLayout::AZERTY);
	auto n2 = ConvertKeyToCurrentLayout(GLFW_KEY_A, KeyboardLayout::QWERTY);

	auto n3 = ConvertKeyToCurrentLayout(GLFW_KEY_Q, KeyboardLayout::AZERTY);
	auto n4 = ConvertKeyToCurrentLayout(GLFW_KEY_Q, KeyboardLayout::QWERTY);

	auto n5 = ConvertKeyToCurrentLayout(GLFW_KEY_Z, KeyboardLayout::AZERTY);
	auto n6 = ConvertKeyToCurrentLayout(GLFW_KEY_Z, KeyboardLayout::QWERTY);

	auto n7 = ConvertKeyToCurrentLayout(GLFW_KEY_W, KeyboardLayout::AZERTY);
	auto n8 = ConvertKeyToCurrentLayout(GLFW_KEY_W, KeyboardLayout::QWERTY);

	auto n9 = ConvertKeyToCurrentLayout(GLFW_KEY_M, KeyboardLayout::AZERTY);
	auto n10 = ConvertKeyToCurrentLayout(GLFW_KEY_M, KeyboardLayout::QWERTY);

	auto n11 = ConvertKeyToCurrentLayout(GLFW_KEY_COMMA, KeyboardLayout::AZERTY);
	auto n12 = ConvertKeyToCurrentLayout(GLFW_KEY_COMMA, KeyboardLayout::QWERTY);

	auto n13 = ConvertKeyToCurrentLayout(GLFW_KEY_SEMICOLON, KeyboardLayout::AZERTY);
	auto n14 = ConvertKeyToCurrentLayout(GLFW_KEY_SEMICOLON, KeyboardLayout::QWERTY);


	return 0;
}