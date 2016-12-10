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
#include <chaos/GLProgramUniformProvider.h>
 
class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() : 
    mipmap_level(0),
    texture_index(0){}

protected:

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
    {
      ChangeTexture(texture_index + 1);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      ChangeTexture(texture_index - 1);
    }
  }

  void ChangeTexture(int index)
  {
    boost::intrusive_ptr<chaos::Texture> new_texture = GenerateTexture(index);
    if (new_texture != nullptr)
    {
      texture_index = index;
      texture = new_texture;
    }
  }

  boost::intrusive_ptr<chaos::Texture> GenerateTexture(int index)
  {
    boost::intrusive_ptr<chaos::Texture> result;

    if (index == 0)
    {
      // create a gray scale texture : show order of lines in memory
      char * image_buffer = new char[4 * 512 * 512];

      chaos::ImageDescription image_desc = chaos::ImageDescription(image_buffer, 512, 512, 32, 0);

      for (int i = 0; i < 512; ++i)
      {
        for (int j = 0; j < 512; ++j)
        {
          image_buffer[4 * j + 0 + (i * image_desc.pitch_size)] = i;
          image_buffer[4 * j + 1 + (i * image_desc.pitch_size)] = i;
          image_buffer[4 * j + 2 + (i * image_desc.pitch_size)] = i;
          image_buffer[4 * j + 3 + (i * image_desc.pitch_size)] = i;
        }
      }
      result = chaos::GLTextureTools::GenTextureObject(image_desc);
      delete[](image_buffer);
    }

    if (index == 1)
    {
      result = chaos::GLTextureTools::GenTextureObject(512, 512, [](chaos::ImageDescription const & desc, chaos::PixelRGB * buffer)
      {
        for (int i = 0; i < desc.height; ++i)
        {
          for (int j = 0; j < desc.width; ++j)
          {
            buffer[j + i * desc.width].R = (unsigned char)i;
            buffer[j + i * desc.width].G = 0;
            buffer[j + i * desc.width].B = 0;
          }
        }
      });
    }

    if (index >= 2 && index <= 7)
    {
      chaos::Application * application = chaos::Application::GetInstance();
      if (application == nullptr)
        return false;
      boost::filesystem::path resources_path = application->GetResourcesPath();
      boost::filesystem::path image_path;

      if (index == 2)
        image_path = resources_path / "opengl_logo.gif";
      else if (index == 3)
        image_path = resources_path / "opengl_logo.png";
      else if (index == 4)
        image_path = resources_path / "opengl_logo_rectangle.png";
      else if (index == 5)
        image_path = resources_path / "icons-animation.gif";
      else if (index == 6)
        image_path = resources_path / "grayscale.png";
      else if (index == 7)
        image_path = resources_path / "grayscale.gif";
	  
      result = chaos::GLTextureTools::GenTextureObject(image_path.string().c_str());
    }

    return result;
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    if (button == 0 && action == GLFW_RELEASE)
    {
      chaos::TextureDescription desc = texture->GetTextureDescription();

      int max_mipmap = chaos::GLTextureTools::GetMipmapLevelCount(desc.width, desc.height);

      mipmap_level = (mipmap_level + 1) % max_mipmap;
      glTextureParameteri(texture->GetResourceID(), GL_TEXTURE_BASE_LEVEL, mipmap_level); //GL_TEXTURE_MAX_LEVEL
    }
  }

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(program->GetResourceID());

    chaos::GLProgramData const & program_data = program->GetProgramData();

    chaos::GLProgramUniformProvider uniform_provider;
    uniform_provider.AddTexture("material", texture);
    program_data.BindUniforms(&uniform_provider);

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

    boost::filesystem::path resources_path       = application->GetResourcesPath();
    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";


    texture = GenerateTexture(texture_index);
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

  int mipmap_level;
  int texture_index;
};



int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    
  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


