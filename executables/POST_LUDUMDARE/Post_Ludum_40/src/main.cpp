#include <chaos/StandardHeaders.h> 
#include <chaos/CollisionFramework.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>
#include "game.h"


// ======================================================================================


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

	static float const VIEWPORT_WANTED_ASPECT;

protected:

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (game != nullptr)
			if (game->OnKeyEvent(key, action))
				return;
	}

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		float aspect = (16.0f / 9.0f) * 0.0f;
		chaos::GLTools::SetViewportWithAspect(size, VIEWPORT_WANTED_ASPECT);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		float world_x = 1000.0f;
		float world_y = world_x / VIEWPORT_WANTED_ASPECT;
		
		if (game != nullptr)
			game->Display(glm::vec2(world_x, world_y));

		return true;
	}

	virtual void Finalize() override
	{
		game = nullptr;

		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;

		debug_display.Finalize();

		sound_manager = nullptr;
	}

	virtual bool Initialize(nlohmann::json const & configuration) override
	{   
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path image_path     = resources_path / "font.png";

		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path               = image_path;
		debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line   = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width            = 20;
		debug_params.spacing                    = glm::ivec2( 0, 0);
		debug_params.crop_texture               = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		sound_manager = new chaos::SoundManager;
		if (sound_manager == nullptr)
			return false;

		game = new Game;
		if (game == nullptr)
			return false;

		float WORLD_X = 1000.0f;
		glm::vec2 world_size = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
		if (!game->Initialize(glfw_window, world_size, resources_path))
			return false;

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

#if !_DEBUG
		hints.toplevel  = 1;
#endif
		hints.decorated = 1;
	}

	virtual bool Tick(double delta_time) override
	{
		game->Tick(delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

protected:

	boost::intrusive_ptr<Game> game;

	boost::intrusive_ptr<chaos::SoundManager> sound_manager;

	boost::intrusive_ptr<chaos::GLProgram>  program;
	boost::intrusive_ptr<chaos::SimpleMesh> mesh;
	boost::intrusive_ptr<chaos::Texture>    texture;

	chaos::GLDebugOnScreenDisplay debug_display;
};

float const MyGLFWWindowOpenGLTest1::VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::box2 b = chaos::box2(glm::vec2(100.0f, 0.0), glm::vec2(50.0f, 50.0f));
  chaos::sphere2 s = chaos::sphere2(glm::vec2(0.0f, 0.0f), 40.0f);

  bool col = chaos::Collide(b, s);













	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor       = nullptr;
	params.width         = 800;
	params.height        = 800;
	params.monitor_index = 0;
	chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	chaos::Application::Finalize();

	return 0;
}


