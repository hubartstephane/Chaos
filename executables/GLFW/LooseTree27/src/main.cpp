#include "chaos/Chaos.h"

#include "PrimitiveRenderer.h"

static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class Tree27NodeBase;

using loose_tree_type = chaos::LooseTree27<3, Tree27NodeBase, chaos::ObjectPool>;
using loose_tree_node_type = loose_tree_type::node_type;

// =======================================================================

class KeyConfiguration
{
public:

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

class GeometricObject : public chaos::Object
{
public:

	chaos::box3 GetBoundingBox() const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE: return chaos::GetBoundingBox(sphere);
		case GeometryType::BOX: return box;
		default: assert(0);
		};
		return {};
	}

	void DrawPrimitive(PrimitiveRenderer * primitive_renderer, glm::vec4 const & color) const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE:
			primitive_renderer->DrawPrimitive(sphere, color, false);
			break;
		case GeometryType::BOX:
			primitive_renderer->DrawPrimitive(box, color, false, false);
			break;
		default:
			assert(0);
		};
	}

	chaos::RayConvexGeometryIntersectionResult<float, 3> GetIntersection(chaos::ray3 const& r) const
	{
		switch (geometry_type)
		{
		case GeometryType::SPHERE:
			return chaos::GetIntersection(r, sphere);
		case GeometryType::BOX:
			break;
		default:
			assert(0);
		};
		return {};
	}

	bool DisplaceObjectWithInputs(ActionType action_type, KeyConfiguration const & key_configuration, float delta_time)
	{
		bool result = false;

		if (action_type == ActionType::MOVE_OBJECT)
		{
			result |= MoveObjectWithInputs(key_configuration.move_object_negative_x, delta_time, { -1.0f,  0.0f,  0.0f });
			result |= MoveObjectWithInputs(key_configuration.move_object_positive_x, delta_time, { 1.0f,  0.0f,  0.0f });
			result |= MoveObjectWithInputs(key_configuration.move_object_negative_y, delta_time, { 0.0f, -1.0f,  0.0f });
			result |= MoveObjectWithInputs(key_configuration.move_object_positive_y, delta_time, { 0.0f,  1.0f,  0.0f });
			result |= MoveObjectWithInputs(key_configuration.move_object_negative_z, delta_time, { 0.0f,  0.0f, -1.0f });
			result |= MoveObjectWithInputs(key_configuration.move_object_positive_z, delta_time, { 0.0f,  0.0f,  1.0f });
		}
		else if (action_type == ActionType::SCALE_OBJECT)
		{
			//result |= ScaleObjectWithInputs(GLFW_KEY_R, delta_time, 1.0f);
			//result |= ScaleObjectWithInputs(GLFW_KEY_F, delta_time, -1.0f);
		}

		return result;
	}

protected:

	bool MoveObjectWithInputs(chaos::Key const & key, float delta_time, glm::vec3 const& direction)
	{
		if (IsKeyboardInput(key))
		{
			chaos::KeyboardAndMouseDevice* keyboard_and_mouse_device = chaos::KeyboardAndMouseDevice::GetInstance();
			if (keyboard_and_mouse_device == nullptr)
				return false;

			if (IsInputActive(keyboard_and_mouse_device->GetInputState(key)))
			{
				float final_speed = (IsInputActive(keyboard_and_mouse_device->GetInputState(chaos::Key::LEFT_SHIFT))) ? FAST_DISPLACEMENT_SPEED : DISPLACEMENT_SPEED;
				sphere.position += direction * delta_time * final_speed;
				box.position += direction * delta_time * final_speed;
				return true;
			}
		}
		return false;
	}

	bool ScaleObjectWithInputs(chaos::Key const& key, float delta_time, float direction)
	{
		if (IsKeyboardInput(key))
		{
			chaos::KeyboardAndMouseDevice* keyboard_and_mouse_device = chaos::KeyboardAndMouseDevice::GetInstance();
			if (keyboard_and_mouse_device == nullptr)
				return false;

			if (IsInputActive(keyboard_and_mouse_device->GetInputState(key)))
			{
				float final_scale_speed = (IsInputActive(keyboard_and_mouse_device->GetInputState(chaos::Key::LEFT_SHIFT))) ? SCALE_SPEED : FAST_SCALE_SPEED;

				sphere.radius = std::max(1.0f, sphere.radius + direction * final_scale_speed);
				return true;
			}
		}
		return false;
	}

public:

	GeometryType geometry_type = GeometryType::SPHERE;

	chaos::sphere3 sphere;

	chaos::box3    box;

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

	float fov = 60.0f;
	float far_plane = 5000.0f;
	float near_plane = 20.0f;

	glm::mat4x4 GetProjectionMatrix(glm::vec2 viewport_size) const
	{
		return glm::perspectiveFov(fov * (float)M_PI / 180.0f, float(viewport_size.x), float(viewport_size.y), near_plane, far_plane);
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

		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		primitive_renderer->projection      = GetProjectionMatrix(draw_params.viewport.size);
		primitive_renderer->world_to_camera = GetGlobalToCameraMatrix();
		primitive_renderer->render_context        = render_context;

		DrawGeometryObjects();
		DrawTree27();
		DrawAction();

		return true;
	}

	void OnDrawImGuiMenu(chaos::BeginImGuiMenuFunc begin_menu_func) override
	{
		chaos::Window::OnDrawImGuiMenu(begin_menu_func);

		begin_menu_func([this]
		{
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Show Help", nullptr, show_help, true))
					show_help = !show_help;
				ImGui::EndMenu();
			}
		});
	}

	void DrawTextItem(char const* title, chaos::Key const& key, bool enabled) const
	{
		if (enabled)
			ImGui::Text("%s      : %s", GetKeyName(key), title);
		else
			ImGui::TextDisabled("%s      : %s", GetKeyName(key), title);
	}

	void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		// the HELP
		if (show_help)
		{
			bool enabled = (GetCurrentGeometricObject() != nullptr);

			if (ImGui::Begin("help", &show_help, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
			{
				DrawTextItem("random scene", key_configuration.new_scene, true);
				DrawTextItem("next object", key_configuration.next_object, enabled);
				DrawTextItem("previous object", key_configuration.previous_object, enabled);
				DrawTextItem("delete object", key_configuration.delete_object, enabled);

				if (current_action_type == ActionType::MOVE_OBJECT)
				{
					DrawTextItem("move object -x", key_configuration.move_object_negative_x, enabled);
					DrawTextItem("move object +x", key_configuration.move_object_positive_x, enabled);
					DrawTextItem("move object -y", key_configuration.move_object_negative_y, enabled);
					DrawTextItem("move object +y", key_configuration.move_object_positive_y, enabled);
					DrawTextItem("move object -z", key_configuration.move_object_negative_z, enabled);
					DrawTextItem("move object +z", key_configuration.move_object_positive_z, enabled);
				}
				else if (current_action_type == ActionType::SCALE_OBJECT)
				{
					DrawTextItem("scale object -x", key_configuration.scale_object_negative_x, enabled);
					DrawTextItem("scale object +x", key_configuration.scale_object_positive_x, enabled);
					DrawTextItem("scale object -y", key_configuration.scale_object_negative_y, enabled);
					DrawTextItem("scale object +y", key_configuration.scale_object_positive_y, enabled);
					DrawTextItem("scale object -z", key_configuration.scale_object_negative_z, enabled);
					DrawTextItem("scale object +z", key_configuration.scale_object_positive_z, enabled);
				}
				ImGui::End();
			}
		}

		OnDrawToolbar();
	}

	void OnDrawToolbar()
	{
		if (ImGuiViewport* viewport = ImGui::GetMainViewport())
		{
			ImGui::SetNextWindowPos(ImVec2(15.0f, viewport->Size.y - 15.0f), ImGuiCond_FirstUseEver, ImVec2(0.0f, 1.0f));
			ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

			auto AddButton = [this](ActionType type, chaos::GPUTexture * texture, char const * tooltip, float base_cursor_y)
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

	void DrawTree27()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		object_tree.ForEachNode([this](loose_tree_node_type const* node)
		{
			primitive_renderer->DrawPrimitive(node->GetBoundingBox(), white, false, true);
		});
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	void DrawGeometryObjects()
	{
		for (auto& geometric_object : geometric_objects)
		{
			glm::vec4 color = white;
			if (pointed_object == geometric_object)
				color = red;
			else if (geometric_object == GetCurrentGeometricObject())
				color = blue;

			geometric_object->DrawPrimitive(primitive_renderer.get(), color);
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

	void DrawAction()
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

		fps_view_controller.fps_view.position.y = 30.0f;
		// create the very single sphere
		chaos::box3 creation_box;
		creation_box.position  = glm::vec3(0.0f, 0.0f, -200.0f);
		creation_box.half_size = glm::vec3(5.0f, 7.0f, 9.0f);

		CreateNewBox(creation_box);

		// create icons
		box_icon_texture = LoadTexture("BoxIcon.png");
		sphere_icon_texture = LoadTexture("SphereIcon.png");
		move_icon_texture = LoadTexture("MoveIcon.png");
		scale_icon_texture = LoadTexture("ScaleIcon.png");
		rotate_icon_texture = LoadTexture("RotateIcon.png");

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

	virtual bool OnMouseButtonImpl(chaos::MouseButtonEvent const &mouse_button_event) override
	{
		if (mouse_button_event.IsKeyPressed(chaos::Key::MOUSE_BUTTON_1))
		{
			if (IsImGuiMenuEnabled())
			{
				if (current_action_type == ActionType::CREATE_BOX)
				{
					CreateNewBox(GetBoxToCreateFromMousePosition());
				}
				else if (current_action_type == ActionType::CREATE_SPHERE)
				{
					CreateNewSphere(GetSphereToCreateFromMousePosition());
				}
				else if (current_action_type == ActionType::MOVE_OBJECT || current_action_type == ActionType::SCALE_OBJECT || current_action_type == ActionType::ROTATE_OBJECT)
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
				}
			}
		}
		return true;
	}

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator & in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		GeometricObject* current_object = GetCurrentGeometricObject();

		bool action_enabled = (current_object != nullptr);

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(key_configuration.next_object), "Next Object", action_enabled, [&]()
		{
			current_object_index = (current_object_index.value() + 1) % geometric_objects.size();
		}))
		{
			return true;
		}
		
		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(key_configuration.previous_object), "Previous Object", action_enabled, [&]()
		{
			current_object_index = (current_object_index.value() + geometric_objects.size() - 1) % geometric_objects.size();
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(key_configuration.delete_object), "Delete Object", action_enabled, [&]()
		{
			if (geometric_objects.size() > 0)
			{
				// remove the object from tree
				EraseObjectFromNode(current_object);
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

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(key_configuration.new_scene), "New Scene", [&]()
		{
			geometric_objects.clear();
			current_object_index = 0;
			object_tree.Clear();

			for (int i = 0; i < 200; ++i)
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
			result->box = creation_box;
			InsertObjectIntoNode(result, object_tree.GetOrCreateNode(creation_box));
			return result;
		}
		return nullptr;
	}

	GeometricObject * CreateNewSphere(chaos::sphere3 const & creation_sphere)
	{
		if (GeometricObject* result = CreateNewGeometry(GeometryType::SPHERE))
		{
			result->sphere = creation_sphere;
			InsertObjectIntoNode(result, object_tree.GetOrCreateNode(chaos::GetBoundingBox(creation_sphere)));
			return result;
		}
		return nullptr;
	}

	void InsertObjectIntoNode(GeometricObject* object, loose_tree_node_type * node)
	{
		object->node = node;
		object->node->objects.push_back(object);
	}

	void EraseObjectFromNode(GeometricObject* object)
	{
		object->node->objects.erase(std::ranges::find(object->node->objects, object));
		object_tree.DeleteNodeIfPossible(object->node);
		object->node = nullptr;
	}

	void OnObjectMoved(GeometricObject* object)
	{
		chaos::box3 new_box = object->GetBoundingBox();

		if (chaos::ComputeTreeNodeInfo(new_box) != object->node->GetNodeInfo()) // changement required
		{
			EraseObjectFromNode(object);
			InsertObjectIntoNode(object, object_tree.GetOrCreateNode(new_box));
		}
	}

	virtual bool TraverseInputReceiver(chaos::InputReceiverTraverser & in_traverser, chaos::InputDeviceInterface const * in_input_device) override
	{
		if (in_traverser.Traverse(&fps_view_controller, in_input_device))
			return true;
		return chaos::Window::TraverseInputReceiver(in_traverser, in_input_device);
	}

	virtual bool DoTick(float delta_time) override
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
		{
			// move object
			if (GeometricObject* current_object = GetCurrentGeometricObject())
			{
				if (current_object->DisplaceObjectWithInputs(current_action_type, key_configuration, delta_time))
					OnObjectMoved(current_object);
			}
		}

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
					if (chaos::RayConvexGeometryIntersectionResult<float, 3> intersections = obj->GetIntersection(r).FilterPositiveIntersectionOnly())
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

				// trace debugging information
				ImGui::Begin("Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::InputFloat("near_plane", &near_plane, 10.0f, 50.0f);
				ImGui::InputFloat("far_plane", &far_plane, 10.0f, 50.0f);
				ImGui::InputFloat("fov", &fov, 1.0f, 5.0f);
				ImGui::End();
			}
		}
		return true; // refresh
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

	ActionType current_action_type = ActionType::CREATE_BOX;

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

    loose_tree_type object_tree;

	chaos::weak_ptr<GeometricObject> pointed_object;

	bool show_help = true;

	/** the camera displacement speed */
	static constexpr float CAMERA_SPEED = 400.0f;
	/** the distance at which object are being created */
	static constexpr float CREATE_OBJECT_DISTANCE = 100.0f;

	KeyConfiguration key_configuration;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}