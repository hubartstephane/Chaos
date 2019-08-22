#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUProgram.h>
#include <chaos/Application.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/MyFbxImporter.h>
#include <chaos/GPUProgramData.h>
#include <chaos/SimpleMesh.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/DrawPrimitive.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			current_material = 1 - current_material;
			return true;
		}
		if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		{
			render_material1 = nullptr;
			return true;
		}
		if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		{
			render_material2 = nullptr;
			return true;
		}
		if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		{
			mesh = nullptr;
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, glm::ivec2 size) override
	{



		float     far_plane = 1000.0f;
		glm::vec4 clear_color(0.2f, 0.2f, 0.2f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, size.x, size.y);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// XXX : the scaling is used to avoid the near plane clipping      
		static float FOV = 60.0f;
		glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);

		glm::mat4 local_to_world_matrix = glm::mat4(10.0f);

		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		int instance_cube_size = 1;

		double realtime = 0.0;

		chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
		if (clock != nullptr)
			clock->GetClockTime();

		chaos::GPUProgramProvider uniform_provider;
		uniform_provider.AddVariableValue("projection", projection_matrix);
		uniform_provider.AddVariableValue("local_to_world", local_to_world_matrix);
		uniform_provider.AddVariableValue("world_to_camera", world_to_camera_matrix);
		uniform_provider.AddVariableValue("instance_cube_size", instance_cube_size);
		uniform_provider.AddVariableValue("realtime", realtime);


		chaos::RenderParams render_params;
		render_params.instancing.instance_count = instance_cube_size * instance_cube_size * instance_cube_size;
		render_params.instancing.base_instance = 0;

		chaos::GPURenderMaterial * materials[] = { render_material1.get(), render_material2.get() };

		chaos::GPURenderMaterial * rm = materials[current_material];
		if (rm == nullptr)
			rm = materials[1 - current_material];

		if (rm != nullptr)
			mesh->Render(renderer, rm, &uniform_provider, render_params);


		return true;
	}

	virtual void Finalize() override
	{
		render_material1 = nullptr;
		render_material2 = nullptr;
		mesh = nullptr;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager == nullptr)
			return nullptr;

		// create the mesh
		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::CubeMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		// get the material
		render_material1 = resource_manager->FindRenderMaterial("mat1");

		render_material2 = resource_manager->FindRenderMaterial("mat2");

		boost::filesystem::path dir_path = chaos::JSONTools::DumpConfigFile(config);
		chaos::WinTools::ShowFile(dir_path);


		

		fps_view_controller.fps_controller.position.z = 30.0f;

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel = 1;
		hints.decorated = 0;
	}

	virtual bool Tick(double delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		return true; // refresh
	}

protected:

	int current_material = 0;

	chaos::shared_ptr<chaos::GPURenderMaterial> render_material1;
	chaos::shared_ptr<chaos::GPURenderMaterial> render_material2;

	chaos::shared_ptr<chaos::SimpleMesh> mesh;

	chaos::FPSViewInputController fps_view_controller;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 800;
	params.height = 800;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


