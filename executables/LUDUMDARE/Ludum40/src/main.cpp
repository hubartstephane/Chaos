#include "Ludum40PCH.h"
#include "Ludum40SpriteManager.h"
#include "Ludum40Game.h"

// ======================================================================================

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

	static constexpr float VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & key_event) override
	{
		if (game != nullptr)
			if (game->OnKeyEvent(key_event.key, key_event.action))
				return true;
		return chaos::Window::OnKeyEventImpl(key_event);
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		float world_x = 1000.0f;
		float world_y = world_x / VIEWPORT_WANTED_ASPECT;

		if (game != nullptr)
			game->Display(render_context, glm::vec2(world_x, world_y));

		return true;
	}

	virtual void Finalize() override
	{
		game = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		// open user temp directory and dump the config file
		chaos::Application * application = chaos::Application::GetInstance();
		if (application != nullptr)
		{
			boost::filesystem::path user_temp = application->CreateUserLocalTempDirectory();
		}

		game = new Game;
		if (game == nullptr)
			return false;

		float WORLD_X = 1000.0f;
		glm::vec2 world_size = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
		if (!game->Initialize(this, chaos::JSONTools::GetAttributeStructureNode(config.default_config, "game"), world_size))
			return false;

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		game->Tick(delta_time);

		return true; // refresh
	}

protected:

	chaos::shared_ptr<Game> game;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}