#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		if (event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			ChangeSkyBox(skybox_index + 1);
			return true;
		}
		else if (event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			ChangeSkyBox(skybox_index - 1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	void ChangeSkyBox(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateSkyBox(index);
		if (new_texture != nullptr)
		{
			skybox_index = index;
			texture = new_texture;
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateSkyBox(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> result;

		boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();

		chaos::SkyBoxImages skybox;
		if (index == 0)
		{
			skybox = chaos::SkyBoxTools::LoadSingleSkyBox((resources_path / "violentdays_large.jpg"));
		}
		else if (index == 1)
		{
			skybox = chaos::SkyBoxTools::LoadSingleSkyBox((resources_path / "originalcubecross.png"));
		}
		else if (index == 2)
		{
			boost::filesystem::path p = resources_path / "Maskonaive";

			boost::filesystem::path left_image = p / "negx.jpg";
			boost::filesystem::path front_image = p / "posz.jpg";
			boost::filesystem::path right_image = p / "posx.jpg";
			boost::filesystem::path back_image = p / "negz.jpg";
			boost::filesystem::path top_image = p / "posy.jpg";
			boost::filesystem::path bottom_image = p / "negy.jpg";

			skybox = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 3)
		{
			boost::filesystem::path p = resources_path / "skybox";

			boost::filesystem::path left_image = p / "nx.jpg";
			boost::filesystem::path front_image = p / "pz.jpg";
			boost::filesystem::path right_image = p / "px.jpg";
			boost::filesystem::path back_image = p / "nz.jpg";
			boost::filesystem::path top_image = p / "py.jpg";
			boost::filesystem::path bottom_image = p / "ny.jpg";

			skybox = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 4)
		{
			boost::filesystem::path p = resources_path / "MilkyWay";

			boost::filesystem::path left_image = p / "dark-s_nx.jpg";
			boost::filesystem::path front_image = p / "dark-s_pz.jpg";
			boost::filesystem::path right_image = p / "dark-s_px.jpg";
			boost::filesystem::path back_image = p / "dark-s_nz.jpg";
			boost::filesystem::path top_image = p / "dark-s_py.jpg";
			boost::filesystem::path bottom_image = p / "dark-s_ny.jpg";

			skybox = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 5)
		{
			skybox = chaos::SkyBoxTools::LoadSingleSkyBox((resources_path / "space.png"));
		}

		if (!skybox.IsEmpty())
			return chaos::GPUTextureLoader().GenTextureObject(&skybox);

		return nullptr;
	}

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		if (query->IsEnded())
		{
			bool available = query->IsResultAvailable();

			GLint result = query->GetResult(true);

			GLint64 result64 = query->GetResult64(true);
		}

		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV = 60.0f;
		glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 local_to_world_matrix = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("projection", projection_matrix);
		main_uniform_provider.AddVariable("local_to_world", local_to_world_matrix);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera_matrix);
		main_uniform_provider.AddTexture("material", texture);

		// XXX : conditional rendering may fail if all conditions are not meet
		//       the rendering will be processed has normal
		glEnable(GL_STENCIL_TEST);

		GLint ref = 1;
		GLuint mask = 0xFF;
		glStencilFunc(GL_ALWAYS, ref, mask); // always pass stencil

		GLenum stencil_fail = GL_KEEP;
		GLenum depth_fail = GL_KEEP;
		GLenum depth_pass = GL_REPLACE;
		glStencilOp(stencil_fail, depth_fail, depth_pass); // as soon as a pixel is rendered, REPLACE the stencil buffer by REF = 1

		// XXX : the stencil is here to ensure that the debug strings is not erased by the sky box
		//       (debug string needs to be rendered first so it can use the conditional rendering from previous frame)
		query->BeginConditionalRendering(true, false);
		debug_display.Display(renderer, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));
		query->EndConditionalRendering();

		// XXX : render the skybox. Use previous frame query for conditinal rendering
		ref = 0;
		mask = 0xFF;
		glStencilFunc(GL_EQUAL, ref, mask); // stencil must be 0 to render a pixel (debug string is not on the screen)

		stencil_fail = GL_KEEP;
		depth_fail = GL_KEEP;
		depth_pass = GL_KEEP;
		glStencilOp(stencil_fail, depth_fail, depth_pass); // do not modify the stencil anymore

		chaos::GPURenderParams render_params;

		query->BeginQuery();
		mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);
		query->EndQuery();

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh = nullptr;
		texture = nullptr;
		query = nullptr;

		debug_display.Finalize();
		chaos::Window::Finalize();
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

		debug_display.AddLine("Press +/- to change skybox");
		debug_display.AddLine("If the skybox has no pixel on screen, ");
		debug_display.AddLine("this text will disappear (conditional rendering)");

		texture = GenerateSkyBox(0);
		if (texture == nullptr)
			return false;

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader_cube.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / "vertex_shader.txt");

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUBoxMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		query = new chaos::GLSamplesPassedQuery(this);
		if (query == nullptr)
			return false;

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_RELEASE)
		{
			debug_display.AddLine("HelloWorld");
			return true;
		}
		return false;
	}

protected:

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	chaos::shared_ptr<chaos::GPUQuery> query;

	chaos::FPSViewController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;

	int skybox_index = 0;
};

int main(int argc, char** argv, char** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 1200;
	create_params.height = 700;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}


