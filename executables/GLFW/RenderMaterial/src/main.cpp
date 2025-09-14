#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	void UpdateDebugDisplay()
	{
		imgui_user_message.Clear();

		if (renderpass_names.size() > 0)
			imgui_user_message.AddLine(chaos::StringTools::Printf("Renderpass [%s]", renderpass_names[current_renderpass].c_str()).c_str(), -1.0f);

		chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
		{
			size_t count = resource_manager->GetRenderMaterialCount();
			if (count != 0)
				imgui_user_message.AddLine(chaos::StringTools::Printf("Material   [%s]", resource_manager->GetRenderMaterial(current_material)->GetName()).c_str(), -1.0f);
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




	virtual bool EnumerateKeyActions(chaos::KeyActionEnumerator & in_action_enumerator) override
	{
		if (in_action_enumerator(RequestKeyPressed(chaos::KeyboardButton::KP_ADD).RequireModifiers(chaos::KeyModifier::Shift) , "Next RenderPass", [this]()
		{
			ChangeRenderpass(+1);
		}))
		{
			return true;
		}

		if (in_action_enumerator(RequestKeyPressed(chaos::KeyboardButton::KP_ADD), "Next Material", [this]()
		{
			ChangeMaterial(+1);
		}))
		{
			return true;
		}

		if (in_action_enumerator(RequestKeyPressed(chaos::KeyboardButton::KP_SUBTRACT).RequireModifiers(chaos::KeyModifier::Shift), "Previous RenderPass", [this]()
		{
			ChangeRenderpass(-1);
		}))
		{
			return true;
		}

		if (in_action_enumerator(RequestKeyPressed(chaos::KeyboardButton::KP_SUBTRACT), "Previous Material", [this]()
		{
			ChangeMaterial(-1);
		}))
		{
			return true;
		}

		return chaos::Window::EnumerateKeyActions(in_action_enumerator);
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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
			mesh->DisplayWithMaterial(rm, render_context, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		mesh = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		UpdateDebugDisplay();

		// create the mesh
		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUBoxMeshGenerator(b).GenerateMesh(GetGPUDevice());
		if (mesh == nullptr)
			return false;

		// initialize FPS
		fps_view_controller.fps_view.position.z = 30.0f;

		return true;
	}

	virtual bool TraverseInputEventReceiverHierarchy(chaos::TraverseInputEventReceiverHierarchyFunction event_func) override
	{
		if (fps_view_controller.TraverseInputEventReceiverHierarchy(event_func))
			return true;
		return chaos::Window::TraverseInputEventReceiverHierarchy(event_func);
	}

	virtual void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			imgui_user_message.OnDrawImGuiContent(this);
		});
	}

protected:

	size_t current_material   = 0;
	size_t current_renderpass = 0;

	std::vector<std::string> renderpass_names = {"", "renderpass1", "renderpass2", "renderpass3", "renderpass4"};

	chaos::shared_ptr<chaos::GPUMesh> mesh;

	chaos::FPSViewController fps_view_controller;

	chaos::ImGuiUserMessageObject imgui_user_message;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}