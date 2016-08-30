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

    return true;

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

    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path      = resources_path / "Flatwheat-Regular.ttf";
    //boost::filesystem::path font_path = resources_path / "Flatwheat-Italic.ttf";
    
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

#define DISPLAY_FLAG(f) std::cout << #f " : " << (face->face_flags & f) << std::endl;

    DISPLAY_FLAG(FT_FACE_FLAG_SCALABLE);
    DISPLAY_FLAG(FT_FACE_FLAG_FIXED_SIZES);
    DISPLAY_FLAG(FT_FACE_FLAG_FIXED_WIDTH);
    DISPLAY_FLAG(FT_FACE_FLAG_SFNT);
    DISPLAY_FLAG(FT_FACE_FLAG_HORIZONTAL);
    DISPLAY_FLAG(FT_FACE_FLAG_VERTICAL);
    DISPLAY_FLAG(FT_FACE_FLAG_KERNING);
    DISPLAY_FLAG(FT_FACE_FLAG_FAST_GLYPHS);
    DISPLAY_FLAG(FT_FACE_FLAG_MULTIPLE_MASTERS);
    DISPLAY_FLAG(FT_FACE_FLAG_GLYPH_NAMES);
    DISPLAY_FLAG(FT_FACE_FLAG_EXTERNAL_STREAM);
    DISPLAY_FLAG(FT_FACE_FLAG_HINTER);
    DISPLAY_FLAG(FT_FACE_FLAG_CID_KEYED);
    DISPLAY_FLAG(FT_FACE_FLAG_TRICKY);
    DISPLAY_FLAG(FT_FACE_FLAG_COLOR);

    for (int i = 0 ; i < face->num_fixed_sizes ; ++i)
    {
      std::cout << "- Fixed Size   [SIZE] = " << face->available_sizes[i].size   << std::endl;
      std::cout << "  Fixed Size  [WIDTH] = " << face->available_sizes[i].width  << std::endl;
      std::cout << "  Fixed Size [HEIGHT] = " << face->available_sizes[i].height << std::endl;  
    }

#if 0
    Err = FT_Set_Char_Size(
      face,    /* handle to face object           */
      0,       /* char_width in 1/64th of points  */
      64*64,   /* char_height in 1/64th of points */
      72,     /* horizontal device resolution    */
      72 );   /* vertical device resolution      */
    if (Err)
      return false;

    //FT_Activate_Size(&face->size);


#else
    Err = FT_Set_Pixel_Sizes(face, 128, 128);
    if (Err)
      return false;
#endif
    int glyph_index = FT_Get_Char_Index(face, 'a' );

    Err = FT_Load_Glyph(
      face,          /* handle to face object */
      glyph_index,   /* glyph index           */
      FT_LOAD_DEFAULT);  /* load flags, see below */  // FT_LOAD_NO_SCALE  ???


    // ori FT_LOAD_DEFAULT

    if (Err)
      return false;

    Err = FT_Render_Glyph(
      face->glyph,   /* glyph slot  */
      FT_RENDER_MODE_NORMAL); /* render mode */
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
    std::cout << "  bbox  [xMax] = " << bbox.xMax / 64 << std::endl;
    std::cout << "  bbox  [xMin] = " << bbox.xMin / 64 << std::endl;
    std::cout << "  bbox  [yMax] = " << bbox.yMax / 64 << std::endl;
    std::cout << "  bbox  [yMin] = " << bbox.yMin / 64 << std::endl;

    //FT_Glyph_Get_CBox(face->glyph, FT_GLYPH_BBOX_UNSCALED, &bbox );
    bool has_kerning = (FT_HAS_KERNING(face) != 0);
    std::cout << "  has_kerning = " << has_kerning << std::endl;

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
      face->glyph->bitmap.width, face->glyph->bitmap.width,
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


