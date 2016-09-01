#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h> 
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

#include FT_OUTLINE_H
#include FT_IMAGE_H

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() : font_index(0){}

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

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
    {
      ChangeFont(font_index + 1);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      ChangeFont(font_index - 1);
    }
  }

  void ChangeFont(int index)
  {
    boost::intrusive_ptr<chaos::Texture> new_font = LoadFont(index, "Hello world");
    if (new_font != nullptr)
    {
      font_index = index;
      texture = new_font;
    }
  }

  boost::intrusive_ptr<chaos::Texture> ReleaseResourceImpl(FT_Library * library, FT_Face * face)
  {
    if (face != nullptr)
      FT_Done_Face(*face);
    if (library != nullptr)
      FT_Done_FreeType(*library);
    return nullptr;
  }

  boost::intrusive_ptr<chaos::Texture> LoadFont(int index, char const * str)
  {
    FT_Error Err;

    FT_Library library;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return nullptr;

    Err = FT_Init_FreeType(&library);
    if (Err)
      return ReleaseResourceImpl(&library, nullptr);

    char const * font_name = nullptr;
    if (index == 0) font_name = "Flatwheat-Regular.ttf";
    if (index == 1) font_name = "Flatwheat-Italic.ttf";
    if (index == 2) font_name = "unispace.ttf";
    if (index == 3) font_name = "unispace bold.ttf";
    if (index == 4) font_name = "unispace italic.ttf";
    if (index == 5) font_name = "unispace bold italic.ttf";
    if (index == 6) font_name = "Outwrite.ttf"; // kerning !!
    if (index == 7) font_name = "absender1.ttf";    // 3 charmaps
    if (font_name == nullptr)
      return ReleaseResourceImpl(&library, nullptr);


    std::cout << "==================== " << font_name << " ====================" << std::endl;

    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path      = resources_path / font_name;

    FT_Face face;
    Err = FT_New_Face(library, font_path.string().c_str(), 0, &face);
    if (Err)
      return ReleaseResourceImpl(&library, nullptr);

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

    for (int i = 0; i < face->num_fixed_sizes; ++i)
    {
      std::cout << "- Fixed Size   [SIZE] = " << face->available_sizes[i].size << std::endl;
      std::cout << "  Fixed Size  [WIDTH] = " << face->available_sizes[i].width << std::endl;
      std::cout << "  Fixed Size [HEIGHT] = " << face->available_sizes[i].height << std::endl;
    }

    Err = FT_Set_Pixel_Sizes(face, 128, 128);
    if (Err)
      return ReleaseResourceImpl(&library, &face);




#if 1
    // compute required size
    int required_width  = 0;
    int required_height = 0;

    int character = 0;
    while (str[character] != 0)
    {
      int glyph_index = FT_Get_Char_Index(face, str[character]);
      if (glyph_index == 0)
        return ReleaseResourceImpl(&library, &face);

      Err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
      if (Err)
        return ReleaseResourceImpl(&library, &face);

      int w = (int)face->glyph->metrics.width / 64;
      int h = (int)face->glyph->metrics.height / 64;

      int next = face->glyph->bitmap_left + w;

      if (str[character + 1] == 0)
        required_width += next;
      else
        required_width += max(next, (int)face->glyph->advance.x / 64);

      required_height = max(required_height, h + face->glyph->bitmap_top);

      ++character;
    }
    // reserve a buffer big enought
    boost::intrusive_ptr<chaos::Texture> result;

    unsigned char * buffer = new unsigned char[required_width * required_height];
    if (buffer != nullptr)
    {
      memset(buffer, 0, required_width * required_height);

      int pos_x = 0;
      int pos_y = 0;

      int character = 0;
      while (str[character] != 0)
      {
        int glyph_index = FT_Get_Char_Index(face, str[character]);

        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

        Err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (Err)
          return ReleaseResourceImpl(&library, &face);

        int w = face->glyph->metrics.width  / 64;
        int h = face->glyph->metrics.height / 64;

        unsigned char * dst = buffer + (pos_x + face->glyph->bitmap_left) + (pos_y /*+ face->glyph->bitmap_top*/) * required_width;
        for (int j = 0; j < h; ++j)
          for (int i = 0; i < w; ++i)
            dst[i + j * required_width] = 0;  //face->glyph->bitmap.buffer[i + j * w];

        pos_x += face->glyph->advance.x / 64;

        ++character;
      }






      // generate the texture
      chaos::ImageDescription image_description = chaos::ImageDescription(buffer, required_width, required_height, 8, 0);

      chaos::GenTextureParameters parameters;
      parameters.wrap_r = GL_CLAMP;
      parameters.wrap_s = GL_CLAMP;
      parameters.wrap_t = GL_CLAMP;


      result = chaos::GLTextureTools::GenTextureObject(image_description, parameters);

      delete[] buffer;
    }


    

    ReleaseResourceImpl(&library, &face);
    return result;

#endif






#if 0

    int glyph_index = FT_Get_Char_Index(face, 'R');
    if (glyph_index == 0)
      return ReleaseResourceImpl(&library, &face);

    Err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (Err)
      return ReleaseResourceImpl(&library, &face);



    Err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (Err)
      return ReleaseResourceImpl(&library, &face);

    if (face->glyph->format == FT_GLYPH_FORMAT_NONE) std::cout << "FT_GLYPH_FORMAT_NONE" << std::endl;
    if (face->glyph->format == FT_GLYPH_FORMAT_COMPOSITE) std::cout << "FT_GLYPH_FORMAT_COMPOSITE" << std::endl;
    if (face->glyph->format == FT_GLYPH_FORMAT_BITMAP) std::cout << "FT_GLYPH_FORMAT_BITMAP" << std::endl;
    if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) std::cout << "FT_GLYPH_FORMAT_OUTLINE" << std::endl;
    if (face->glyph->format == FT_GLYPH_FORMAT_PLOTTER) std::cout << "FT_GLYPH_FORMAT_PLOTTER" << std::endl;


    std::cout << "- glyph  [WIDTH]     = " << face->glyph->bitmap.width << std::endl;
    std::cout << "  glyph  [PITCH]     = " << face->glyph->bitmap.pitch << std::endl;
    std::cout << "  glyph  [GRAYS]     = " << face->glyph->bitmap.num_grays << std::endl;
    std::cout << "  glyph  [MODE]      = " << (int)face->glyph->bitmap.pixel_mode << std::endl;
    std::cout << "  glyph  [ROWS]      = " << face->glyph->bitmap.rows << std::endl;
    std::cout << "  glyph  [ADVANCE X] = " << face->glyph->advance.x / 64 << std::endl;
    std::cout << "  glyph  [ADVANCE Y] = " << face->glyph->advance.y / 64 << std::endl;
    std::cout << "  glyph  [BITMAP L]  = " << face->glyph->bitmap_left << std::endl;
    std::cout << "  glyph  [BITMAP T]  = " << face->glyph->bitmap_top << std::endl;
    std::cout << "  glyph  [METRICS W] = " << face->glyph->metrics.width / 64 << std::endl;
    std::cout << "  glyph  [METRICS H] = " << face->glyph->metrics.height / 64 << std::endl;
    std::cout << "  glyph  [horiAdvance]  = " << face->glyph->metrics.horiAdvance / 64 << std::endl;
    std::cout << "  glyph  [horiBearingX] = " << face->glyph->metrics.horiBearingX / 64 << std::endl;
    std::cout << "  glyph  [horiBearingY] = " << face->glyph->metrics.horiBearingY / 64 << std::endl;
    std::cout << "  glyph  [vertAdvance]  = " << face->glyph->metrics.vertAdvance / 64 << std::endl;
    std::cout << "  glyph  [vertBearingX] = " << face->glyph->metrics.vertBearingX / 64 << std::endl;
    std::cout << "  glyph  [vertBearingY] = " << face->glyph->metrics.vertBearingY / 64 << std::endl;

    FT_BBox  bbox = face->bbox;
    std::cout << "  bbox  [xMax] = " << bbox.xMax << std::endl;
    std::cout << "  bbox  [xMin] = " << bbox.xMin << std::endl;
    std::cout << "  bbox  [yMax] = " << bbox.yMax << std::endl;
    std::cout << "  bbox  [yMin] = " << bbox.yMin << std::endl;

    int A_index = FT_Get_Char_Index(face, 'A');
    int V_index = FT_Get_Char_Index(face, 'V');

    FT_Vector kerning;
    Err = FT_Get_Kerning(face, A_index, V_index, FT_KERNING_DEFAULT, &kerning);
    if (Err)
      return ReleaseResourceImpl(&library, &face);

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

    boost::intrusive_ptr<chaos::Texture> result = chaos::GLTools::GenTextureObject(image_description, parameters);
    ReleaseResourceImpl(&library, &face);


    return result;
#endif
  }

  virtual bool Initialize() override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path       = application->GetResourcesPath();
    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

    texture = LoadFont(0, "Hello world");
    if (texture == nullptr)
      return false;

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

  int font_index;
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


