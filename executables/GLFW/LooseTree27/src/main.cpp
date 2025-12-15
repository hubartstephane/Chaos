#include "chaos/Chaos.h"

#include "PrimitiveRenderer.h"

static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class Tree27NodeBase;

using loose_tree_type = chaos::LooseTree27<3, float, Tree27NodeBase, chaos::ObjectPool>;
using loose_tree_node_type = loose_tree_type::node_type;

// =======================================================================

enum class GeometryType
{
	SPHERE,
	BOX,
	COUNT
};

enum class ActionType
{
	CREATE_SPHERE,
	CREATE_BOX,
	MOVE_OBJECT,
	SCALE_OBJECT,
	ROTATE_OBJECT
};

// =======================================================================

class KeyConfiguration
{
public:

	chaos::Key toggle_render_all = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("R"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key new_scene = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("Y"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key delete_object = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("DELETE"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key next_object = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("KP_ADD"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key previous_object = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("KP_SUBTRACT"), chaos::KeyboardLayoutType::AZERTY);

	chaos::Key move_object_positive_x = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("D"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key move_object_negative_x = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("Q"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key move_object_positive_y = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("E"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key move_object_negative_y = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("A"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key move_object_positive_z = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("S"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key move_object_negative_z = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("Z"), chaos::KeyboardLayoutType::AZERTY);

	chaos::Key scale_object_positive_x = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("D"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key scale_object_negative_x = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("Q"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key scale_object_positive_y = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("E"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key scale_object_negative_y = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("A"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key scale_object_positive_z = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("S"), chaos::KeyboardLayoutType::AZERTY);
	chaos::Key scale_object_negative_z = chaos::KeyboardLayoutConversion::ConvertKey(chaos::GetKeyFromName("Z"), chaos::KeyboardLayoutType::AZERTY);
};

// =======================================================================

ActionType current_action_type = ActionType::CREATE_BOX;

KeyConfiguration key_configuration;

loose_tree_type object_tree;

// =======================================================================

class CameraInfo
{
public:

	float fov = 60.0f;
	float far_plane = 5000.0f;
	float near_plane = 20.0f;
};

// =======================================================================

class ImGuiCameraInformationObject : public chaos::ImGuiObject
{
public:

	ImGuiCameraInformationObject(CameraInfo& in_camera_info) :
		camera_info(in_camera_info)
	{
	}

protected:

	virtual void OnDrawImGuiContent(chaos::Window* window) override
	{
		ImGui::InputFloat("near_plane", &camera_info.near_plane, 10.0f, 50.0f);
		ImGui::InputFloat("far_plane", &camera_info.far_plane, 10.0f, 50.0f);
		ImGui::InputFloat("fov", &camera_info.fov, 1.0f, 5.0f);
	}

protected:

	CameraInfo & camera_info;
};



// =======================================================================

class Tree27NodeBase
{
public:

	bool IsUseful() const
	{
		return (objects.size() > 0);
	}

public:

	std::vector<class GeometricObject*> objects;
};

// =======================================================================

class GeometricObject : public chaos::Object, public chaos::InputReceiverInterface
{
public:

	virtual char const* GetInputReceiverName() const override
	{
		return "Geometric Object";
	}

	chaos::box3 GetBoundingBox() const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE: return chaos::GetBoundingBox(object_sphere);
		case GeometryType::BOX: return object_box;
		default: assert(0);
		};
		return {};
	}

	void DrawPrimitive(PrimitiveRenderer * primitive_renderer, glm::vec4 const & color) const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE:
			primitive_renderer->DrawPrimitive(object_sphere, color, false);
			break;
		case GeometryType::BOX:
			primitive_renderer->DrawPrimitive(object_box, color, false, false);
			break;
		default:
			assert(0);
		};
	}

	chaos::RayConvexGeometryIntersectionResult<3, float> GetIntersection(chaos::ray3 const& r) const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE:
			return chaos::GetIntersection(r, object_sphere);
		case GeometryType::BOX:
			break;
		default:
			assert(0);
		};
		return {};
	}

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator& in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		if (current_action_type == ActionType::MOVE_OBJECT)
		{
			auto MoveObjectWithInputs = [&](chaos::Key key, char const * title, chaos::Direction direction)
			{
				return in_action_enumerator.CheckAndProcess(chaos::Active(key), title, [&]()
				{
					float delta_time = (float)chaos::FrameTimeManager::GetInstance()->GetCurrentFrameDuration();

					float final_speed = DISPLACEMENT_SPEED;

					glm::vec3 delta_position = DirectionToVector(direction) * delta_time * final_speed;

					if (geometry_type == GeometryType::SPHERE)
						object_sphere.position += delta_position;
					else if (geometry_type == GeometryType::BOX)
						object_box.position += delta_position;
					OnObjectMoved();
				});
			};

			return
				MoveObjectWithInputs(key_configuration.move_object_negative_x, "move object -X", chaos::Direction::NEGATIVE_X) ||
				MoveObjectWithInputs(key_configuration.move_object_positive_x, "move object +X", chaos::Direction::POSITIVE_X) ||
				MoveObjectWithInputs(key_configuration.move_object_negative_y, "move object -Y", chaos::Direction::NEGATIVE_Y) ||
				MoveObjectWithInputs(key_configuration.move_object_positive_y, "move object +Y", chaos::Direction::POSITIVE_Y) ||
				MoveObjectWithInputs(key_configuration.move_object_negative_z, "move object -Z", chaos::Direction::NEGATIVE_Z) ||
				MoveObjectWithInputs(key_configuration.move_object_positive_z, "move object +Z", chaos::Direction::POSITIVE_Z);
		}

		if (current_action_type == ActionType::SCALE_OBJECT)
		{
			auto ScaleObjectWithInputs = [&](chaos::Key key, char const* title, chaos::Direction direction)
				{
					return in_action_enumerator.CheckAndProcess(chaos::Active(key), title, [&]()
					{
						float delta_time = (float)chaos::FrameTimeManager::GetInstance()->GetCurrentFrameDuration();

						float final_scale_speed = SCALE_SPEED;

						if (geometry_type == GeometryType::SPHERE)
						{
							object_sphere.radius += GetDirectionSign(direction) * final_scale_speed;
							object_sphere.radius = std::max(1.0f, object_sphere.radius);
						}
						else if (geometry_type == GeometryType::BOX)
						{
							float& half_size_component = object_box.half_size[size_t(DirectionToAxis(direction))];
							half_size_component += GetDirectionSign(direction) * final_scale_speed;
							half_size_component = std::max(1.0f, half_size_component);
						}
						OnObjectMoved();
					});
				};

			return
				ScaleObjectWithInputs(key_configuration.scale_object_negative_x, "scale object -X", chaos::Direction::NEGATIVE_X) ||
				ScaleObjectWithInputs(key_configuration.scale_object_positive_x, "scale object +X", chaos::Direction::POSITIVE_X) ||
				ScaleObjectWithInputs(key_configuration.scale_object_negative_y, "scale object -Y", chaos::Direction::NEGATIVE_Y) ||
				ScaleObjectWithInputs(key_configuration.scale_object_positive_y, "scale object +Y", chaos::Direction::POSITIVE_Y) ||
				ScaleObjectWithInputs(key_configuration.scale_object_negative_z, "scale object -Z", chaos::Direction::NEGATIVE_Z) ||
				ScaleObjectWithInputs(key_configuration.scale_object_positive_z, "scale object +Z", chaos::Direction::POSITIVE_Z);
		}


		return chaos::InputReceiverInterface::EnumerateInputActions(in_action_enumerator, in_context);
	}

	void InsertIntoTree()
	{
		if (loose_tree_node_type* new_node = object_tree.GetOrCreateNode(GetBoundingBox()))
		{
			if (new_node != node)
			{
				new_node->objects.push_back(this);
				RemoveFromTree();
				node = new_node;
			}
		}
	}

	void RemoveFromTree()
	{
		if (node != nullptr)
		{
			node->objects.erase(std::ranges::find(node->objects, this));
			object_tree.DeleteNodeIfPossible(node);
			node = nullptr;
		}
	}

	void OnObjectMoved()
	{
		InsertIntoTree();
	}

public:

	GeometryType geometry_type = GeometryType::SPHERE;

	chaos::sphere3 object_sphere;

	chaos::box3    object_box;

	size_t object_id = 0;

	loose_tree_node_type * node = nullptr;

	static constexpr float DISPLACEMENT_SPEED = 100.0f;

	static constexpr float FAST_DISPLACEMENT_SPEED = 200.0f;

	static constexpr float SCALE_SPEED = 1.5f;

	static constexpr float FAST_SCALE_SPEED = 3.0f;

};

// =======================================================================

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

	friend class GeometricObject;

protected:

	chaos::perspective<3, float> GetPerspective(glm::vec2 viewport_size) const
	{
		chaos::perspective<3, float> result;
		result.fov = camera_info.fov;
		result.width = viewport_size.x;
		result.height = viewport_size.y;
		result.front = camera_info.near_plane;
		result.back = camera_info.far_plane;
		return result;
	}

	glm::mat4x4 GetProjectionMatrix(glm::vec2 viewport_size) const
	{
		return chaos::GetProjectionMatrix(GetPerspective(viewport_size));
	}

	glm::mat4x4 GetGlobalToCameraMatrix() const
	{
		return fps_view_controller.GlobalToLocal();
	}

	glm::mat4x4 GetCameraToGlobalMatrix() const
	{
		return fps_view_controller.LocalToGlobal();
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		glClearBufferfi(GL_DEPTH_STENCIL, 0, camera_info.far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		primitive_renderer->projection      = GetProjectionMatrix(draw_params.viewport.size);
		primitive_renderer->world_to_camera = GetGlobalToCameraMatrix();
		primitive_renderer->render_context        = render_context;

		DrawGeometryObjects(render_context, uniform_provider, draw_params);
		DrawTree27(render_context, uniform_provider, draw_params);
		DrawAction(render_context, uniform_provider, draw_params);

		return true;
	}

	void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();
		OnDrawToolbar();
		OnDrawTreeInfo();
	}

	void OnDrawTreeInfo()
	{
		if (ImGuiViewport* viewport = ImGui::GetMainViewport())
		{
			ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 15.0f, viewport->Size.y - 15.0f), ImGuiCond_FirstUseEver, ImVec2(1.0f, 1.0f));
			if (ImGui::Begin("Tree Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
			{
				size_t object_count = geometric_objects.size();
				size_t node_count = object_tree.GetNodeCount();
				size_t node_size = sizeof(loose_tree_node_type);
				size_t total_size = node_count * node_size;
				ImGui::Text("Object Count          = %d", object_count);
				ImGui::Text("Node   Count          = %d", node_count);
				ImGui::Text("Node   Size           = %s", chaos::StringTools::MemorySizeToString(node_size).c_str());
				ImGui::Text("Total  Size           = %s", chaos::StringTools::MemorySizeToString(total_size).c_str());
				ImGui::Text("Object Rendered       = %d", object_rendered);
				ImGui::Text("Node   Visited        = %d", node_visited);
				ImGui::Text("Node   No plane check = %d", node_noplanecheck_visited);
				ImGui::End();
			}
		}
	}

	void OnDrawToolbar()
	{
		if (ImGuiViewport* viewport = ImGui::GetMainViewport())
		{
			ImGui::SetNextWindowPos(ImVec2(15.0f, viewport->Size.y - 15.0f), ImGuiCond_FirstUseEver, ImVec2(0.0f, 1.0f));

			if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto AddButton = [this](ActionType type, chaos::GPUTexture* texture, char const* tooltip, float base_cursor_y)
					{
						if (texture != nullptr)
						{
							ImVec2 icon_size = { 32.0f, 32.0f };

							bool selected = (current_action_type == type);

							if (selected)
							{
								ImGui::SetCursorPosY(base_cursor_y);
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
								ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
								ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
								icon_size = { 40.0f, 40.0f };
							}
							else
							{
								ImGui::SetCursorPosY(base_cursor_y + 4.0f);
							}

							ImTextureID textureID = (ImTextureID)((uint64_t)texture->GetResourceID()); // conversion into uint64_t to remove a C4312 warning

							if (ImGui::ImageButton(tooltip, textureID, icon_size, ImVec2(0, 1), ImVec2(1, 0))) // reverse the texture coordinate along Y
							{
								current_action_type = type;
							}
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(tooltip);

							if (selected)
								ImGui::PopStyleColor(3);
						}
					};

				float base_cursor_y = ImGui::GetCursorPosY();

				AddButton(ActionType::CREATE_BOX, box_icon_texture.get(), "create box", base_cursor_y);
				ImGui::SameLine();
				AddButton(ActionType::CREATE_SPHERE, sphere_icon_texture.get(), "create sphere", base_cursor_y);
				ImGui::SameLine();
				AddButton(ActionType::MOVE_OBJECT, move_icon_texture.get(), "move object", base_cursor_y);
				ImGui::SameLine();
				AddButton(ActionType::SCALE_OBJECT, scale_icon_texture.get(), "scale object", base_cursor_y);
				ImGui::SameLine();
				AddButton(ActionType::ROTATE_OBJECT, rotate_icon_texture.get(), "rotate object", base_cursor_y);

				ImGui::End();
			}
		}
	}

	void DrawTree27(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params)
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		object_tree.Visit([this](loose_tree_node_type const* node)
		{
			primitive_renderer->DrawPrimitive(node->GetBoundingBox(), white, false, true);
		});

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	void DrawGeometryObjects(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params)
	{
		auto RenderObject = [&](GeometricObject const* geometric_object)
		{
			glm::vec4 color = white;
			if (pointed_object == geometric_object)
				color = red;
			else if (geometric_object == GetCurrentGeometricObject())
				color = blue;

			geometric_object->DrawPrimitive(primitive_renderer.get(), color);
		};

		node_visited = 0;
		node_noplanecheck_visited = 0;
		object_rendered = 0;

		if (render_all)
		{
			for (auto& geometric_object : geometric_objects)
				RenderObject(geometric_object.get());
			object_rendered += geometric_objects.size();
		}
		else
		{
			chaos::perspective<3, float> perspective = GetPerspective(draw_params.viewport.size);
			chaos::box_planes3 planes = GetProjectionPlanes(perspective);


			//float SIZE = 500.0f;
			//chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(SIZE, SIZE, SIZE));
			//chaos::box_plane3 planes = GetBoxPlanes(b);

			chaos::Tree27PlaneClipVisitor::Visit(object_tree, &planes[0], planes.size(), [&](loose_tree_node_type const* node, glm::vec4 const* planes, size_t plane_count, uint32_t plane_bitfield)
			{
				for (GeometricObject const* geometric_object : node->objects)
					RenderObject(geometric_object);
				object_rendered += node->objects.size();
				++node_visited;
				if (plane_bitfield == 0)
					++node_noplanecheck_visited;
			});
		}	
	}

	chaos::box3 GetBoxToCreateFromMousePosition() const
	{
		chaos::ray3 r = GetRayFromMousePosition();
		glm::vec3 center = r.position + CREATE_OBJECT_DISTANCE * r.direction;

		chaos::box3 result;
		result.position = center;
		result.half_size = glm::vec3(5.0f, 7.0f, 9.0f);

		return result;
	}

	chaos::sphere3 GetSphereToCreateFromMousePosition() const
	{
		chaos::ray3 r = GetRayFromMousePosition();
		glm::vec3 center = r.position + CREATE_OBJECT_DISTANCE * r.direction;

		chaos::sphere3 result;
		result.position = center;
		result.radius = 10.0f;

		return result;
	}

	void DrawAction(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params)
	{
		if (IsImGuiMenuEnabled())
		{
			if (current_action_type == ActionType::CREATE_BOX)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				primitive_renderer->DrawPrimitive(GetBoxToCreateFromMousePosition(), red, false, false);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else if (current_action_type == ActionType::CREATE_SPHERE)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				primitive_renderer->DrawPrimitive(GetSphereToCreateFromMousePosition(), red, false);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}

	virtual void Finalize() override
	{
		box_icon_texture = nullptr;
		sphere_icon_texture = nullptr;
		move_icon_texture = nullptr;
		scale_icon_texture = nullptr;
		rotate_icon_texture = nullptr;

		primitive_renderer = nullptr;
		chaos::Window::Finalize();
	}

	chaos::GPUTexture* LoadTexture(char const* filename) const
	{
		if (chaos::WindowApplication* application = chaos::Application::GetInstance())
		{
			boost::filesystem::path resources_path = application->GetResourcesPath();

			chaos::GPUTextureLoader loader(GetGPUDevice());
			return loader.LoadObject(resources_path / filename);
		}
		return nullptr;
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		// super
		if (!chaos::Window::OnInitialize(config))
			return false;

		// create the render_context
		primitive_renderer = new PrimitiveRenderer();
		if (primitive_renderer == nullptr)
			return false;
		if (!primitive_renderer->Initialize())
			return false;
		// update the camera speed
		fps_view_controller.config.back_speed = CAMERA_SPEED;
		fps_view_controller.config.down_speed = CAMERA_SPEED;
		fps_view_controller.config.up_speed = CAMERA_SPEED;
		fps_view_controller.config.forward_speed = CAMERA_SPEED;
		fps_view_controller.config.strafe_speed = CAMERA_SPEED;

		fps_view_controller.input_config.rotation_button = chaos::Key::MOUSE_BUTTON_2;

		// create the very single sphere
		chaos::box3 creation_box;
		creation_box.position  = glm::vec3(0.0f, 0.0f, 0.0f);
		creation_box.half_size = glm::vec3(5.0f, 7.0f, 9.0f);

		CreateNewBox(creation_box);

		// create icons
		box_icon_texture = LoadTexture("BoxIcon.png");
		sphere_icon_texture = LoadTexture("SphereIcon.png");
		move_icon_texture = LoadTexture("MoveIcon.png");
		scale_icon_texture = LoadTexture("ScaleIcon.png");
		rotate_icon_texture = LoadTexture("RotateIcon.png");

		key_configuration = KeyConfiguration(); // necessary because the global instance is not properly constructed

		return true;
	}

	GeometricObject* GetCurrentGeometricObject()
	{
		return (!current_object_index.has_value()) ? nullptr : geometric_objects[current_object_index.value()].get();
	}

	bool IsImGuiMenuEnabled() const
	{
		if (chaos::WindowApplication const* application = chaos::Application::GetInstance())
		{
			return application->IsImGuiMenuEnabled();
		}

		return false;
	}

	chaos::ray3 GetRayFromMousePosition() const
	{
		glm::ivec2 window_size = GetWindowSize();
		if (window_size.x > 0 && window_size.y > 0)
		{
			// get the viewport
			chaos::aabox2 viewport = GetRequiredViewport(window_size);

			// get the position of the cursor
			double x = 0.0;
			double y = 0.0;
			glfwGetCursorPos(glfw_window, &x, &y);

			// compute the ray in projection space from eye center to cursor
			auto RecenterInterval = [](float value)
			{
				return (value * 2.0f - 1.0f);
			};

			glm::mat4x4 proj_inv = glm::inverse(GetProjectionMatrix(viewport.size));
			glm::mat4x4 cam_to_world = GetCameraToGlobalMatrix();

			glm::vec4 ray_point1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			ray_point1 = cam_to_world * ray_point1;

			glm::vec4 ray_point2 = glm::vec4(
				 RecenterInterval(((float)x - viewport.position.x) / viewport.size.x),
				-RecenterInterval(((float)y - viewport.position.y) / viewport.size.y), // for mouse position, Y = 0.0f is top Y = 1.0f is bottom
				1.0f,
				1.0f);

			ray_point2 = proj_inv * ray_point2;
			ray_point2 = cam_to_world * ray_point2;

			glm::vec3 p1 = glm::vec3(ray_point1.x / ray_point1.w, ray_point1.y / ray_point1.w, ray_point1.z / ray_point1.w);
			glm::vec3 p2 = glm::vec3(ray_point2.x / ray_point2.w, ray_point2.y / ray_point2.w, ray_point2.z / ray_point2.w);

			chaos::ray3 r;
			r.position = p1;
			r.direction = glm::normalize(p2 - p1);

			return r;
		}

		return chaos::ray3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	}

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator & in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		GeometricObject* current_object = GetCurrentGeometricObject();

		bool action_enabled = (current_object != nullptr);

		if (in_action_enumerator.CheckAndProcess(JustActivated(key_configuration.next_object), "Next Object", action_enabled, [&]()
		{
			current_object_index = (current_object_index.value() + 1) % geometric_objects.size();
		}))
		{
			return true;
		}
		
		if (in_action_enumerator.CheckAndProcess(JustActivated(key_configuration.previous_object), "Previous Object", action_enabled, [&]()
		{
			current_object_index = (current_object_index.value() + geometric_objects.size() - 1) % geometric_objects.size();
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(JustActivated(key_configuration.delete_object), "Delete Object", action_enabled, [&]()
		{
			if (geometric_objects.size() > 0)
			{
				// remove the object from tree
				current_object->RemoveFromTree();
				// remove the object from object list
				auto it = std::ranges::find_if(geometric_objects, [current_object](auto& v)
				{
					return (v.get() == current_object);
				});
				geometric_objects.erase(it); // destroy the last reference on object
				// select object near in creation order
				if (geometric_objects.size() == 0)
					current_object_index.reset();
				else
					current_object_index = (current_object_index.value() == 0)? 0 : (current_object_index.value() - 1); // while there is always at least one object in the array, this is always valid
			}
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(JustActivated(key_configuration.toggle_render_all), "Toggle Render All", [&]()
		{
			render_all = !render_all;

		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(JustActivated(key_configuration.new_scene), "New Scene", [&]()
		{
			geometric_objects.clear();
			current_object_index = 0;
			object_tree.Clear();

			int OBJECT_COUNT = 5000;
			for (int i = 0; i < OBJECT_COUNT; ++i)
			{
				glm::vec3 center = {
					chaos::MathTools::RandFloat(-1000.0f, 1000.0f),
					chaos::MathTools::RandFloat(-1000.0f, 1000.0f),
					chaos::MathTools::RandFloat(-1000.0f, 1000.0f)
				};

				chaos::box3 creation_box;
				creation_box.position = center;
				creation_box.half_size = {
					chaos::MathTools::RandFloat(1.0f, 25.0f),
					chaos::MathTools::RandFloat(1.0f, 25.0f),
					chaos::MathTools::RandFloat(1.0f, 25.0f)
				};

				std::random_device rd;
				std::uniform_int_distribution<int> uniform_dist(0, int(GeometryType::COUNT) - 1);

				switch(uniform_dist(rd))
				{
				case 0: CreateNewBox(creation_box); break;
				case 1: CreateNewSphere(chaos::GetBoundingSphere(creation_box)); break;
				default: assert(0);
				}
			}

		}))
		{
			return true;
		}

		if (IsImGuiMenuEnabled())
		{

			if (current_action_type == ActionType::CREATE_BOX)
			{
				if (in_action_enumerator.CheckAndProcess(JustActivated(chaos::Key::MOUSE_BUTTON_1), "Create Box", [&]()
				{
					CreateNewBox(GetBoxToCreateFromMousePosition());
				}))
				{
					return true;
				}
			}

			if (current_action_type == ActionType::CREATE_SPHERE)
			{
				if (in_action_enumerator.CheckAndProcess(JustActivated(chaos::Key::MOUSE_BUTTON_1), "Create Sphere", [&]()
				{
					CreateNewSphere(GetSphereToCreateFromMousePosition());
				}))
				{
					return true;
				}
			}

			if (current_action_type == ActionType::MOVE_OBJECT || current_action_type == ActionType::SCALE_OBJECT || current_action_type == ActionType::ROTATE_OBJECT)
			{
				if (in_action_enumerator.CheckAndProcess(JustActivated(chaos::Key::MOUSE_BUTTON_1), "Select Object", [&]()
				{
					if (pointed_object == nullptr)
						current_object_index.reset();
					else
					{
						for (size_t i = 0; i < geometric_objects.size(); ++i)
						{
							if (pointed_object == geometric_objects[i].get())
							{
								current_object_index = i;
								break;
							}
						}
					}
				}))
				{
					return true;
				}
			}
		}

		return chaos::Window::EnumerateInputActions(in_action_enumerator, in_context);
	}

	GeometricObject* CreateNewGeometry(GeometryType type)
	{
		if (GeometricObject* new_object = new GeometricObject)
		{
			new_object->geometry_type = type;
			new_object->object_id = new_object_id++;
			geometric_objects.push_back(new_object);
			current_object_index = geometric_objects.size() - 1;
			return new_object;
		}
		return nullptr;
	}

	GeometricObject* CreateNewBox(chaos::box3 const& creation_box)
	{
		if (GeometricObject* result = CreateNewGeometry(GeometryType::BOX))
		{
			result->object_box = creation_box;
			result->InsertIntoTree();			
			return result;
		}
		return nullptr;
	}

	GeometricObject * CreateNewSphere(chaos::sphere3 const & creation_sphere)
	{
		if (GeometricObject* result = CreateNewGeometry(GeometryType::SPHERE))
		{
			result->object_sphere = creation_sphere;
			result->InsertIntoTree();
			return result;
		}
		return nullptr;
	}

















	virtual bool TraverseInputReceiver(chaos::InputReceiverTraverser & in_traverser, chaos::InputDeviceInterface const * in_input_device) override
	{
		if (in_traverser.Traverse(&fps_view_controller, in_input_device))
			return true;
		if (GeometricObject* current_object = GetCurrentGeometricObject())
			if (current_object->TraverseInputReceiver(in_traverser, in_input_device))
				return true;
		return chaos::Window::TraverseInputReceiver(in_traverser, in_input_device);
	}

	virtual bool DoTick(float delta_time) override
	{
		// object selection with mouse
		if (IsImGuiMenuEnabled())
		{
			glm::ivec2 window_size = GetWindowSize();
			if (window_size.x > 0 && window_size.y > 0)
			{
				// get the viewport
				chaos::aabox2 viewport = GetRequiredViewport(window_size);

				// get the position of the cursor
				double x = 0.0;
				double y = 0.0;
				glfwGetCursorPos(glfw_window, &x, &y);

				// search whether their is an object intersection
				pointed_object = nullptr;

				chaos::ray3 r = GetRayFromMousePosition();

				float best_distance = std::numeric_limits<float>::max();

				for (auto const& obj : geometric_objects)
				{
					if (chaos::RayConvexGeometryIntersectionResult<3, float> intersections = obj->GetIntersection(r).FilterPositiveIntersectionOnly())
					{
						for (int i = 0; i < intersections.count ; ++i)
						{
							if (best_distance > intersections[i].t)
							{
								pointed_object = obj.get();
								best_distance = intersections[i].t;
							}
						}
					}
				}
			}
		}
		return true; // refresh
	}

	bool EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func)
	{
		if (chaos::Window::EnumerateKnownImGuiObjects(func))
			return true;

		char const* camera_menu_path = "Camera";	
		if (func("Camera", camera_menu_path, [this]()
		{
			return new ImGuiCameraInformationObject(camera_info);
		}))
		{
			return true;
		}
		return false;
	}

	virtual void OnInputLanguageChanged() override
	{
		chaos::Window::OnInputLanguageChanged();
		key_configuration = KeyConfiguration();
	}

protected:

	// an object for rendering
	chaos::shared_ptr<PrimitiveRenderer> primitive_renderer;
	/** the camera */
	chaos::FPSViewController fps_view_controller;
	/** the objects */
	std::vector<chaos::shared_ptr<GeometricObject>> geometric_objects;
	/** the selected object */
	std::optional<size_t> current_object_index;
	/** the new object id */
	size_t new_object_id = 0;

	/** the object to create */
	GeometryType current_creation_type = GeometryType::BOX;

	/** the icon for the sphere */
	chaos::shared_ptr<chaos::GPUTexture> sphere_icon_texture;
	/** the icon for the box */
	chaos::shared_ptr<chaos::GPUTexture> box_icon_texture;
	/** the icon for move */
	chaos::shared_ptr<chaos::GPUTexture> move_icon_texture;
	/** the icon for scale */
	chaos::shared_ptr<chaos::GPUTexture> scale_icon_texture;
	/** the icon for rotate */
	chaos::shared_ptr<chaos::GPUTexture> rotate_icon_texture;

	chaos::weak_ptr<GeometricObject> pointed_object;


	bool render_all = true;
	size_t node_visited = 0;
	size_t node_noplanecheck_visited = 0;
	size_t object_rendered = 0;

	CameraInfo camera_info;

	/** the camera displacement speed */
	static constexpr float CAMERA_SPEED = 400.0f;
	/** the distance at which object are being created */
	static constexpr float CREATE_OBJECT_DISTANCE = 100.0f;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}