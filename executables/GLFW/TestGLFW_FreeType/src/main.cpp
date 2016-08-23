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

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


#if 1

    GLuint program_id = program->GetResourceID();
       
    glValidateProgram(program_id);
    GLint validation = chaos::GLTools::CheckProgramStatus(program_id, GL_VALIDATE_STATUS, "Program validation failure : %s");
    
    glUseProgram(program_id);
      
    static float FOV =  60.0f;
    static float Z   = -20.0f;
    glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)width, (float)height, 1.0f, far_plane);
    glm::mat4 local_to_world  = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
    glm::mat4 world_to_camera = glm::translate(glm::vec3(0.0f, 0.0f, Z));

    chaos::GLProgramData const & program_data = program->GetProgramData();

    program_data.SetUniform("projection",      projection);
    program_data.SetUniform("local_to_world",  local_to_world);
    program_data.SetUniform("world_to_camera", world_to_camera);

    glBindTextureUnit(0, texture->GetResourceID());
    program_data.SetUniform("material", 0);

    mesh->Render(program_data, nullptr, 0, 0);
#endif
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

    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
    boost::filesystem::path font_path      = resources_path / "Flatwheat-Italic.ttf";
    
    FT_Face face;
    Err = FT_New_Face(library, font_path.string().c_str(), 0, &face);
    if (Err)
      return false;

    for (int i = 0 ; i < face->num_charmaps ; ++i)
    {



    }


    Err = FT_Set_Char_Size(
      face,    /* handle to face object           */
      0,       /* char_width in 1/64th of points  */
      16*64,   /* char_height in 1/64th of points */
      300,     /* horizontal device resolution    */
      300 );   /* vertical device resolution      */
    if (Err)
      return false;
#if 0

    Err = FT_Set_Pixel_Sizes(face, 256, 256);
    if (Err)
      return false;
#endif
    int glyph_index = FT_Get_Char_Index(face, 'a' );

    Err = FT_Load_Glyph(
      face,          /* handle to face object */
      glyph_index,   /* glyph index           */
      FT_LOAD_DEFAULT);  /* load flags, see below */

    if (Err)
      return false;

    Err = FT_Render_Glyph(
      face->glyph,   /* glyph slot  */
      FT_RENDER_MODE_NORMAL); /* render mode */
    if (Err)
      return false;



#if 1
    chaos::ImageDescription image_description;
    image_description.data         = face->glyph->bitmap.buffer;
    image_description.width        = face->glyph->bitmap.width;
    image_description.height       = face->glyph->bitmap.width;
    image_description.bpp          = 8;
    image_description.padding_size = 0;
    image_description.line_size    = image_description.width * image_description.bpp / 8;
    image_description.pitch_size   = image_description.line_size + image_description.padding_size;

#else

    char * bb = new char [ 256 * 256];
    for (int i = 0; i < 256 * 256; ++i)
      bb[i] = (char)(i & 0xFF);

    chaos::ImageDescription image_description;
    image_description.data = bb;
    image_description.width = 256;
    image_description.height = 256;
    image_description.bpp = 8;
    image_description.padding_size = 0;
    image_description.line_size = image_description.width * image_description.bpp / 8;
    image_description.pitch_size = image_description.line_size + image_description.padding_size;

#endif


    texture = chaos::GLTools::GenTextureObject(image_description);




    glyph_index = glyph_index;



#if 0



    boost::filesystem::path resources_path       = application->GetApplicationPath() / "resources";
    boost::filesystem::path image_path           = (RECTANGLE_TEXTURE)?
      resources_path / "opengl_logo_rectangle.png":
      resources_path / "opengl_logo.png";
    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader_face.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

    FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(image_path.string().c_str());
    if (image == nullptr)
      return false;

    texture = chaos::GLTools::GenTextureObject(image);
    if (texture == nullptr)
      return false;

    FreeImage_Unload(image);
#endif

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


