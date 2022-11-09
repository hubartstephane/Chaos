#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
	{

		for (int pass = 0; pass < 2; ++pass)
		{
			glm::ivec2 size = (pass == 0) ?
				framebuffer->GetSize():
				draw_params.viewport.size;

			if (pass == 0)
				renderer->PushFramebufferRenderContext(framebuffer.get(), true);

			float far_plane = 1000.0f;
			glm::vec4 clear_color = (pass == 0)?
				glm::vec4(0.0f, 0.0f, 0.0f, 0.0f):
				glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
			glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);


			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			// XXX : the scaling is used to avoid the near plane clipping
			static float FOV = 60.0f;
			glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);

			glm::mat4 local_to_world_matrix = glm::mat4(10.0f);

			glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

			int instance_cube_size = 5;

			double realtime = 0.0;

			chaos::Clock * clock = chaos::WindowApplication::GetMainClockInstance();
			if (clock != nullptr)
				clock->GetClockTime();

			chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariable("projection", projection_matrix);
			main_uniform_provider.AddVariable("local_to_world", local_to_world_matrix);
			main_uniform_provider.AddVariable("world_to_camera", world_to_camera_matrix);
			main_uniform_provider.AddVariable("instance_cube_size", instance_cube_size);
			main_uniform_provider.AddVariable("realtime", realtime);
			main_uniform_provider.AddVariable("pass_value", (float)pass);

			if (pass == 1)
			{
				chaos::GPUFramebufferAttachmentInfo const * attachment = framebuffer->GetColorAttachment(0);
				if (attachment != nullptr)
				{
					chaos::GPUTexture * texture = attachment->texture.get();
					if (texture != nullptr)
					{
						main_uniform_provider.AddVariable("scene_texture", texture);
					}
				}
			}

			chaos::GPURenderParams render_params;
			render_params.instancing.instance_count = instance_cube_size * instance_cube_size * instance_cube_size;
			render_params.instancing.base_instance = 0;

			mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);

			if (pass == 0)
				renderer->PopFramebufferRenderContext();
		}
		return true;
	}

	virtual void Finalize() override
	{
		framebuffer = nullptr;
		program = nullptr;
		mesh = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();

		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUCubeMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		// create shader
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader_cube.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / "vertex_shader.txt");

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// create the framebuffer

		static bool with_depth_stencil = true;

		chaos::GPUFramebufferGenerator framebuffer_generator;
		framebuffer_generator.AddColorAttachment(0, chaos::PixelFormat::BGRA, glm::ivec2(0, 0), "scene");
		if (with_depth_stencil)
			framebuffer_generator.AddDepthStencilAttachment(glm::ivec2(0, 0));
		framebuffer = framebuffer_generator.GenerateFramebuffer(glm::ivec2(1024, 1024));
		if (framebuffer == nullptr)
			return false;
		if (!framebuffer->CheckCompletionStatus())
			return false;

		// set camera position
		fps_view_controller.fps_controller.position.z = 50.0f;

		return true;
	}

	virtual void TweakHints(chaos::GLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel = 1;
		hints.decorated = 0;
	}

	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		return true; // refresh
	}

protected:

	chaos::shared_ptr<chaos::GPUProgram> program;

	chaos::shared_ptr<chaos::GPUFramebuffer> framebuffer;

	chaos::shared_ptr<chaos::GPUMesh> mesh;

	chaos::FPSViewInputController fps_view_controller;
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 800;
	params.height = 800;
	params.monitor_index = 0;

	chaos::GLFWWindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}



