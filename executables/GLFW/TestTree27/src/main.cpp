#include "chaos/Chaos.h"

#include "PrimitiveRenderer.h"



// =======================================================================

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














static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// =======================================================================

class Tree27NodeBase
{
public:

	bool IsUseful() const
	{
		return (objects.size() > 0);
	}

public:

	std::vector<class GeometricObject*> objects;
};

// =======================================================================

class GeometricObject : public chaos::Object
{
public:

	chaos::sphere3 sphere;

	size_t object_index = 0;

	chaos::Tree27<3, Tree27NodeBase>::node_type * node = nullptr;
};

// =======================================================================

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	float fov = 60.0f;
	float far_plane = 5000.0f;
	float near_plane = 20.0f;
	float create_object_distance = 50.0f;

	glm::mat4x4 GetProjectionMatrix(glm::vec2 viewport_size) const
	{
		return glm::perspectiveFov(fov * (float)M_PI / 180.0f, float(viewport_size.x), float(viewport_size.y), near_plane, far_plane);
	}

	glm::mat4x4 GetGlobalToCameraMatrix() const
	{
		return fps_view_controller.GlobalToLocal();
	}

	glm::mat4x4 GetCameraToGlobalMatrix() const
	{
		return fps_view_controller.LocalToGlobal();
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		primitive_renderer->projection      = GetProjectionMatrix(draw_params.viewport.size);
		primitive_renderer->world_to_camera = GetGlobalToCameraMatrix();
		primitive_renderer->renderer        = renderer;

		DrawGeometryObjects();
		DrawTree27();

		return true;
	}

	void OnDrawWindowImGuiMenu() override
	{
		chaos::Window::OnDrawWindowImGuiMenu();

		chaos::ImGuiDrawableInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this]()
		{
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Show Help", nullptr, show_help, true))
					show_help = !show_help;
				ImGui::EndMenu();
			}
		});
	}

	void OnDrawWindowImGuiContent() override
	{
		if (show_help)
		{
			ImGui::Begin("help", &show_help);
			ImGui::Text("+      : next object");
			ImGui::Text("-      : previous object");
			ImGui::Text("space  : new object");
			ImGui::Text("delete : delete");
			ImGui::Text("y      : new random scene");
			ImGui::Text("z      : move forward");
			ImGui::Text("s      : move backward");
			ImGui::Text("q      : move left");
			ImGui::Text("d      : move right");
			ImGui::Text("a      : move down");
			ImGui::Text("e      : move up");
			ImGui::Text("r      : scale up");
			ImGui::Text("f      : scale down");
			ImGui::Text("shift  : speed");
			ImGui::End();
		}
	}

	void DrawTree27()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		object_tree.ForEachNode([this](chaos::Tree27<3, Tree27NodeBase>::node_type const* node)
		{
			primitive_renderer->GPUDrawPrimitive(node->GetBoundingBox(), white, false, true);
		});
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	void DrawGeometryObjects()
	{
		for (auto& geometric_object : geometric_objects)
		{
			if (geometric_object.get() == GetCurrentGeometricObject())
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightIncreaseSize(geometric_object->sphere), red, false);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else
			{
				primitive_renderer->GPUDrawPrimitive(geometric_object->sphere, white, false);
			}
		}
	}

	virtual void Finalize() override
	{
		primitive_renderer = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override
	{
		// super
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;
		// create the renderer
		primitive_renderer = new PrimitiveRenderer();
		if (primitive_renderer == nullptr)
			return false;
		if (!primitive_renderer->Initialize())
			return false;
		// update the camera speed
		fps_view_controller.movement_speed.back = camera_speed;
		fps_view_controller.movement_speed.down = camera_speed;
		fps_view_controller.movement_speed.up = camera_speed;
		fps_view_controller.movement_speed.forward = camera_speed;
		fps_view_controller.movement_speed.strafe = camera_speed;

		fps_view_controller.fps_controller.position.y = 30.0f;
		// create the very single sphere
		chaos::sphere3 creation_sphere;
		creation_sphere.position = glm::vec3(0.0f, 0.0f, -200.0f);
		creation_sphere.radius = 10.0f;
		CreateNewObject(creation_sphere);

		return true;
	}

	GeometricObject* GetCurrentGeometricObject()
	{
		return (geometric_objects.size() == 0) ? nullptr : geometric_objects[current_object_index].get();
	}

	bool GetImGuiMenuMode() const
	{
		if (chaos::WindowApplication const* application = chaos::Application::GetInstance())
		{
			return application->GetImGuiMenuMode();
		}

		return false;
	}

	chaos::ray3 GetRayFromMousePosition() const
	{
		glm::ivec2 window_size = GetWindowSize();
		if (window_size.x > 0 && window_size.y > 0)
		{
			// get the viewport
			chaos::aabox2 viewport = GetRequiredViewport(window_size);

			// get the position of the cursor
			double x = 0.0;
			double y = 0.0;
			glfwGetCursorPos(glfw_window, &x, &y);

			// compute the ray in projection space from eye center to cursor
			auto RecenterInterval = [](float value)
			{
				return (value * 2.0f - 1.0f);
			};

			glm::mat4x4 proj_inv = glm::inverse(GetProjectionMatrix(viewport.size));
			glm::mat4x4 cam_to_world = GetCameraToGlobalMatrix();

			glm::vec4 ray_point1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			ray_point1 = cam_to_world * ray_point1;

			glm::vec4 ray_point2 = glm::vec4(
				 RecenterInterval(((float)x - viewport.position.x) / viewport.size.x),
				-RecenterInterval(((float)y - viewport.position.y) / viewport.size.y), // for mouse position, Y = 0.0f is top Y = 1.0f is bottom
				1.0f,
				1.0f);

			ray_point2 = proj_inv * ray_point2;
			ray_point2 = cam_to_world * ray_point2;

			glm::vec3 p1 = glm::vec3(ray_point1.x / ray_point1.w, ray_point1.y / ray_point1.w, ray_point1.z / ray_point1.w);
			glm::vec3 p2 = glm::vec3(ray_point2.x / ray_point2.w, ray_point2.y / ray_point2.w, ray_point2.z / ray_point2.w);

			chaos::ray3 r;
			r.position = p1;
			r.direction = glm::normalize(p2 - p1);

			return r;
		}

		return chaos::ray3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_PRESS)
		{
			if (GetImGuiMenuMode())
			{
				chaos::ray3 r = GetRayFromMousePosition();

				glm::vec3 center = r.position + create_object_distance * r.direction;

				chaos::sphere3 creation_sphere;
				creation_sphere.position = center;
				creation_sphere.radius = 10.0f;
				CreateNewObject(creation_sphere);
			}
		}
		return true;
	}


	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		// change the current object if any
		if (GeometricObject* current_object = GetCurrentGeometricObject())
		{
			// change current selection
			if (event.IsKeyPressed(GLFW_KEY_KP_ADD))
			{
				current_object_index = (current_object_index + 1) % geometric_objects.size();
				return true;
			}
			else if (event.IsKeyPressed(GLFW_KEY_KP_SUBTRACT))
			{
				current_object_index = (current_object_index + geometric_objects.size() - 1) % geometric_objects.size();
				return true;
			}

			// create a new object from current object or nothing
			if (event.IsKeyPressed(GLFW_KEY_SPACE))
			{
				for (auto& object : geometric_objects)
					if (current_object != object.get() && object->sphere == current_object->sphere)
						return false;

				CreateNewObject(current_object->sphere);
				return true;
			}

			if (event.IsKeyPressed(GLFW_KEY_DELETE))
			{
				if (geometric_objects.size() > 1)
				{
					// remove the object from tree
					EraseObjectFromNode(current_object);
					// remove the object from object list
					auto it = std::ranges::find_if(geometric_objects, [current_object](auto& v)
					{
						return (v.get() == current_object);
					});
					geometric_objects.erase(it); // destroy the last reference on object
					// select object near in creation order
					if (current_object_index > 0)
						--current_object_index; // while there is always at least one object in the array, this is always valid
				}

				return true;
			}

			if (event.IsKeyPressed(GLFW_KEY_Y) || event.IsKeyPressed(ConvertKeyToCurrentLayout(GLFW_KEY_COMMA)))
			{
				geometric_objects.clear();
				current_object_index = 0;
				object_tree.Clear();

				for (int i = 0; i < 200; ++i)
				{
					chaos::sphere3 creation_sphere;
					creation_sphere.position.x = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);
					creation_sphere.position.y = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);
					creation_sphere.position.z = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);

					creation_sphere.radius = chaos::MathTools::RandFloat(2.0f, 50.0f);
					CreateNewObject(creation_sphere);
				}
				return true;
			}
		}

		return chaos::Window::OnKeyEventImpl(event);
	}

	GeometricObject * CreateNewObject(chaos::sphere3 const & creation_sphere)
	{
		if (GeometricObject* new_object = new GeometricObject)
		{
			new_object->sphere = creation_sphere;
			new_object->object_index = new_object_index++;
			geometric_objects.push_back(new_object);
			current_object_index = geometric_objects.size() - 1;

			InsertObjectIntoNode(new_object, object_tree.GetOrCreateNode(chaos::GetBoundingBox(new_object->sphere)));

			return new_object;
		}
		return nullptr;
	}

	bool MoveObject(GeometricObject* object, int key, float& target_value, float delta_time, float speed, float fast_speed)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ? fast_speed : speed;
			target_value += final_speed * delta_time;
			return true;
		}
		return false;
	}


	bool MoveObject(GeometricObject* object, int key, float delta_time, glm::vec3 const & direction)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ?
				fast_displacement_speed:
				displacement_speed;

			object->sphere.position += glm::vec3(fps_view_controller.LocalToGlobal() * glm::vec4(direction, 0.0f)) * delta_time * final_speed;
			return true;
		}
		return false;
	}



	bool ScaleObject(GeometricObject* object, int key, float delta_time, float direction)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_scale_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ?
				fast_scale_speed :
				scale_speed;

			object->sphere.radius = std::max(1.0f, object->sphere.radius + direction * final_scale_speed);
			return true;
		}
		return false;
	}


	void InsertObjectIntoNode(GeometricObject* object, chaos::Tree27<3, Tree27NodeBase>::node_type * node)
	{
		object->node = node;
		object->node->objects.push_back(object);
	}

	void EraseObjectFromNode(GeometricObject* object)
	{
		object->node->objects.erase(std::ranges::find(object->node->objects, object));
		object_tree.DeleteNodeIfPossible(object->node);
		object->node = nullptr;
	}

	void OnObjectMoved(GeometricObject* object)
	{
		chaos::box3 new_box = chaos::GetBoundingBox(object->sphere);

		if (chaos::ComputeTreeNodeInfo(new_box) != object->node->GetNodeInfo()) // changement required
		{
			EraseObjectFromNode(object);
			InsertObjectIntoNode(object, object_tree.GetOrCreateNode(new_box));
		}
	}

	virtual bool DoTick(float delta_time) override
	{
		// move camera
		//if (!GetImGuiMenuMode())
		fps_view_controller.Tick(glfw_window, delta_time);

		// move object
		if (GeometricObject* current_object = GetCurrentGeometricObject())
		{
			bool moved = false;

			moved |= MoveObject(current_object, GLFW_KEY_A, delta_time, {-1.0f,  0.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_D, delta_time, { 1.0f,  0.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_Q, delta_time, { 0.0f, -1.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_E, delta_time, { 0.0f,  1.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_W, delta_time, { 0.0f,  0.0f, -1.0f });
			moved |= MoveObject(current_object, GLFW_KEY_S, delta_time, { 0.0f,  0.0f,  1.0f });

			moved |= ScaleObject(current_object, GLFW_KEY_R, delta_time,  1.0f);
			moved |= ScaleObject(current_object, GLFW_KEY_F, delta_time, -1.0f);

			if (moved)
				OnObjectMoved(current_object);
		}

		// object selection with mouse
		if (glfwGetInputMode(glfw_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		{
			glm::ivec2 window_size = GetWindowSize();
			if (window_size.x > 0 && window_size.y > 0)
			{
				// get the viewport
				chaos::aabox2 viewport = GetRequiredViewport(window_size);

				// get the position of the cursor
				double x = 0.0;
				double y = 0.0;
				glfwGetCursorPos(glfw_window, &x, &y);

				// search whether their is an object intersection
				GeometricObject const * intersection_obj = nullptr;

				chaos::ray3 r = GetRayFromMousePosition();
				for (auto const& obj : geometric_objects)
					if (chaos::RaySphereIntersectionResult<float, 3> intersections = chaos::GetIntersection(r, obj->sphere))
						intersection_obj = obj.get();

				// trace debugging information
				ImGui::Begin("Information");
				ImGui::Text("cursor            : (%0.3f, %0.3f)", (float)x, (float)y);
				ImGui::Text("window   size     : (%0.3f, %0.3f)", (float)window_size.x, (float)window_size.y);
				ImGui::Text("viewport position : (%0.3f, %0.3f)", (float)viewport.position.x, (float)viewport.position.y);
				ImGui::Text("viewport size     : (%0.3f, %0.3f)", (float)viewport.size.x, (float)viewport.size.y);
				ImGui::InputFloat("create_object_distance", &create_object_distance);
				ImGui::InputFloat("near_plane", &near_plane, 10.0f, 50.0f);
				ImGui::InputFloat("far_plane", &far_plane, 10.0f, 50.0f);
				ImGui::InputFloat("fov", &fov, 1.0f, 5.0f);
				if (intersection_obj != nullptr)
				{
					ImGui::Text("ray found sphere");
				}
				ImGui::End();
			}
		}

		return true; // refresh
	}

protected:

	// an object for debug rendering
	chaos::shared_ptr<PrimitiveRenderer> primitive_renderer;

	chaos::FPSViewInputController fps_view_controller;

	std::vector<chaos::shared_ptr<GeometricObject>> geometric_objects;

	size_t current_object_index = 0;

	size_t new_object_index = 0;

	float displacement_speed = 100.0f;

	float fast_displacement_speed = 200.0f;

	float scale_speed = 1.5f;

	float fast_scale_speed = 3.0f;

	float camera_speed = 400.0f;

	chaos::Tree27<3, Tree27NodeBase> object_tree;

	bool show_help = true;
};






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


	//return 0;








	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}