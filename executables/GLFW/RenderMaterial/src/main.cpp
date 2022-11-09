#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	void UpdateDebugDisplay()
	{
		debug_display.Clear();

		if (renderpass_names.size() > 0)
			debug_display.AddLine(chaos::StringTools::Printf("Renderpass [%s]", renderpass_names[current_renderpass].c_str()).c_str(), -1.0f);

		chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
		{
			size_t count = resource_manager->GetRenderMaterialCount();
			if (count != 0)
				debug_display.AddLine(chaos::StringTools::Printf("Material   [%s]", resource_manager->GetRenderMaterial(current_material)->GetName()).c_str(), -1.0f);
		}
	}

	void ChangeRenderpass(int direction)
	{
		size_t count = renderpass_names.size();
		if (count == 0)
			return;

		if (current_renderpass == 0 && direction < 0)
			current_renderpass = count - 1;
		else if (current_renderpass == count - 1 && direction > 0)
			current_renderpass = 0;
		else
			current_renderpass += direction;

		UpdateDebugDisplay();
	}

	void ChangeMaterial(int direction)
	{
		chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager == nullptr)
			return;

		size_t count = resource_manager->GetRenderMaterialCount();
		if (count == 0)
			return;

		if (current_material == 0 && direction < 0)
			current_material = count - 1;
		else if (current_material == count - 1 && direction > 0)
			current_material = 0;
		else
			current_material += direction;

		UpdateDebugDisplay();
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyPressed(GLFW_KEY_KP_ADD, GLFW_MOD_SHIFT))
		{
			ChangeRenderpass(+1);
			return true;
		}
		else if (event.IsKeyPressed(GLFW_KEY_KP_ADD))
		{
			ChangeMaterial(+1);
			return true;
		}
		else if (event.IsKeyPressed(GLFW_KEY_KP_SUBTRACT, GLFW_MOD_SHIFT))
		{
			ChangeRenderpass(-1);
			return true;
		}
		else if (event.IsKeyPressed(GLFW_KEY_KP_SUBTRACT))
		{
			ChangeMaterial(-1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
	{
		float     far_plane = 10000.0f;
		glm::vec4 clear_color(0.2f, 0.2f, 0.2f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		if (current_renderpass >= renderpass_names.size())
			return true;
		chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager == nullptr)
			return true;
		size_t count = resource_manager->GetRenderMaterialCount();
		if (count == 0)
			return true;


		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV = 60.0f;
		glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);

		glm::mat4 local_to_world_matrix = glm::mat4(1.0f);

		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		int instance_cube_size = 1;

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

		chaos::GPURenderParams render_params;
		render_params.instancing.instance_count = instance_cube_size * instance_cube_size * instance_cube_size;
		render_params.instancing.base_instance = 0;
		render_params.renderpass_name = renderpass_names[current_renderpass];

		chaos::GPURenderMaterial * rm = resource_manager->GetRenderMaterial(current_material);
		if (rm == nullptr)
			return true;
		if (mesh != nullptr)
			mesh->DisplayWithMaterial(rm, renderer, &main_uniform_provider, render_params);

		debug_display.Display(renderer, draw_params.viewport.size.x, draw_params.viewport.size.y);

		return true;
	}

	virtual void Finalize() override
	{
		debug_display.Finalize();
		mesh = nullptr;
		chaos::Window::Finalize();
	}

	bool InitializeDebugLogger()
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
		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		InitializeDebugLogger();
		UpdateDebugDisplay();

		// create the mesh
		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUCubeMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		// initialize FPS
		fps_view_controller.fps_controller.position.z = 30.0f;

		return true;
	}

	virtual void TweakHints(chaos::GLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel = 0;
		hints.decorated = 1;
	}

	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

protected:

	size_t current_material   = 0;
	size_t current_renderpass = 0;

	std::vector<std::string> renderpass_names = {"", "renderpass1", "renderpass2", "renderpass3", "renderpass4"};

	chaos::shared_ptr<chaos::GPUMesh> mesh;

	chaos::FPSViewInputController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;
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


