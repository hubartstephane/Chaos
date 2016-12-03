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
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/MathTools.h>

#include <chaos/SpriteTextParser.h>
#include <chaos/SpriteManager.h>


#if 0
int operator ()(std::string const & a, std::string const & b) const
{
	char const * s1 = a.c_str();
	char const * s2 = b.c_str();

	if (s1 == nullptr)
		return (s2 == nullptr)? 0 : -1;
	if (s2 == nullptr)
		return +1;

	int  i  = 0;
	char c1 = toupper(s1[i]);
	char c2 = toupper(s2[i]);			
	while (c1 != 0 && c2 != 0)
	{
		if (c1 != c2)
			return (c1 > c2)? +1 : -1;				
		++i;				
		c1 = toupper(s1[i]);
		c2 = toupper(s2[i]);
	}

	if (c1 == c2)
		return 0;
	return (c1 > c2)? +1 : -1;	
}
#endif






// --------------------------------------------------------------------

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1()
  {


	  /*

	  std::string ss;
	  std::string s2;

	  auto pp = ss.c_str();
	  auto p2 = s2.c_str();

	  std::map<std::string, int, chaos::StringTools::ci_less> m;
	  m["toto"] = 3;
	  m["atoto"] = 34;
	  m["ztoto"] = 35;

	  int u = m["TOTO"];


	  char const * str1 = "hello, world!";
	  char const * str2 = "HELLO, WORLD!";

	  if (boost::iequals(str1, str2))
	  {
		  int i = 0;
		  ++i;
	  }
	  */

  }

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

                              // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_camera.GlobalToLocal();
    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    sprite_manager.Display();

    return true;
  }

  virtual void Finalize() override
  {
    sprite_manager.Finalize();
    atlas.Clear();
  }

  bool LoadAtlas(boost::filesystem::path const & resources_path)
  {
    return atlas.LoadAtlas(resources_path / "MyAtlas.json");
  }

  bool InitializeSpriteManager()
  {
    chaos::SpriteManagerInitParams params;
    params.atlas = &atlas;

    if (!sprite_manager.Initialize(params))
      return false;

    int SPRITE_COUNT = 100;

    // add characters
    auto const & character_sets = atlas.GetCharacterSets();
    if (character_sets.size() > 0)
    {
      chaos::BitmapAtlas::CharacterSet const * character_set = character_sets.at(0).get();

      size_t element_count = character_set->elements.size();
      if (element_count > 0)
      {
        for (int i = 0; i < SPRITE_COUNT; ++i)
        {
          chaos::BitmapAtlas::CharacterEntry const * entry = &character_set->elements[rand() % element_count];

          glm::vec2 position = 2.0f * chaos::MathTools::RandVec2() - glm::vec2(1.0f);
          glm::vec2 size = 0.3f * glm::vec2(chaos::MathTools::RandFloat());
          glm::vec3 color = chaos::MathTools::RandVec3();

          sprite_manager.AddSprite(entry, position, size, chaos::SpriteManager::HOTPOINT_CENTER, color);
        }

      }
    }

    // add bitmap
    auto const & bitmap_sets = atlas.GetBitmapSets();
    if (bitmap_sets.size() > 0)
    {
      chaos::BitmapAtlas::BitmapSet const * bitmap_set = bitmap_sets.at(0).get();

      size_t element_count = bitmap_set->elements.size();
      if (element_count > 0)
      {        
        for (int i = 0; i < SPRITE_COUNT; ++i)
        {
          chaos::BitmapAtlas::BitmapEntry const * entry = &bitmap_set->elements[rand() % element_count];

          glm::vec2 position = 2.0f * chaos::MathTools::RandVec2() - glm::vec2(1.0f);
          glm::vec2 size = 0.3f * glm::vec2(chaos::MathTools::RandFloat());

          sprite_manager.AddSprite(entry, position, size, chaos::SpriteManager::HOTPOINT_CENTER);
        }

      }
    }


    return true;

	  /*
    TextParser parser(atlas);
    ParseTextParams params;
    params.max_text_width = 300;

    bool result = parser.ParseText("Hello worl[d]\n", params);



    return result;
	*/


  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // initialize the atlas
    if (!LoadAtlas(resources_path))
      return false;

    // initialize the sprite manager
    if (!InitializeSpriteManager())
      return false;

    // place camera
    fps_camera.fps_controller.position.y = 0.0f;
    fps_camera.fps_controller.position.z = 10.0f;

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

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

  // the sprite manager
  chaos::SpriteManager sprite_manager;
  // the atlas
  chaos::BitmapAtlas::TextureArrayAtlas atlas;
  // the camera
  chaos::MyGLFWFpsCamera fps_camera;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


