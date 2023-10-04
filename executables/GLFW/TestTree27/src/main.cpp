#include "chaos/Chaos.h"

#include "PrimitiveRenderer.h"

static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

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

	chaos::sphere3 sphere;

	size_t object_index = 0;

	chaos::Tree27<3, Tree27NodeBase>::node_type * node = nullptr;
};

// =======================================================================

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	float fov = 60.0f;
	float far_plane = 5000.0f;
	float near_plane = 20.0f;
	float create_object_distance = 50.0f;

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

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		primitive_renderer->projection      = GetProjectionMatrix(draw_params.viewport.size);
		primitive_renderer->world_to_camera = GetGlobalToCameraMatrix();
		primitive_renderer->renderer        = renderer;

		DrawGeometryObjects();
		DrawTree27();

		return true;
	}

	void OnDrawWindowImGuiMenu() override
	{
		chaos::Window::OnDrawWindowImGuiMenu();

		chaos::ImGuiDrawableInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this]()
		{
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Show Help", nullptr, show_help, true))
					show_help = !show_help;
				ImGui::EndMenu();
			}
		});
	}

	void OnDrawWindowImGuiContent() override
	{
		if (show_help)
		{
			ImGui::Begin("help", &show_help);
			ImGui::Text("+      : next object");
			ImGui::Text("-      : previous object");
			ImGui::Text("space  : new object");
			ImGui::Text("delete : delete");
			ImGui::Text("y      : new random scene");
			ImGui::Text("z      : move forward");
			ImGui::Text("s      : move backward");
			ImGui::Text("q      : move left");
			ImGui::Text("d      : move right");
			ImGui::Text("a      : move down");
			ImGui::Text("e      : move up");
			ImGui::Text("r      : scale up");
			ImGui::Text("f      : scale down");
			ImGui::Text("shift  : speed");
			ImGui::End();
		}
	}

	void DrawTree27()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		object_tree.ForEachNode([this](chaos::Tree27<3, Tree27NodeBase>::node_type const* node)
		{
			primitive_renderer->GPUDrawPrimitive(node->GetBoundingBox(), white, false, true);
		});
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	void DrawGeometryObjects()
	{
		for (auto& geometric_object : geometric_objects)
		{
			if (geometric_object.get() == GetCurrentGeometricObject())
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightIncreaseSize(geometric_object->sphere), red, false);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else
			{
				glm::vec4 color = (pointed_object == geometric_object) ? blue : white;
				primitive_renderer->GPUDrawPrimitive(geometric_object->sphere, color, false);
			}
		}
	}

	virtual void Finalize() override
	{
		primitive_renderer = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override
	{
		// super
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;
		// create the renderer
		primitive_renderer = new PrimitiveRenderer();
		if (primitive_renderer == nullptr)
			return false;
		if (!primitive_renderer->Initialize())
			return false;
		// update the camera speed
		fps_view_controller.movement_speed.back = camera_speed;
		fps_view_controller.movement_speed.down = camera_speed;
		fps_view_controller.movement_speed.up = camera_speed;
		fps_view_controller.movement_speed.forward = camera_speed;
		fps_view_controller.movement_speed.strafe = camera_speed;

		fps_view_controller.fps_controller.position.y = 30.0f;
		// create the very single sphere
		chaos::sphere3 creation_sphere;
		creation_sphere.position = glm::vec3(0.0f, 0.0f, -200.0f);
		creation_sphere.radius = 10.0f;
		CreateNewObject(creation_sphere);

		return true;
	}

	GeometricObject* GetCurrentGeometricObject()
	{
		return (geometric_objects.size() == 0) ? nullptr : geometric_objects[current_object_index].get();
	}

	bool GetImGuiMenuMode() const
	{
		if (chaos::WindowApplication const* application = chaos::Application::GetInstance())
		{
			return application->GetImGuiMenuMode();
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

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_PRESS)
		{
			if (GetImGuiMenuMode())
			{
				chaos::ray3 r = GetRayFromMousePosition();

				glm::vec3 center = r.position + create_object_distance * r.direction;

				chaos::sphere3 creation_sphere;
				creation_sphere.position = center;
				creation_sphere.radius = 10.0f;
				CreateNewObject(creation_sphere);
			}
		}
		return true;
	}


	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		// change the current object if any
		if (GeometricObject* current_object = GetCurrentGeometricObject())
		{
			// change current selection
			if (event.IsKeyPressed(GLFW_KEY_KP_ADD))
			{
				current_object_index = (current_object_index + 1) % geometric_objects.size();
				return true;
			}
			else if (event.IsKeyPressed(GLFW_KEY_KP_SUBTRACT))
			{
				current_object_index = (current_object_index + geometric_objects.size() - 1) % geometric_objects.size();
				return true;
			}

			// create a new object from current object or nothing
			if (event.IsKeyPressed(GLFW_KEY_SPACE))
			{
				for (auto& object : geometric_objects)
					if (current_object != object.get() && object->sphere == current_object->sphere)
						return false;

				CreateNewObject(current_object->sphere);
				return true;
			}

			if (event.IsKeyPressed(GLFW_KEY_DELETE))
			{
				if (geometric_objects.size() > 1)
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
					if (current_object_index > 0)
						--current_object_index; // while there is always at least one object in the array, this is always valid
				}

				return true;
			}

			if (event.IsKeyPressed(GLFW_KEY_Y))
			{
				geometric_objects.clear();
				current_object_index = 0;
				object_tree.Clear();

				for (int i = 0; i < 200; ++i)
				{
					chaos::sphere3 creation_sphere;
					creation_sphere.position.x = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);
					creation_sphere.position.y = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);
					creation_sphere.position.z = chaos::MathTools::RandFloat(-1000.0f, 1000.0f);

					creation_sphere.radius = chaos::MathTools::RandFloat(2.0f, 50.0f);
					CreateNewObject(creation_sphere);
				}
				return true;
			}
		}

		return chaos::Window::OnKeyEventImpl(event);
	}

	GeometricObject * CreateNewObject(chaos::sphere3 const & creation_sphere)
	{
		if (GeometricObject* new_object = new GeometricObject)
		{
			new_object->sphere = creation_sphere;
			new_object->object_index = new_object_index++;
			geometric_objects.push_back(new_object);
			current_object_index = geometric_objects.size() - 1;

			InsertObjectIntoNode(new_object, object_tree.GetOrCreateNode(chaos::GetBoundingBox(new_object->sphere)));

			return new_object;
		}
		return nullptr;
	}

	bool MoveObject(GeometricObject* object, int key, float& target_value, float delta_time, float speed, float fast_speed)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ? fast_speed : speed;
			target_value += final_speed * delta_time;
			return true;
		}
		return false;
	}


	bool MoveObject(GeometricObject* object, int key, float delta_time, glm::vec3 const & direction)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ?
				fast_displacement_speed:
				displacement_speed;

			object->sphere.position += glm::vec3(fps_view_controller.LocalToGlobal() * glm::vec4(direction, 0.0f)) * delta_time * final_speed;
			return true;
		}
		return false;
	}



	bool ScaleObject(GeometricObject* object, int key, float delta_time, float direction)
	{
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			float final_scale_speed = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) ?
				fast_scale_speed :
				scale_speed;

			object->sphere.radius = std::max(1.0f, object->sphere.radius + direction * final_scale_speed);
			return true;
		}
		return false;
	}


	void InsertObjectIntoNode(GeometricObject* object, chaos::Tree27<3, Tree27NodeBase>::node_type * node)
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
		chaos::box3 new_box = chaos::GetBoundingBox(object->sphere);

		if (chaos::ComputeTreeNodeInfo(new_box) != object->node->GetNodeInfo()) // changement required
		{
			EraseObjectFromNode(object);
			InsertObjectIntoNode(object, object_tree.GetOrCreateNode(new_box));
		}
	}

	virtual bool DoTick(float delta_time) override
	{
		// move camera
		//if (!GetImGuiMenuMode())
		fps_view_controller.Tick(glfw_window, delta_time);

		// move object
		if (GeometricObject* current_object = GetCurrentGeometricObject())
		{
			bool moved = false;

			moved |= MoveObject(current_object, GLFW_KEY_A, delta_time, {-1.0f,  0.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_D, delta_time, { 1.0f,  0.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_Q, delta_time, { 0.0f, -1.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_E, delta_time, { 0.0f,  1.0f,  0.0f });
			moved |= MoveObject(current_object, GLFW_KEY_W, delta_time, { 0.0f,  0.0f, -1.0f });
			moved |= MoveObject(current_object, GLFW_KEY_S, delta_time, { 0.0f,  0.0f,  1.0f });

			moved |= ScaleObject(current_object, GLFW_KEY_R, delta_time,  1.0f);
			moved |= ScaleObject(current_object, GLFW_KEY_F, delta_time, -1.0f);

			if (moved)
				OnObjectMoved(current_object);
		}

		// object selection with mouse
		if (glfwGetInputMode(glfw_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
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
					if (chaos::RaySphereIntersectionResult<float, 3> intersections = chaos::GetIntersection(r, obj->sphere).FilterPositiveIntersectionOnly())
					{
						for (int i = 0; i < intersections.intersection_count; ++i)
						{
							if (best_distance > intersections.data[i]->t)
							{
								pointed_object = obj.get();
								best_distance = intersections.data[i]->t;
							}
						}
					}
				}

				// trace debugging information
				ImGui::Begin("Information");
				ImGui::Text("cursor            : (%0.3f, %0.3f)", (float)x, (float)y);
				ImGui::Text("window   size     : (%0.3f, %0.3f)", (float)window_size.x, (float)window_size.y);
				ImGui::Text("viewport position : (%0.3f, %0.3f)", (float)viewport.position.x, (float)viewport.position.y);
				ImGui::Text("viewport size     : (%0.3f, %0.3f)", (float)viewport.size.x, (float)viewport.size.y);
				ImGui::InputFloat("create_object_distance", &create_object_distance);
				ImGui::InputFloat("near_plane", &near_plane, 10.0f, 50.0f);
				ImGui::InputFloat("far_plane", &far_plane, 10.0f, 50.0f);
				ImGui::InputFloat("fov", &fov, 1.0f, 5.0f);
				ImGui::End();
			}
		}

		return true; // refresh
	}

protected:

	// an object for debug rendering
	chaos::shared_ptr<PrimitiveRenderer> primitive_renderer;

	chaos::FPSViewInputController fps_view_controller;

	std::vector<chaos::shared_ptr<GeometricObject>> geometric_objects;

	size_t current_object_index = 0;

	size_t new_object_index = 0;

	float displacement_speed = 100.0f;

	float fast_displacement_speed = 200.0f;

	float scale_speed = 1.5f;

	float fast_scale_speed = 3.0f;

	float camera_speed = 400.0f;

	chaos::Tree27<3, Tree27NodeBase> object_tree;

	chaos::weak_ptr<GeometricObject> pointed_object;

	bool show_help = true;
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 800;
	create_params.height = 800;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}