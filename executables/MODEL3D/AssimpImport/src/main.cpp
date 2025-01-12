#include "chaos/Chaos.h"

using namespace chaos;

struct MeshVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

class Object3D : public Object
{
public:

	void Display(GPUProgram * program, GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, GPURenderParams & render_params)
	{
		glm::mat4x4 local_to_world = transform.GetLocalToParent();

		GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("local_to_world", local_to_world);
		main_uniform_provider.AddVariable("material_color", color);

		mesh->DisplayWithProgram(program, renderer, &main_uniform_provider, render_params);
	}

public:

	std::string name;

	SceneTransform<float, 3> transform;

	glm::vec3 color = { 0.0f, 0.0f, 1.0f };

	shared_ptr<GPUMesh> mesh;
};


class WindowOpenGLTest : public Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, Window);

protected:

	virtual bool OnDraw(GPURenderer * renderer, GPUProgramProviderInterface const * uniform_provider, WindowDrawParams const& draw_params) override
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
				object->Display(program.get(), renderer, &main_uniform_provider, render_params);
		}

		return true;
	}

	boost::filesystem::path const & GetResourcesPath() const
	{
		WindowApplication* application = Application::GetInstance();
		return application->GetResourcesPath();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const* config) override
	{
		// super
		if (!Window::InitializeFromConfiguration(config))
			return false;

		// create light mesh
		CreateLightMesh();

		// load the mesh model
		LoadMeshesAndCreateObjects("scene.glb");

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

	Object3D * CreateObject3D(GPUMesh* mesh, glm::vec3 const& position, glm::vec3 const & scale, glm::quat const & rotation, glm::vec3 const& color, char const * name)
	{
		Object3D* result = new Object3D;
		if (result == nullptr)
			return result;

		result->mesh = mesh;
		result->transform.position = position;
		result->transform.scale    = scale;
		result->transform.rotator  = rotation;
		result->color = color;
		result->name = name;

		objects.push_back(result);

		return result;
	}

	bool CreateLightMesh()
	{
		sphere3 s = sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);

		shared_ptr<GPUMesh> gpu_mesh = GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 10).GenerateMesh();
		meshes.push_back(gpu_mesh);

		glm::vec3 position = { 0.0f, 300.0f, 0.0f };
		glm::vec3 scale    = { 1.0f, 1.0f, 1.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 color    = { 1.0f, 1.0f, 1.0f };

		light = CreateObject3D(
			gpu_mesh.get(),
			position,
			scale,
			rotation,
			color, 
			"light");

		return (light != nullptr);
	}

	bool LoadMeshesAndCreateObjects(FilePathParam const & path)
	{
		// compute resource path
		boost::filesystem::path const & resources_path = GetResourcesPath();

		// load the file
		Buffer<char> buffer = FileTools::LoadFile(resources_path / path.GetResolvedPath());
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
				Log::Message("Assimp::Importer::ReadFileFromMemory failure [%s]", error);
			return false;
		}

		// get all meshes

		std::vector<GPUMesh*> imported_meshes;

		if (scene->HasMeshes())
		{
			// create a vertex declaration
			shared_ptr<GPUVertexDeclaration> declaration = new GPUVertexDeclaration();
			declaration->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
			declaration->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);

			for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
			{
				shared_ptr<GPUMesh> gpu_mesh = new GPUMesh;

				aiMesh const* imported_mesh = scene->mMeshes[i];
				if (imported_mesh == nullptr)
					continue;

				if (!imported_mesh->HasNormals())
					continue;
				if (!imported_mesh->HasPositions())
					continue;

				// vertices
				shared_ptr<GPUBuffer> vertex_buffer = new GPUBuffer(false);
				if (vertex_buffer == nullptr)
					break;
			
				unsigned int vertices_count = imported_mesh->mNumVertices;
				size_t vertex_bufsize = vertices_count * sizeof(MeshVertex);
				vertex_buffer->SetBufferData(nullptr, vertex_bufsize); // set buffer size

				char * vertex_buf = vertex_buffer->MapBuffer(0, 0, false, true);
				if (vertex_buf == nullptr)
					break;

				MemoryBufferWriter vertices_writer(vertex_buf, vertex_bufsize);

				for (unsigned int i = 0 ; i < vertices_count ; ++i)
				{
					vertices_writer << imported_mesh->mVertices[i] * SCALE_SCENE_FACTOR;
					vertices_writer << imported_mesh->mNormals[i];
				}

				vertex_buffer->UnMapBuffer();

				// indices
				shared_ptr<GPUBuffer> index_buffer = new GPUBuffer(false);
				if (vertex_buffer == nullptr)
					break;

				unsigned int faces_count = imported_mesh->mNumFaces;
				size_t index_bufsize = faces_count * 3 * sizeof(int32_t);
				index_buffer->SetBufferData(nullptr, index_bufsize); // set buffer size


				char* index_buf = index_buffer->MapBuffer(0, 0, false, true);
				if (index_buf == nullptr)
					break;

				MemoryBufferWriter indices_writer(index_buf, index_bufsize);

				for (unsigned int i = 0; i < faces_count; ++i)
				{
					assert(imported_mesh->mFaces[i].mNumIndices == 3);

					for (unsigned j = 0; j < imported_mesh->mFaces[i].mNumIndices; ++j)
					{
						indices_writer << int32_t(imported_mesh->mFaces[i].mIndices[j]);
					}
				}

				index_buffer->UnMapBuffer();

				// create the element
				GPUMeshElement & element = gpu_mesh->AddMeshElement(vertex_buffer.get(), index_buffer.get());
				element.vertex_declaration = declaration;

				GPUDrawPrimitive primitive; 
				primitive.indexed = true;
				primitive.primitive_type = GL_TRIANGLES;
				primitive.count = faces_count * 3;
				element.primitives.push_back(primitive);

				meshes.push_back(gpu_mesh);

				imported_meshes.push_back(gpu_mesh.get());
			}
		}

		// create the scene
		ImportSceneNode(scene->mRootNode, imported_meshes);

		return true;
	}

	void ImportSceneNode(aiNode const * node, std::vector<GPUMesh*> const & imported_meshes)
	{
		if (node == nullptr)
			return;

		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			unsigned int imported_mesh_index = node->mMeshes[i];

			if (imported_mesh_index < imported_meshes.size())
			{
				GPUMesh * gpu_mesh = imported_meshes[imported_mesh_index];


				aiVector3D   ai_scale    = { 1.0f, 1.0f, 1.0f };
				aiVector3D   ai_position = { 0.0f, 0.0f, 0.0f };
				aiQuaternion ai_rotation = { 0.0f, 0.0f, 0.0f };

				node->mTransformation.Decompose(ai_scale, ai_rotation, ai_position);
				
				glm::vec3 position = { ai_position.x, ai_position.y, ai_position.z };
				glm::vec3 scale    = { ai_scale.x, ai_scale.y, ai_scale.z };
				glm::quat rotation = { ai_rotation.w, ai_rotation.x, ai_rotation.y, ai_rotation.z };
				glm::vec3 color    = { 0.0f, 0.0f, 1.0f };

				CreateObject3D(
					gpu_mesh, 
					position * SCALE_SCENE_FACTOR,
					scale,
					rotation,
					color, 
					node->mName.C_Str());
			}
		}

		// recurively import child nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			ImportSceneNode(node->mChildren[i], imported_meshes);
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
						selected_object_index = current_index;

					ImGui::TableNextColumn();
					DrawImGuiVariable(object->transform.position);

					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		});
		Window::OnDrawImGuiContent();
	}

	virtual bool OnKeyEventImpl(KeyEvent const& key_event) override
	{
		size_t object_count = objects.size();
		if (object_count > 0)
		{
			if (key_event.IsKeyDown(KeyboardButton::KP_ADD))
			{
				selected_object_index = (selected_object_index == object_count - 1)?
					0:
					selected_object_index + 1;
				return true;
			}
			if (key_event.IsKeyDown(KeyboardButton::KP_SUBTRACT))
			{
				selected_object_index = (selected_object_index == 0) ?
					object_count - 1 :
					selected_object_index - 1;
				return true;
			}
		}
		return Window::OnKeyEventImpl(key_event);
	}

	virtual bool DoTick(float delta_time) override
	{
		// move camera
		fps_view_controller.Tick(glfw_window, delta_time);

		// move objects
		size_t object_count = objects.size();
		if (object_count > 0)
		{
			auto MoveObject = [this, delta_time](KeyboardButton button, size_t component_index, float direction)
			{
				if (ButtonState const* state = KeyboardState::GetKeyboardButtonState(button))
				{
					if (state->IsPressed())
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

protected:

	static constexpr float CAMERA_SPEED = 400.0f;

	static constexpr float SCALE_SCENE_FACTOR = 100.0f;

	static constexpr float OBJECT_SPEED = 300.0f;

	shared_ptr<Object3D> light;

	std::vector<shared_ptr<Object3D>> objects;

	std::vector<shared_ptr<GPUMesh>> meshes;

	shared_ptr<GPUProgram> program;

	FPSViewController fps_view_controller;

	size_t selected_object_index = 0;


};

int main(int argc, char ** argv, char ** env)
{
    return RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
