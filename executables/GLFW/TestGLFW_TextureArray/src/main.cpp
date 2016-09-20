#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1()
  {

  }

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_camera.GlobalToLocal();
    glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

    chaos::GLProgramData const & program_data = program_box->GetProgramData();

    glUseProgram(program_box->GetResourceID());
    program_data.SetUniform("projection",      projection);
    program_data.SetUniform("world_to_camera", world_to_camera);
    program_data.SetUniform("local_to_world",  local_to_world);

    glBindTextureUnit(0, texture->GetResourceID());
    program_data.SetUniform("material", 0);

    mesh_box->Render(program_box->GetProgramData(), nullptr, 0, 0);

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box    = nullptr;
    program_box = nullptr;
    texture     = nullptr;

    debug_display.Finalize();
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }




  boost::intrusive_ptr<chaos::Texture> LoadTextureArray(std::vector<FIBITMAP*> const & images, int max_bpp, int max_width, int max_height)
  {
    chaos::GenTextureResult result;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &result.texture_id);
    if (result.texture_id > 0)
    {
      // choose format and internal format (beware FreeImage is BGR/BGRA)
      std::pair<GLenum, GLenum> all_formats = chaos::GLTextureTools::GetTextureFormatsFromBPP(max_bpp);

      GLenum format = all_formats.first;
      GLenum internal_format = all_formats.second;

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      int level_count = chaos::GLTextureTools::GetMipmapLevelCount(max_width, max_height);
      glTextureStorage3D(result.texture_id, level_count, internal_format, max_width, max_height, images.size());

      for (size_t i = 0 ; i < images.size() ; ++i)
      {
        FIBITMAP * image = images[i];

        chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(image);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 8 * desc.pitch_size / desc.bpp);

        glTextureSubImage3D(result.texture_id, 0, 0, 0, i, desc.width, desc.height, 1, format, GL_UNSIGNED_BYTE, desc.data);
      }

      result.texture_description.type            = GL_TEXTURE_2D_ARRAY;
      result.texture_description.internal_format = internal_format;
      result.texture_description.width           = max_width;
      result.texture_description.height          = max_height;
      result.texture_description.depth           = images.size();

      return new chaos::Texture(result.texture_id, result.texture_description);
    }

    return nullptr;
  }

  boost::intrusive_ptr<chaos::Texture> LoadTextureArray(boost::filesystem::path const & resources_path)
  {
    boost::intrusive_ptr<chaos::Texture> result;

    std::vector<FIBITMAP *> images;

    // load the images
    int i = 0;
    while (true)
    {
      std::string filename = chaos::StringTools::Printf("Brick_%d.png", i + 1);

      FIBITMAP * image = chaos::ImageTools::LoadImageFromFile((resources_path / filename).string().c_str());
      if (image == nullptr)
        break;
      images.push_back(image);    
      ++i;
    }

    // at least one texture ?
    if (images.size() == 0)
      return result; // nothing loaded

    // search max size/bpp
    int max_bpp    = 0;
    int max_width  = 0;
    int max_height = 0;
    for (auto image : images)
    {
      int bpp    = (int)FreeImage_GetBPP(image);
      int width  = (int)FreeImage_GetWidth(image);
      int height = (int)FreeImage_GetHeight(image);

      max_bpp    = max(max_bpp, bpp);
      max_width  = max(max_width, width);
      max_height = max(max_height, height);
    }
    // generate a texture for that
    result = LoadTextureArray(images, max_bpp, max_width, max_height);

    // free the images
    for (auto image : images)
      FreeImage_Unload(image);
    images.clear();

    return result;
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // initialize debug font display 
    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = font_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;
    
    // load texture
    texture = LoadTextureArray(resources_path);
    if (texture == nullptr)
      return false;

    // load programs      
    program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
    if (program_box == nullptr)
      return false;

    // create meshes
    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    chaos::MultiMeshGenerator generators;    
    generators.AddGenerator(chaos::CubeMeshGenerator(b), mesh_box);

    if (!generators.GenerateMeshes())
      return false;

    // place camera
    fps_camera.fps_controller.position.y = 0.0f;
    fps_camera.fps_controller.position.z = 10.0f;

    // initial display
    debug_display.AddLine("Draw a box with a texture array : \nthe 2 textures in the array have differents BPP (24 and 32)");

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {

  }


  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    fps_camera.OnMouseButton(glfw_window, button, action, modifier);
  }

  virtual void OnMouseMove(double x, double y) override
  {
    fps_camera.OnMouseMove(glfw_window, x, y);
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GLProgram>  program_box;
  boost::intrusive_ptr<chaos::Texture>    texture;

  chaos::box3 bigger_box;
  chaos::box3 smaller_box;

  int    display_example;

  chaos::MyGLFWFpsCamera fps_camera;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width  = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


