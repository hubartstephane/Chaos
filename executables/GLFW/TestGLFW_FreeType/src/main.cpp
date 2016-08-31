#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/GLProgramData.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1(){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    //return true;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(program->GetResourceID());
         
    glBindTextureUnit(0, texture->GetResourceID());

    chaos::GLProgramData const & program_data = program->GetProgramData();
    program_data.SetUniform("material", 0);

    mesh->Render(program_data, nullptr, 0, 0);

    return true;
  }

  virtual void Finalize() override
  {
    program = nullptr;
    mesh    = nullptr;
    texture = nullptr;
  }

  virtual bool Initialize() override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    FT_Error Err;

    FT_Library library;

    Err = FT_Init_FreeType(&library);
    if (Err)
      return false;

    int font_index = 2;

    char const * font_name = nullptr;    
    if (font_index == 0) font_name = "Flatwheat-Regular.ttf";
    if (font_index == 1) font_name = "Flatwheat-Italic.ttf";
    if (font_index == 2) font_name = "unispace.ttf";
    if (font_index == 3) font_name = "unispace bold.ttf";
    if (font_index == 4) font_name = "unispace italic.ttf";
    if (font_index == 5) font_name = "unispace bold italic.ttf";
    if (font_name == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path      = resources_path / font_name;

    FT_Face face;
    Err = FT_New_Face(library, font_path.string().c_str(), 0, &face);
    if (Err)
      return false;

    std::cout << "num_glyphs         : " << face->num_glyphs << std::endl;
    std::cout << "num_faces          : " << face->num_faces << std::endl;
    std::cout << "units_per_EM       : " << face->units_per_EM << std::endl;
    std::cout << "ascender           : " << face->ascender << std::endl;
    std::cout << "descender          : " << face->descender << std::endl;
    std::cout << "height             : " << face->height << std::endl;
    std::cout << "max_advance_width  : " << face->max_advance_width << std::endl;
    std::cout << "max_advance_height : " << face->max_advance_height << std::endl;
    std::cout << "num_charmaps       : " << face->num_charmaps << std::endl;

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

    for (int i = 0 ; i < face->num_fixed_sizes ; ++i)
    {
      std::cout << "- Fixed Size   [SIZE] = " << face->available_sizes[i].size   << std::endl;
      std::cout << "  Fixed Size  [WIDTH] = " << face->available_sizes[i].width  << std::endl;
      std::cout << "  Fixed Size [HEIGHT] = " << face->available_sizes[i].height << std::endl;  
    }

    Err = FT_Set_Pixel_Sizes(face, 128, 128);
    if (Err)
      return false;

    int glyph_index = FT_Get_Char_Index(face, '9' );
    if (glyph_index == 0)
      return false;

    Err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (Err)
      return false;

    Err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (Err)
      return false;

    std::cout << "- glyph  [WIDTH]     = " << face->glyph->bitmap.width << std::endl;
    std::cout << "  glyph  [PITCH]     = " << face->glyph->bitmap.pitch << std::endl;
    std::cout << "  glyph  [GRAYS]     = " << face->glyph->bitmap.num_grays << std::endl;
    std::cout << "  glyph  [MODE]      = " << (int)face->glyph->bitmap.pixel_mode << std::endl;
    std::cout << "  glyph  [ROWS]      = " << face->glyph->bitmap.rows  << std::endl;
    std::cout << "  glyph  [ADVANCE X] = " << face->glyph->advance.x / 64<< std::endl;
    std::cout << "  glyph  [ADVANCE Y] = " << face->glyph->advance.y / 64 << std::endl;
    std::cout << "  glyph  [BITMAP L]  = " << face->glyph->bitmap_left << std::endl;
    std::cout << "  glyph  [BITMAP T]  = " << face->glyph->bitmap_top << std::endl;
    std::cout << "  glyph  [METRICS W] = " << face->glyph->metrics.width / 64 << std::endl;
    std::cout << "  glyph  [METRICS H] = " << face->glyph->metrics.height / 64<< std::endl;
    std::cout << "  glyph  [horiAdvance]  = " << face->glyph->metrics.horiAdvance / 64 << std::endl;
    std::cout << "  glyph  [horiBearingX] = " << face->glyph->metrics.horiBearingX / 64  << std::endl;
    std::cout << "  glyph  [horiBearingY] = " << face->glyph->metrics.horiBearingY / 64  << std::endl;
    std::cout << "  glyph  [vertAdvance]  = " << face->glyph->metrics.vertAdvance / 64  << std::endl;
    std::cout << "  glyph  [vertBearingX] = " << face->glyph->metrics.vertBearingX / 64  << std::endl;
    std::cout << "  glyph  [vertBearingY] = " << face->glyph->metrics.vertBearingY / 64  << std::endl;

    FT_BBox  bbox = face->bbox;
    std::cout << "  bbox  [xMax] = " << bbox.xMax << std::endl;
    std::cout << "  bbox  [xMin] = " << bbox.xMin << std::endl;
    std::cout << "  bbox  [yMax] = " << bbox.yMax << std::endl;
    std::cout << "  bbox  [yMin] = " << bbox.yMin << std::endl;

    int A_index = FT_Get_Char_Index(face, 'A' );
    int V_index = FT_Get_Char_Index(face, 'V' );

    FT_Vector kerning;
    Err = FT_Get_Kerning( face, A_index, V_index, FT_KERNING_DEFAULT, &kerning);
    if (Err)
      return false;

    std::cout << "  kerningX A/V = " << kerning.x << std::endl;
    std::cout << "  kerningY A/V = " << kerning.y << std::endl;

    chaos::ImageDescription image_description = chaos::ImageDescription(
      face->glyph->bitmap.buffer, 
      face->glyph->metrics.width / 64, face->glyph->metrics.height / 64,
      8,
      0       
    );

    chaos::GenTextureParameters parameters;
    parameters.wrap_r = GL_CLAMP;
    parameters.wrap_s = GL_CLAMP;
    parameters.wrap_t = GL_CLAMP;

    texture = chaos::GLTools::GenTextureObject(image_description, parameters);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, fragment_shader_path);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   vertex_shader_path);
    
    program = loader.GenerateProgramObject();
    if (program == nullptr)
      return false;

    // create the mesh
    chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

    mesh = chaos::QuadMeshGenerator(b).GenerateMesh();
    if (mesh == nullptr)
      return false;

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel  = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<chaos::GLProgram>  program;
  boost::intrusive_ptr<chaos::SimpleMesh> mesh;
  boost::intrusive_ptr<chaos::Texture>    texture;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    
  FreeImage_Initialise(); // glew will be initialized 
  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


