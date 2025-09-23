#include "chaos/Chaos.h"

using namespace chaos;

class Object3D : public Object
{
public:

	void Display(GPUProgram * program, GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams & render_params)
	{
		glm::mat4x4 local_to_world = transform.GetLocalToParent();

		GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("local_to_world", local_to_world);

		if (selected)
		{
			glm::vec3 selected_color = {0.0f, 0.0f, 0.0f};
			glm::vec3 selected_emissive_color = { 1.0f, 0.0f, 0.0f };

			main_uniform_provider.AddVariable("color", selected_color);
			main_uniform_provider.AddVariable("emissive_color", selected_emissive_color);
		}
		else
		{
			main_uniform_provider.AddVariable("color", color);
			main_uniform_provider.AddVariable("emissive_color", emissive_color);
		}

		mesh->DisplayWithProgram(program, render_context, &main_uniform_provider, render_params);
	}

	void SetSelected(bool in_selected)
	{
		selected = in_selected;
	}

public:

	std::string name;

	SceneTransform<float, 3> transform;

	glm::vec3 color = { 0.0f, 0.0f, 1.0f };

	glm::vec3 emissive_color = { 0.0f, 0.0f, 0.0f };

	shared_ptr<GPUMesh> mesh;

	bool selected = false;
};


class WindowOpenGLTest : public Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, Window);

protected:

	virtual bool OnDraw(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, WindowDrawParams const& draw_params) override
	{
		float fov = 60.0f;
		float far_plane = 10000.0f;
		float near_plane = 20.0f;

		glm::vec4 clear_color(0.4f, 0.4f, 0.4f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (program != nullptr)
		{
			// compute matrices
			glm::mat4x4 projection = glm::perspectiveFov(fov * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), near_plane, far_plane);
			glm::mat4x4 world_to_camera = fps_view_controller.GlobalToLocal();

			// prepare unforms
			GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariable("projection", projection);
			main_uniform_provider.AddVariable("world_to_camera", world_to_camera);
			if (light != nullptr)
				main_uniform_provider.AddVariable("light_position", light->transform.position);

			// display objects
			GPURenderParams render_params;

			for (shared_ptr<Object3D> const& object : objects)
				object->Display(program.get(), render_context, &main_uniform_provider, render_params);
		}

		return true;
	}

	boost::filesystem::path const & GetResourcesPath() const
	{
		WindowApplication* application = Application::GetInstance();
		return application->GetResourcesPath();
	}

	virtual bool OnInitialize(JSONReadConfiguration config) override
	{
		// super
		if (!Window::OnInitialize(config))
			return false;

		// create light mesh
		CreateLight();

		// create all the meshes
		CreateMeshes();

		// select first object
		selected_object_index = 0;
		if (selected_object_index < objects.size())
			objects[selected_object_index]->SetSelected(true);

		// generate the program
		boost::filesystem::path const& resources_path = GetResourcesPath();

		GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(ShaderType::FRAGMENT, resources_path / "pixel_shader.txt");
		program_generator.AddShaderSourceFile(ShaderType::VERTEX, resources_path / "vertex_shader.txt");
		program = program_generator.GenProgramObject();

		// update the camera speed
		fps_view_controller.config.back_speed = CAMERA_SPEED;
		fps_view_controller.config.down_speed = CAMERA_SPEED;
		fps_view_controller.config.up_speed = CAMERA_SPEED;
		fps_view_controller.config.forward_speed = CAMERA_SPEED;
		fps_view_controller.config.strafe_speed = CAMERA_SPEED;

		fps_view_controller.input_config.yaw_left_button   = KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.yaw_right_button  = KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.pitch_up_button   = KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.pitch_down_button = KeyboardButton::UNKNOWN;

		fps_view_controller.fps_view.position.z = 1000.0f;

		return true;
	}

	Object3D * CreateObject3D(GPUMesh* mesh, glm::vec3 const& position, glm::vec3 const & scale, glm::quat const & rotation, glm::vec3 const& color, glm::vec3 const & emissive_color, char const * name)
	{
		Object3D* result = new Object3D;
		if (result == nullptr)
			return result;

		result->mesh               = mesh;
		result->transform.position = position;
		result->transform.scale    = scale;
		result->transform.rotator  = rotation;
		result->color              = color;
		result->emissive_color     = emissive_color;
		result->name               = name;
		objects.push_back(result);

		return result;
	}

	bool CreateLight()
	{
		sphere3 s = sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);

		shared_ptr<GPUMesh> gpu_mesh = GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 10).GenerateMesh(GetGPUDevice());
		meshes.push_back(gpu_mesh);

		glm::vec3 position       = { -200.0f, 200.0f, 500.0f };
		glm::vec3 scale          = { 1.0f, 1.0f, 1.0f };
		glm::quat rotation       = { 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 color          = { 0.0f, 0.0f, 0.0f };
		glm::vec3 emissive_color = { 1.0f, 1.0f, 1.0f };

		light = CreateObject3D(gpu_mesh.get(), position, scale, rotation, color, emissive_color, "light");
		if (light == nullptr)
			return false;
		return true;
	}

	bool CreateMeshes()
	{
		GPUMultiMeshGenerator generator;

		shared_ptr<GPUMesh> quad_mesh;
		shared_ptr<GPUMesh> circle_mesh;
		shared_ptr<GPUMesh> box3_mesh;
		shared_ptr<GPUMesh> box3_wireframe_mesh;
		shared_ptr<GPUMesh> sphere_mesh;

		box2 quad;
		quad.position  = { 0.0f, 0.0f };
		quad.half_size = { 50.0f, 50.0f };
		generator.AddGenerator(new GPUQuadMeshGenerator(quad), quad_mesh);

		sphere2 circle;
		circle.position = { 0.0f, 0.0f };
		circle.radius   = 50.0f;
		generator.AddGenerator(new GPUCircleMeshGenerator(circle), circle_mesh);

		triangle3 triangle;
		triangle.a = {0.0f, 100.0f, 0.0f};
		triangle.b = {-100.0f, -100.0f, 0.0f};
		triangle.c = {+100.0f, -100.0f, 0.0f};
		generator.AddGenerator(new GPUTriangleMeshGenerator(triangle), quad_mesh);

		box3 box;
		box.position = { 0.0f, 0.0f, 0.0f };
		box.half_size = { 50.0f, 50.0f, 50.0f };
		generator.AddGenerator(new GPUBoxMeshGenerator(box), box3_mesh);
		generator.AddGenerator(new GPUWireframeBoxMeshGenerator(box), box3_wireframe_mesh);

		sphere3 sphere;
		sphere.position = { 0.0f, 0.0f, 0.0f };
		sphere.radius = 50.0f;
		generator.AddGenerator(new GPUSphereMeshGenerator(sphere), sphere_mesh);

		if (!generator.GenerateMeshes(GetGPUDevice()))
			return false;

		glm::vec3 primitive_position = { 0.0f, 0.0f, 0.0f };
		auto AddObjectAndMesh = [this, &primitive_position](shared_ptr<GPUMesh> const & mesh, char const * name, bool wireframe)
		{
			meshes.push_back(mesh);

			glm::vec3 scale          = { 1.0f, 1.0f, 1.0f};
			glm::quat rotation       = { 1.0f, 0.0f, 0.0f, 0.0f };
			glm::vec3 color          = wireframe ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 emissive_color = wireframe ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f);
			CreateObject3D(mesh.get(), primitive_position, scale, rotation, color, emissive_color,  name);
		
			primitive_position.x += 200.0f;
		};

		AddObjectAndMesh(quad_mesh, "quad", false);
		AddObjectAndMesh(circle_mesh, "circle", false);
		AddObjectAndMesh(box3_mesh, "box", false);
		AddObjectAndMesh(box3_wireframe_mesh, "box wireframe", true);
		AddObjectAndMesh(sphere_mesh, "sphere", false);

		return true;
	}

	virtual void OnDrawImGuiContent() override
	{	
		ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			if (ImGui::BeginTable("objects", 2, ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Fixed Width", ImGuiTableColumnFlags_WidthFixed, 100.0f);

				for (shared_ptr<Object3D> const& object : objects)
				{
					size_t current_index = (&object - &objects[0]);
					bool is_selected = (selected_object_index == current_index);

					ImGui::PushID(object.get());
					
					ImGui::TableNextColumn();
					if (ImGui::Selectable(object->name.c_str(), &is_selected))
					{
						objects[selected_object_index]->SetSelected(false);
						selected_object_index = current_index;
						objects[selected_object_index]->SetSelected(true);
					}

					ImGui::TableNextColumn();
					DrawImGuiVariable(object->transform.position);

					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		});
		Window::OnDrawImGuiContent();
	}

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator & in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		size_t object_count = objects.size();

		bool enabled = object_count > 0;

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(KeyboardButton::KP_ADD), "Next Object", enabled, [&]()
		{
			objects[selected_object_index]->SetSelected(false);

			selected_object_index = (selected_object_index == object_count - 1)?
				0:
				selected_object_index + 1;

			objects[selected_object_index]->SetSelected(true);
		
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(KeyboardButton::KP_SUBTRACT), "Previous Object", enabled, [&]()
		{
			objects[selected_object_index]->SetSelected(false);

			selected_object_index = (selected_object_index == 0) ?
				object_count - 1 :
				selected_object_index - 1;

			objects[selected_object_index]->SetSelected(true);
		
		}))
		{
			return true;
		}


		return false;
	}

	virtual bool DoTick(float delta_time) override
	{
		// move objects
		size_t object_count = objects.size();
		if (object_count > 0)
		{
			auto MoveObject = [this, delta_time](KeyboardButton button, size_t component_index, float direction)
			{
				if (KeyState const* state = KeyboardAndMouseState::GetInstance()->GetKeyState(button))
				{
					if (state->IsDown())
					{
						Object3D* selected_object = objects[selected_object_index].get();
						selected_object->transform.position[component_index] += direction * OBJECT_SPEED * delta_time;
					}
				}
			};

			MoveObject(KeyboardButton::KP_4, 0, -1.0f);
			MoveObject(KeyboardButton::KP_6, 0, +1.0f);
			MoveObject(KeyboardButton::KP_8, 2, -1.0f);
			MoveObject(KeyboardButton::KP_2, 2, +1.0f);
			MoveObject(KeyboardButton::KP_9, 1, +1.0f);
			MoveObject(KeyboardButton::KP_3, 1, -1.0f);
		}
		return Window::DoTick(delta_time);
	}

	virtual bool TraverseInputEventReceiverHierarchy(chaos::InputEventReceiverHierarchyTraverser & in_traverser) override
	{
		if (in_traverser.Traverse(&fps_view_controller))
			return true;
		return chaos::Window::TraverseInputEventReceiverHierarchy(in_traverser);
	}

protected:

	static constexpr float CAMERA_SPEED = 500.0f;

	static constexpr float OBJECT_SPEED = 500.0f;

	shared_ptr<Object3D> light;

	std::vector<shared_ptr<Object3D>> objects;

	std::vector<shared_ptr<GPUMesh>> meshes;

	shared_ptr<GPUProgram> program;

	FPSViewController fps_view_controller;

	size_t selected_object_index = 0;


};

int main(int argc, char ** argv, char ** env)
{
    return RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
