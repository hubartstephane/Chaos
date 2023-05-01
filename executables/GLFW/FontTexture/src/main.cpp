#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(1.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		debug_display.Display(renderer, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));
		return true;
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (action == GLFW_PRESS)
		{
			if (button == 1)
				debug_display.Clear();
			else
				debug_display.AddLine(chaos::StringTools::Printf("LINE FROM CLICKED [%d]", click_count++).c_str(), 5.0f);
			return true;
		}
		return false;
	}

	virtual void Finalize() override
	{
		debug_display.Finalize();
		chaos::Window::Finalize();
	}

	virtual bool DoTick(float delta_time) override
	{
		return debug_display.Tick(delta_time);
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::Application* application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path image_path = resources_path / "font.png";

		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path = image_path;
		debug_params.font_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width = 20;
		debug_params.spacing = glm::ivec2(0, 0);
		debug_params.crop_texture = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		debug_display.AddLine("line1 gdjkhfghhbb xxxxx", 2.0f);
		debug_display.AddLine("line2 yyyyyyyy xxxxx");
		debug_display.AddLine("line3 yyyyyyyy xxxxx", 5.0f);
		debug_display.AddLine("line4 yyyyyyyy xxxxx");
		debug_display.AddLine("line5 yyyyyyyy xxxxx", 0.0f);
		debug_display.AddLine("line6 yyyyyyyy xxxxx");

		debug_display.AddLine(debug_display.GetFontCharacters().c_str());

		return true;
	}

protected:

	chaos::GLDebugOnScreenDisplay debug_display;

	int click_count = 0;
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