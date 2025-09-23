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

	void Display(GPUProgram * program, GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams & render_params)
	{
		GPUProgramProviderChain main_uniform_provider(uniform_provider);

		glm::mat4x4 local_to_world = transform.GetLocalToParent();
		main_uniform_provider.AddVariable("local_to_world", local_to_world);

		if (selected)
		{
			glm::vec3 selected_color = { 0.0f, 0.0f, 0.0f };
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

		if (std::optional<box3> bounding_box = mesh->GetBoundingBox())
		{
			if (std::optional<box3> wireframe_bounding_box = wireframe_box_mesh->GetBoundingBox())
			{
				glm::vec3 scale = bounding_box->half_size / wireframe_bounding_box->half_size;

				local_to_world = local_to_world * glm::translate(bounding_box->position) * glm::scale(scale) * glm::translate(-wireframe_bounding_box->position);

				main_uniform_provider.AddVariable("local_to_world", local_to_world);

				if (wireframe_box_mesh != nullptr)
					wireframe_box_mesh->DisplayWithProgram(program, render_context, &main_uniform_provider, render_params);
			}
		}
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

	shared_ptr<GPUMesh> wireframe_box_mesh;

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

		glm::vec4 clear_color(0.4f, 0.4f, 0.4f, 1.0f);
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

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		// super
		if (!Window::OnInitialize(config))
			return false;

		// create a wireframe box
		CreateWireframeBoxMesh();

		// create light mesh
		CreateLightMesh();

		// load the mesh model
		LoadMeshesAndCreateObjects("scene.glb");

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

		fps_view_controller.fps_view.position.z = 2000.0f;

		return true;
	}

	Object3D * CreateObject3D(GPUMesh* mesh, glm::vec3 const& position, glm::vec3 const & scale, glm::quat const & rotation, glm::vec3 const& color, glm::vec3 const& emissive_color, char const * name)
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
		result->wireframe_box_mesh = wireframe_box_mesh.get();

		objects.push_back(result);

		return result;
	}

	bool CreateWireframeBoxMesh()
	{
		box3 box;
		box.position  = { 0.0f, 0.0f, 0.0f };
		box.half_size = { 1.0f, 1.0f, 1.0f };
		GPUWireframeBoxMeshGenerator generator(box);
		 	
		wireframe_box_mesh = generator.GenerateMesh(GetGPUDevice());

		return (wireframe_box_mesh != nullptr);
	}

	bool CreateLightMesh()
	{
		sphere3 s = sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);

		shared_ptr<GPUMesh> gpu_mesh = GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 10).GenerateMesh(GetGPUDevice());
		meshes.push_back(gpu_mesh);

		glm::vec3 position       = { -500.0f, 0.0f, 1000.0f };
		glm::vec3 scale          = { 1.0f, 1.0f, 1.0f };
		glm::quat rotation       = { 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 color          = { 0.0f, 0.0f, 0.0f };
		glm::vec3 emissive_color = { 1.0f, 1.0f, 1.0f };

		light = CreateObject3D(
			gpu_mesh.get(),
			position,
			scale,
			rotation,
			color, 
			emissive_color,
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
			shared_ptr<GPUVertexDeclaration> vertex_declaration = new GPUVertexDeclaration();
			vertex_declaration->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
			vertex_declaration->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);

			for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
			{
				shared_ptr<GPUMesh> gpu_mesh = new GPUMesh(GetGPUDevice());

				aiMesh const* ai_mesh = scene->mMeshes[i];
				if (ai_mesh == nullptr)
					continue;

				if (!ai_mesh->HasNormals())
					continue;
				if (!ai_mesh->HasPositions())
					continue;

				// allocate buffers
				unsigned int vertices_count = ai_mesh->mNumVertices;
				size_t vertex_bufsize = vertices_count * sizeof(MeshVertex);

				unsigned int faces_count = ai_mesh->mNumFaces;
				size_t index_bufsize = faces_count * 3 * sizeof(int32_t);

				GPUVertexAndIndexMappedBuffers buffers = GPUVertexAndIndexMappedBuffers::CreateMappedBuffers(GetGPUDevice(), vertex_bufsize, index_bufsize);

				// vertices
				MemoryBufferWriter vertices_writer(buffers.mapped_vertex_buffer, vertex_bufsize);

				box3 bounding_box;
				for (unsigned int i = 0 ; i < vertices_count ; ++i)
				{
					glm::vec3 position = { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z};
					glm::vec3 normal   = { ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z};

					position *= SCALE_SCENE_FACTOR;

					ExtendBox(bounding_box, position);

					vertices_writer << position;
					vertices_writer << normal;
				}

				gpu_mesh->SetBoundingBox(bounding_box);

				// indices


				// the height line going from bounding box center to bounding box corners
				glm::vec3 bounding_box_corners[8] =
				{
					bounding_box.half_size * glm::vec3( -1.0f, -1.0f, -1.0f ), //     corner [0]
					bounding_box.half_size * glm::vec3( -1.0f, -1.0f,  1.0f ), //     corner [1]
					bounding_box.half_size * glm::vec3( -1.0f,  1.0f, -1.0f ), //     corner [2]
					bounding_box.half_size * glm::vec3( -1.0f,  1.0f,  1.0f ), //     corner [3]
					bounding_box.half_size * glm::vec3(  1.0f, -1.0f, -1.0f ), //     corner [4]
					bounding_box.half_size * glm::vec3(  1.0f, -1.0f,  1.0f ), //     corner [5]
					bounding_box.half_size * glm::vec3(  1.0f,  1.0f, -1.0f ), //     corner [6]
					bounding_box.half_size * glm::vec3(  1.0f,  1.0f,  1.0f )  //     corner [7]
				};

				// the six faces of the bounding box
				glm::int4 faces_to_corners[6]
				{
					{ 0, 1, 2, 3 }, // face X < 0      face [0]
					{ 4, 5, 6, 7 }, // face X > 0      face [1]
					{ 0, 1, 4, 5 }, // face Y < 0      face [2]
					{ 2, 3, 6, 7 }, // face Y > 0      face [3]
					{ 0, 2, 4, 6 }, // face Z < 0      face [4]
					{ 1, 3, 5, 7 }  // face Z > 0      face [5]
				};

				glm::int3 corners_to_faces[8]
				{
					{ 0, 2, 4 }, // vertex 0 belongs to faces {0, 2, 4}
					{ 0, 2, 5 },
					{ 0, 3, 4 },
					{ 0, 3, 5 },
					{ 1, 2, 4 },
					{ 1, 2, 5 },
					{ 1, 3, 4 },
					{ 1, 3, 5 }
				};

				std::vector<uint32_t> mesh_faces[64]; // 6 faces -> 32 possible combinaisons


				for (unsigned int i = 0; i < faces_count; ++i)
				{
					aiFace const & ai_face = ai_mesh->mFaces[i];

					assert(ai_face.mNumIndices == 3);

					int index_a = ai_face.mIndices[0];
					int index_b = ai_face.mIndices[1];
					int index_c = ai_face.mIndices[2];

					glm::vec3 a =
					{
						ai_mesh->mVertices[index_a].x,
						ai_mesh->mVertices[index_a].y,
						ai_mesh->mVertices[index_a].z
					};

					glm::vec3 b =
					{
						ai_mesh->mVertices[index_b].x,
						ai_mesh->mVertices[index_b].y,
						ai_mesh->mVertices[index_b].z
					};

					glm::vec3 c =
					{
						ai_mesh->mVertices[index_c].x,
						ai_mesh->mVertices[index_c].y,
						ai_mesh->mVertices[index_c].z
					};

					glm::vec3 normal = glm::cross(b - a, c - a); // no need to normalize

					unsigned int face_mask = 0;
					for (int j = 0; j < 4; ++j) // no need to iterate over the 8 corners. iterate over only half. we know that the normal must accept a corner or its opposition (opposite = 7 - corner)
					{
						if (dot(normal, bounding_box_corners[j]) > 0.0f)
						{
							for (int k = 0 ; k < 3 ; ++k)
								face_mask |= 1 << (corners_to_faces[j][k]);
						}
						else
						{
							for (int k = 0; k < 3; ++k)
								face_mask |= 1 << (corners_to_faces[7 - j][k]);
						}
					}

					mesh_faces[face_mask].push_back(index_a);
					mesh_faces[face_mask].push_back(index_b);
					mesh_faces[face_mask].push_back(index_c);
				};

				MemoryBufferWriter indices_writer(buffers.mapped_index_buffer, index_bufsize);

				for (size_t i = 0 ; i < 64; ++i)
				{
					if ((i & 4) == 0)
						continue;

					std::vector<uint32_t> const & faces = mesh_faces[i];



					GPUMeshElement& element = gpu_mesh->AddMeshElement(vertex_declaration.get(), buffers.vertex_buffer.get(), buffers.index_buffer.get());

					GPUDrawPrimitive primitive;
					primitive.indexed = true;
					primitive.primitive_type = GL_TRIANGLES;
					primitive.start = (int)indices_writer.GetWrittenCount() / (sizeof(uint32_t));
					primitive.count = (int)mesh_faces[i].size();
					element.primitives.push_back(primitive);


					for (uint32_t index : faces)
						indices_writer << index;
						






				}

				auto u = indices_writer.GetWrittenCount() / (3 * sizeof(uint32_t));
				auto v = faces_count;

				auto n = ai_mesh->mName.C_Str();

				float delta = 100.0f * float(u) / float(v);


				u = u;



#if 0

				

				for (unsigned int i = 0; i < faces_count; ++i)
				{
					assert(ai_mesh->mFaces[i].mNumIndices == 3);

					for (unsigned j = 0; j < ai_mesh->mFaces[i].mNumIndices; ++j)
					{
						indices_writer << int32_t(ai_mesh->mFaces[i].mIndices[j]);
					}
				}


				// create the element
				GPUMeshElement & element = gpu_mesh->AddMeshElement(vertex_declaration.get(), buffers.vertex_buffer.get(), buffers.index_buffer.get());

				GPUDrawPrimitive primitive; 
				primitive.indexed = true;
				primitive.primitive_type = GL_TRIANGLES;
				primitive.count = faces_count * 3;
				element.primitives.push_back(primitive);
#endif
				// clean the resource
				buffers.CleanResources();

				// store new instances
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
				glm::vec3 emissive_color = { 0.0f, 0.0f, 0.0f };

				CreateObject3D(gpu_mesh, 
					position * SCALE_SCENE_FACTOR,
					scale,
					rotation,
					color, 
					emissive_color,
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
				if (KeyboardAndMouseState::GetInstance()->IsKeyDown(button))
				{
					Object3D* selected_object = objects[selected_object_index].get();
					selected_object->transform.position[component_index] += direction * OBJECT_SPEED * delta_time;
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

	static constexpr float SCALE_SCENE_FACTOR = 100.0f;

	static constexpr float OBJECT_SPEED = 500.0f;

	shared_ptr<Object3D> light;

	std::vector<shared_ptr<Object3D>> objects;

	std::vector<shared_ptr<GPUMesh>> meshes;

	shared_ptr<GPUMesh> wireframe_box_mesh;

	shared_ptr<GPUProgram> program;

	FPSViewController fps_view_controller;

	size_t selected_object_index = 0;


};

int main(int argc, char ** argv, char ** env)
{
    return RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
