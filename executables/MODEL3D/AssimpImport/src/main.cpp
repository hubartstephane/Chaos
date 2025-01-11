#include "chaos/Chaos.h"


struct MeshVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

class Object3D : public chaos::Object
{
public:

	void Display(chaos::GPUProgram * program, chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::GPURenderParams & render_params)
	{
		glm::mat4x4 local_to_world = glm::translate(position);

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("local_to_world", local_to_world);
		main_uniform_provider.AddVariable("material_color", color);

		mesh->DisplayWithProgram(program, renderer, &main_uniform_provider, render_params);
	}

public:

	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 color    = { 0.0f, 0.0f, 1.0f };

	std::string name;

	chaos::shared_ptr<chaos::GPUMesh> mesh;
};


class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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
			chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariable("projection", projection);
			main_uniform_provider.AddVariable("world_to_camera", world_to_camera);
			if (light != nullptr)
				main_uniform_provider.AddVariable("light_position", light->position);

			// display objects
			chaos::GPURenderParams render_params;

			for (chaos::shared_ptr<Object3D> const& object : objects)
				object->Display(program.get(), renderer, &main_uniform_provider, render_params);
		}

		return true;
	}

	boost::filesystem::path const & GetResourcesPath() const
	{
		chaos::WindowApplication* application = chaos::Application::GetInstance();
		return application->GetResourcesPath();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const* config) override
	{
		// super
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		// create light mesh
		CreateLightMesh();

		// load the mesh model
		LoadMeshes("scene.glb");

		// generate the program
		boost::filesystem::path const& resources_path = GetResourcesPath();

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / "vertex_shader.txt");
		program = program_generator.GenProgramObject();

		// update the camera speed
		fps_view_controller.config.back_speed = CAMERA_SPEED;
		fps_view_controller.config.down_speed = CAMERA_SPEED;
		fps_view_controller.config.up_speed = CAMERA_SPEED;
		fps_view_controller.config.forward_speed = CAMERA_SPEED;
		fps_view_controller.config.strafe_speed = CAMERA_SPEED;

		fps_view_controller.input_config.yaw_left_button   = chaos::KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.yaw_right_button  = chaos::KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.pitch_up_button   = chaos::KeyboardButton::UNKNOWN;
		fps_view_controller.input_config.pitch_down_button = chaos::KeyboardButton::UNKNOWN;

		fps_view_controller.fps_view.position.z = 1000.0f;

		return true;
	}

	Object3D * CreateObject3D(chaos::GPUMesh* mesh, glm::vec3 const& position, glm::vec3 const& color, char const * name)
	{
		Object3D* result = new Object3D;
		if (result == nullptr)
			return result;

		result->mesh = mesh;
		result->position = position;
		result->color = color;
		result->name = name;

		objects.push_back(result);

		return result;
	}

	bool CreateLightMesh()
	{
		chaos::sphere3 s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);

		chaos::shared_ptr<chaos::GPUMesh> mesh = chaos::GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 10).GenerateMesh();

		light = CreateObject3D(mesh.get(), { 0.0f, 500.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, "light");

		if (light == nullptr)
			return false;
		return true;
	}

	bool LoadMeshes(chaos::FilePathParam const & path)
	{
		// compute resource path
		boost::filesystem::path const & resources_path = GetResourcesPath();

		// load the file
		chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resources_path / path.GetResolvedPath());
		if (buffer == nullptr)
			return false;

		// get the extensions
		Assimp::Importer importer;

		std::string supported_extension;
		importer.GetExtensionList(supported_extension);

		// load the scene
		unsigned int load_flags =
			aiProcess_CalcTangentSpace |
			//aiProcess_GenNormals |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType;

		const aiScene* scene = importer.ReadFileFromMemory(buffer.data, buffer.bufsize, load_flags);

		if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
		{
			char const* error = importer.GetErrorString();
			if (error != nullptr)
				chaos::Log::Message("Assimp::Importer::ReadFileFromMemory failure [%s]", error);
			return false;
		}

		// get all meshes
		if (scene->HasMeshes())
		{
			// create a vertex declaration
			chaos::shared_ptr<chaos::GPUVertexDeclaration> declaration = new chaos::GPUVertexDeclaration();
			declaration->Push(chaos::VertexAttributeSemantic::POSITION, 0, chaos::VertexAttributeType::FLOAT3);
			declaration->Push(chaos::VertexAttributeSemantic::NORMAL, 0, chaos::VertexAttributeType::FLOAT3);

			for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
			{
				chaos::shared_ptr<chaos::GPUMesh> gpu_mesh = new chaos::GPUMesh;

				aiMesh const* mesh = scene->mMeshes[i];
				if (mesh == nullptr)
					continue;

				if (!mesh->HasNormals())
					continue;
				if (!mesh->HasPositions())
					continue;

				// vertices
				chaos::shared_ptr<chaos::GPUBuffer> vertex_buffer = new chaos::GPUBuffer(false);
				if (vertex_buffer == nullptr)
					break;
			
				unsigned int vertices_count = mesh->mNumVertices;
				size_t vertex_bufsize = vertices_count * sizeof(MeshVertex);
				vertex_buffer->SetBufferData(nullptr, vertex_bufsize); // set buffer size

				char * vertex_buf = vertex_buffer->MapBuffer(0, 0, false, true);
				if (vertex_buf == nullptr)
					break;

				chaos::MemoryBufferWriter vertices_writer(vertex_buf, vertex_bufsize);

				for (unsigned int i = 0 ; i < vertices_count ; ++i)
				{
					vertices_writer << mesh->mVertices[i] * 100.0f;
					vertices_writer << mesh->mNormals[i];
				}

				vertex_buffer->UnMapBuffer();

				// indices
				chaos::shared_ptr<chaos::GPUBuffer> index_buffer = new chaos::GPUBuffer(false);
				if (vertex_buffer == nullptr)
					break;

				unsigned int faces_count = mesh->mNumFaces;
				size_t index_bufsize = faces_count * 3 * sizeof(int32_t);
				index_buffer->SetBufferData(nullptr, index_bufsize); // set buffer size


				char* index_buf = index_buffer->MapBuffer(0, 0, false, true);
				if (index_buf == nullptr)
					break;

				chaos::MemoryBufferWriter indices_writer(index_buf, index_bufsize);

				for (unsigned int i = 0; i < faces_count; ++i)
				{
					assert(mesh->mFaces[i].mNumIndices == 3);

					for (unsigned j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
					{
						indices_writer << int32_t(mesh->mFaces[i].mIndices[j]);
					}
				}

				index_buffer->UnMapBuffer();

				// create the element
				chaos::GPUMeshElement & element = gpu_mesh->AddMeshElement(vertex_buffer.get(), index_buffer.get());
				element.vertex_declaration = declaration;

				chaos::GPUDrawPrimitive primitive; 
				primitive.indexed = true;
				primitive.primitive_type = GL_TRIANGLES;
				primitive.count = faces_count * 3;
				element.primitives.push_back(primitive);

				CreateObject3D(gpu_mesh.get(), {0.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, mesh->mName.C_Str());
			}
		}

		return true;
	}

	virtual void OnDrawImGuiContent() override
	{	
		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			if (ImGui::BeginTable("objects", 2, ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Fixed Width", ImGuiTableColumnFlags_WidthFixed, 100.0f);

				for (chaos::shared_ptr<Object3D> const& object : objects)
				{
					size_t current_index = (&object - &objects[0]);
					bool is_selected = (selected_object_index == current_index);

					ImGui::PushID(object.get());
					
					ImGui::TableNextColumn();
					if (ImGui::Selectable(object->name.c_str(), &is_selected))
						selected_object_index = current_index;

					ImGui::TableNextColumn();
					chaos::DrawImGuiVariable(object->position);

					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		});
		chaos::Window::OnDrawImGuiContent();
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& key_event) override
	{
		size_t object_count = objects.size();
		if (object_count > 0)
		{
			if (key_event.IsKeyDown(chaos::KeyboardButton::KP_ADD))
			{
				selected_object_index = (selected_object_index == object_count - 1)?
					0:
					selected_object_index + 1;
				return true;
			}
			if (key_event.IsKeyDown(chaos::KeyboardButton::KP_SUBTRACT))
			{
				selected_object_index = (selected_object_index == 0) ?
					object_count - 1 :
					selected_object_index - 1;
				return true;
			}
		}
		return chaos::Window::OnKeyEventImpl(key_event);
	}

	virtual bool DoTick(float delta_time) override
	{
		// move camera
		fps_view_controller.Tick(glfw_window, delta_time);

		// move objects
		size_t object_count = objects.size();
		if (object_count > 0)
		{
			auto MoveObject = [this, delta_time](chaos::KeyboardButton button, size_t component_index, float direction)
			{
				if (chaos::ButtonState const* state = chaos::KeyboardState::GetKeyboardButtonState(button))
				{
					if (state->IsPressed())
					{
						const float OBJECT_SPEED = 300.0f;

						Object3D* selected_object = objects[selected_object_index].get();
						selected_object->position[component_index] += direction * OBJECT_SPEED * delta_time;
					}
				}
			};

			MoveObject(chaos::KeyboardButton::KP_4, 0, -1.0f);
			MoveObject(chaos::KeyboardButton::KP_6, 0, +1.0f);
			MoveObject(chaos::KeyboardButton::KP_8, 2, -1.0f);
			MoveObject(chaos::KeyboardButton::KP_2, 2, +1.0f);
			MoveObject(chaos::KeyboardButton::KP_9, 1, +1.0f);
			MoveObject(chaos::KeyboardButton::KP_3, 1, -1.0f);
		}
		return chaos::Window::DoTick(delta_time);
	}

protected:

	static constexpr float CAMERA_SPEED = 400.0f;

	chaos::shared_ptr<Object3D> light;

	std::vector<chaos::shared_ptr<Object3D>> objects;

	chaos::shared_ptr<chaos::GPUProgram> program;

	chaos::FPSViewController fps_view_controller;

	size_t selected_object_index = 0;
};

int main(int argc, char ** argv, char ** env)
{
    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
