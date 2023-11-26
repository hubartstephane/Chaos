#include "chaos/Chaos.h"

static constexpr int TESTS_PER_FONT = 4;

class WindowOpenGLTest : public chaos::Window
{

	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.7f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);



		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddTexture("material", texture);

		chaos::GPURenderParams render_params;
		mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh = nullptr;
		texture = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			ChangeFont(font_index + 1);
			return true;
		}
		else if (event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			ChangeFont(font_index - 1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	void ChangeFont(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> new_font = LoadFont(index);
		if (new_font != nullptr)
		{
			font_index = index;
			texture = new_font;
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> ReleaseResourceImpl(FT_Library* library, FT_Face* face)
	{
		if (face != nullptr)
			FT_Done_Face(*face);
		if (library != nullptr)
			FT_Done_FreeType(*library);
		return nullptr;
	}

	char const* GetFontName(int index) const
	{
		index = index / TESTS_PER_FONT;

		char const* font_names[] = {
		  "Multicolore.otf",
		  "Flatwheat-Regular.ttf",
		  "Flatwheat-Italic.ttf",
		  "unispace.ttf",
		  "unispace bold.ttf",
		  "unispace italic.ttf",
		  "unispace bold italic.ttf",
		  "Outwrite.ttf", // kerning !!
		  "absender1.ttf",    // 3 charmaps
		  "Destroy x.ttf",
		  "AgreloyInB1.ttf",
		  "AgreloyInT3.ttf",
		  "AgreloyOut1.ttf",
		  "AgreloyS1.ttf"
		};

		int font_count = sizeof(font_names) / sizeof(font_names[0]);

		if (index < 0)
			return nullptr;
		if (index >= font_count)
			return nullptr;

		return font_names[index];
	}

	void DisplayFaceInfo(FT_Face face) const
	{
		std::cout << "num_glyphs         : " << face->num_glyphs << std::endl;
		std::cout << "num_faces          : " << face->num_faces << std::endl;
		std::cout << "units_per_EM       : " << face->units_per_EM << std::endl;
		std::cout << "ascender           : " << face->ascender << std::endl;
		std::cout << "descender          : " << face->descender << std::endl;
		std::cout << "height             : " << face->height << std::endl;
		std::cout << "max_advance_width  : " << face->max_advance_width << std::endl;
		std::cout << "max_advance_height : " << face->max_advance_height << std::endl;
		std::cout << "num_charmaps       : " << face->num_charmaps << std::endl;
		std::cout << "min_glyph_x        : " << face->bbox.xMin << std::endl;
		std::cout << "max_glyph_x        : " << face->bbox.xMax << std::endl;
		std::cout << "min_glyph_y        : " << face->bbox.yMin << std::endl;
		std::cout << "max_glyph_y        : " << face->bbox.yMax << std::endl;

		if (FT_HAS_VERTICAL(face))
			face = face;

		if (FT_HAS_KERNING(face))
			face = face;

#define DISPLAY_FACE_FLAG(f) std::cout << #f " : " << ((face->face_flags & f) != 0) << std::endl;
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_SCALABLE);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_FIXED_SIZES);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_FIXED_WIDTH);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_SFNT);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_HORIZONTAL);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_VERTICAL);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_KERNING);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_FAST_GLYPHS);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_MULTIPLE_MASTERS);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_GLYPH_NAMES);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_EXTERNAL_STREAM);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_HINTER);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_CID_KEYED);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_TRICKY);
		DISPLAY_FACE_FLAG(FT_FACE_FLAG_COLOR);

#define DISPLAY_FACE_TEST(f) std::cout << #f " : " << (f(face) != 0) << std::endl;
		DISPLAY_FACE_TEST(FT_HAS_HORIZONTAL);
		DISPLAY_FACE_TEST(FT_HAS_VERTICAL);
		DISPLAY_FACE_TEST(FT_HAS_KERNING);
		DISPLAY_FACE_TEST(FT_HAS_FIXED_SIZES);
		DISPLAY_FACE_TEST(FT_HAS_GLYPH_NAMES);
		DISPLAY_FACE_TEST(FT_HAS_MULTIPLE_MASTERS);
		DISPLAY_FACE_TEST(FT_HAS_COLOR);
		DISPLAY_FACE_TEST(FT_IS_SFNT);
		DISPLAY_FACE_TEST(FT_IS_SCALABLE);
		DISPLAY_FACE_TEST(FT_IS_FIXED_WIDTH);
		DISPLAY_FACE_TEST(FT_IS_CID_KEYED);
		DISPLAY_FACE_TEST(FT_IS_TRICKY);

		for (int i = 0; i < face->num_fixed_sizes; ++i)
		{
			std::cout << "- Fixed Size   [SIZE] = " << face->available_sizes[i].size << std::endl;
			std::cout << "  Fixed Size  [WIDTH] = " << face->available_sizes[i].width << std::endl;
			std::cout << "  Fixed Size [HEIGHT] = " << face->available_sizes[i].height << std::endl;
		}

		FT_BBox  bbox = face->bbox;
		std::cout << "  bbox  [xMax] = " << bbox.xMax << std::endl;
		std::cout << "  bbox  [xMin] = " << bbox.xMin << std::endl;
		std::cout << "  bbox  [yMax] = " << bbox.yMax << std::endl;
		std::cout << "  bbox  [yMin] = " << bbox.yMin << std::endl;
	}

	void DisplayGlyphInfo(FT_GlyphSlot glyph) const
	{
		if (glyph->format == FT_GLYPH_FORMAT_NONE)
			std::cout << "FT_GLYPH_FORMAT_NONE" << std::endl;
		else if (glyph->format == FT_GLYPH_FORMAT_COMPOSITE)
			std::cout << "FT_GLYPH_FORMAT_COMPOSITE" << std::endl;
		else if (glyph->format == FT_GLYPH_FORMAT_BITMAP)
			std::cout << "FT_GLYPH_FORMAT_BITMAP" << std::endl;
		else if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			std::cout << "FT_GLYPH_FORMAT_OUTLINE" << std::endl;
		else if (glyph->format == FT_GLYPH_FORMAT_PLOTTER)
			std::cout << "FT_GLYPH_FORMAT_PLOTTER" << std::endl;

		std::cout << "- glyph  [WIDTH]     = " << glyph->bitmap.width << std::endl;
		std::cout << "  glyph  [PITCH]     = " << glyph->bitmap.pitch << std::endl;
		std::cout << "  glyph  [GRAYS]     = " << glyph->bitmap.num_grays << std::endl;
		std::cout << "  glyph  [MODE]      = " << (int)glyph->bitmap.pixel_mode << std::endl;
		std::cout << "  glyph  [ROWS]      = " << glyph->bitmap.rows << std::endl;
		std::cout << "  glyph  [ADVANCE X] = " << glyph->advance.x / 64 << std::endl;
		std::cout << "  glyph  [ADVANCE Y] = " << glyph->advance.y / 64 << std::endl;
		std::cout << "  glyph  [BITMAP L]  = " << glyph->bitmap_left << std::endl;
		std::cout << "  glyph  [BITMAP T]  = " << glyph->bitmap_top << std::endl;
		std::cout << "  glyph  [METRICS W] = " << glyph->metrics.width / 64 << std::endl;
		std::cout << "  glyph  [METRICS H] = " << glyph->metrics.height / 64 << std::endl;
		std::cout << "  glyph  [horiAdvance]  = " << glyph->metrics.horiAdvance / 64 << std::endl;
		std::cout << "  glyph  [horiBearingX] = " << glyph->metrics.horiBearingX / 64 << std::endl;
		std::cout << "  glyph  [horiBearingY] = " << glyph->metrics.horiBearingY / 64 << std::endl;
		std::cout << "  glyph  [vertAdvance]  = " << glyph->metrics.vertAdvance / 64 << std::endl;
		std::cout << "  glyph  [vertBearingX] = " << glyph->metrics.vertBearingX / 64 << std::endl;
		std::cout << "  glyph  [vertBearingY] = " << glyph->metrics.vertBearingY / 64 << std::endl;
	}

	void TweakBitmapOffset(int& bl, int& bt) const
	{
		bt = -bt;
	}

	chaos::shared_ptr<chaos::GPUTexture> LoadFont(int index)
	{
		FT_Error Err;

		FT_Library library;

		chaos::Application* application = chaos::Application::GetInstance();
		if (application == nullptr)
			return nullptr;

		// initialize library
		Err = FT_Init_FreeType(&library);
		if (Err)
			return ReleaseResourceImpl(nullptr, nullptr);

		// get the font name for the example
		char const* font_name = GetFontName(index);
		if (font_name == nullptr)
			return ReleaseResourceImpl(&library, nullptr);

		// load the foot
		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path font_path = resources_path / font_name;

		FT_Face face;
		Err = FT_New_Face(library, font_path.string().c_str(), 0, &face);
		if (Err)
			return ReleaseResourceImpl(&library, nullptr);

		// display debug information
		DisplayFaceInfo(face);

		// set pixel size
		Err = FT_Set_Pixel_Sizes(face, 64, 64); // Important else FT_Load_Glyph(...) fails
		if (Err)
			return ReleaseResourceImpl(&library, &face);

		chaos::GenTextureParameters parameters;
		parameters.wrap_r = GL_CLAMP;
		parameters.wrap_s = GL_CLAMP;
		parameters.wrap_t = GL_CLAMP;
		//parameters.min_filter = GL_NEAREST;
		//parameters.mag_filter = GL_NEAREST;


		chaos::PixelFormat pixel_format = chaos::PixelFormat::RGBAFloat;

		FIBITMAP* bm = nullptr;
#if 0
		bm = chaos::FontTools::GenerateImage(face, font_name[2], pixel_format);

#else

		if ((index % TESTS_PER_FONT) == 0)
			bm = chaos::FontTools::GenerateImage(face, font_name, pixel_format);
		else if ((index % TESTS_PER_FONT) == 1)
			bm = chaos::FontTools::GenerateImage(face, font_name[2], pixel_format);
		else if ((index % TESTS_PER_FONT) == 2)
			bm = chaos::FontTools::GenerateImage(face, "abcdefghijkl  mnopqrstuvwxyz", pixel_format);
		else if ((index % TESTS_PER_FONT) == 3)
			bm = chaos::FontTools::GenerateImage(face, "ABCDEFGHIJKL  MNOPQRSTUVWXYZ", pixel_format);


#endif

		chaos::shared_ptr<chaos::GPUTexture> result = chaos::GPUTextureLoader().GenTextureObject(bm, parameters);

		glfwSetWindowSize(glfw_window, FreeImage_GetWidth(bm), FreeImage_GetHeight(bm));

		FreeImage_Unload(bm);

		ReleaseResourceImpl(&library, &face);
		return result;

#if 0

		int A_index = FT_Get_Char_Index(face, 'A');
		int V_index = FT_Get_Char_Index(face, 'V');

		FT_Vector kerning;
		Err = FT_Get_Kerning(face, A_index, V_index, FT_KERNING_DEFAULT, &kerning);
		if (Err)
			return ReleaseResourceImpl(&library, &face);

		std::cout << "  kerningX A/V = " << kerning.x << std::endl;
		std::cout << "  kerningY A/V = " << kerning.y << std::endl;
#endif
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::Application* application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
		boost::filesystem::path vertex_shader_path = resources_path / "vertex_shader.txt";

		texture = LoadFont(font_index);
		if (texture == nullptr)
			return false;

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, fragment_shader_path);
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, vertex_shader_path);

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// create the mesh
		chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		mesh = chaos::GPUQuadMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		return true;
	}

protected:

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	int font_index = 0;
};

int main(int argc, char** argv, char** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}