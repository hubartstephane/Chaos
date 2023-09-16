#include "chaos/Chaos.h"

#include "PrimitiveRenderer.h"

static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class GeometricObject : public chaos::Object
{
public:

	chaos::sphere3 sphere;

	size_t object_index = 0;

};



class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV = 60.0f;
		primitive_renderer->projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		primitive_renderer->world_to_camera = fps_view_controller.GlobalToLocal();
		primitive_renderer->renderer        = renderer;

		DrawGeometryObjects();

		return true;
	}

	void DrawGeometryObjects()
	{
		for (auto& geometric_object : geometric_objects)
		{
			glm::vec4 color;
			if (geometric_object.get() == GetCurrentGeometricObject())
				color = red;
			else
				color = white;

			primitive_renderer->GPUDrawPrimitive(geometric_object->sphere, color, false);
		}
	}

	virtual void Finalize() override
	{
		primitive_renderer = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		primitive_renderer = new PrimitiveRenderer();
		if (primitive_renderer == nullptr)
			return false;
		if (!primitive_renderer->Initialize())
			return false;

		fps_view_controller.movement_speed.back = camera_speed;
		fps_view_controller.movement_speed.down = camera_speed;
		fps_view_controller.movement_speed.up = camera_speed;
		fps_view_controller.movement_speed.forward = camera_speed;
		fps_view_controller.movement_speed.strafe = camera_speed;

		for (int i = 0; i < 20; ++i)
		{
			chaos::sphere3 creation_sphere;
			creation_sphere.position.x = chaos::MathTools::RandFloat(-200.0f, +200.0f);
			creation_sphere.position.y = chaos::MathTools::RandFloat(-200.0f, +200.0f);
			creation_sphere.position.z = chaos::MathTools::RandFloat(-200.0f, +200.0f);
			creation_sphere.radius = chaos::MathTools::RandFloat(1.0f, 20.0f);

			CreateNewObject(creation_sphere);
		}




		return true;
	}

	GeometricObject* GetCurrentGeometricObject()
	{
		return (geometric_objects.size() == 0) ? nullptr : geometric_objects[current_object_index].get();
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		// change the current object if any
		if (GeometricObject* geometric_object = GetCurrentGeometricObject())
		{
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


		}

		// create a new object from current object or nothing
		if (event.IsKeyPressed(GLFW_KEY_SPACE))
		{
			CreateNewObject();
			return true;
		}
		return false;
	}

	GeometricObject *CreateNewObject()
	{
		chaos::sphere3 creation_sphere;
		creation_sphere.position = glm::vec3(0.0f);
		creation_sphere.radius = 100.0f;

		if (GeometricObject* geometric_object = GetCurrentGeometricObject())
		{
			creation_sphere = geometric_object->sphere;
		}

		return CreateNewObject(creation_sphere);
	}

	GeometricObject * CreateNewObject(chaos::sphere3 const & creation_sphere)
	{
		if (GeometricObject* new_object = new GeometricObject)
		{
			new_object->sphere = creation_sphere;
			new_object->object_index = new_object_index++;
			geometric_objects.push_back(new_object);
			current_object_index = geometric_objects.size() - 1;
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













	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		if (GeometricObject* object = GetCurrentGeometricObject())
		{
			MoveObject(object, GLFW_KEY_A, delta_time, {-1.0f,  0.0f,  0.0f });
			MoveObject(object, GLFW_KEY_D, delta_time, { 1.0f,  0.0f,  0.0f });
			MoveObject(object, GLFW_KEY_Q, delta_time, { 0.0f, -1.0f,  0.0f });
			MoveObject(object, GLFW_KEY_E, delta_time, { 0.0f,  1.0f,  0.0f });
			MoveObject(object, GLFW_KEY_W, delta_time, { 0.0f,  0.0f, -1.0f });
			MoveObject(object, GLFW_KEY_S, delta_time, { 0.0f,  0.0f,  1.0f });
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

	float fast_displacement_speed = 500.0f;

	float scale_speed = 1.5f;

	float fast_scale_speed = 3.0f;

	float camera_speed = 100.0f;
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