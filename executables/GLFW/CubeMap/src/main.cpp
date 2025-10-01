#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator & in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(chaos::KeyboardButton::KP_ADD), "Next CubeMap", [this]()
		{
			ChangeCubeMap(cubemap_index + 1);
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(chaos::KeyboardButton::KP_SUBTRACT), "Previous CubeMap", [this]()
		{
			ChangeCubeMap(cubemap_index - 1);
		}))
		{
			return true;
		}

		return chaos::Window::EnumerateInputActions(in_action_enumerator, in_context);
	}

	void ChangeCubeMap(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateCubeMap(index);
		if (new_texture != nullptr)
		{
			cubemap_index = index;
			texture = new_texture;
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateCubeMap(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> result;

		boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();

		chaos::CubeMapImages cubemap;
		if (index == 0)
		{
			cubemap = chaos::CubeMapTools::LoadSingleCubeMap((resources_path / "violentdays_large.jpg"));
		}
		else if (index == 1)
		{
			cubemap = chaos::CubeMapTools::LoadSingleCubeMap((resources_path / "originalcubecross.png"));
		}
		else if (index == 2)
		{
			boost::filesystem::path p = resources_path / "Maskonaive";

			boost::filesystem::path left_image = p / "negx.jpg";
			boost::filesystem::path front_image = p / "posz.jpg";
			boost::filesystem::path right_image = p / "posx.jpg";
			boost::filesystem::path back_image = p / "negz.jpg";
			boost::filesystem::path top_image = p / "posy.jpg";
			boost::filesystem::path bottom_image = p / "negy.jpg";

			cubemap = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 3)
		{
			boost::filesystem::path p = resources_path / "cubemap";

			boost::filesystem::path left_image = p / "nx.jpg";
			boost::filesystem::path front_image = p / "pz.jpg";
			boost::filesystem::path right_image = p / "px.jpg";
			boost::filesystem::path back_image = p / "nz.jpg";
			boost::filesystem::path top_image = p / "py.jpg";
			boost::filesystem::path bottom_image = p / "ny.jpg";

			cubemap = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 4)
		{
			boost::filesystem::path p = resources_path / "MilkyWay";

			boost::filesystem::path left_image = p / "dark-s_nx.jpg";
			boost::filesystem::path front_image = p / "dark-s_pz.jpg";
			boost::filesystem::path right_image = p / "dark-s_px.jpg";
			boost::filesystem::path back_image = p / "dark-s_nz.jpg";
			boost::filesystem::path top_image = p / "dark-s_py.jpg";
			boost::filesystem::path bottom_image = p / "dark-s_ny.jpg";

			cubemap = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, right_image, top_image, bottom_image, front_image, back_image);
		}
		else if (index == 5)
		{
			cubemap = chaos::CubeMapTools::LoadSingleCubeMap((resources_path / "space.png"));
		}

		if (!cubemap.IsEmpty())
			return chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(&cubemap);

		return nullptr;
	}

	virtual bool OnDraw(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
	#if 0
		if (query->IsEnded())
		{
			bool available = query->IsResultAvailable();

			GLint result = query->GetResult(true);

			GLint64 result64 = query->GetResult64(true);
		}
		#endif

		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV = 60.0f;
		glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 local_to_world_matrix = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("projection", projection_matrix);
		main_uniform_provider.AddVariable("local_to_world", local_to_world_matrix);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera_matrix);
		main_uniform_provider.AddTexture("material", texture);

		// XXX : conditional rendering may fail if all conditions are not meet
		//       the rendering will be processed has normal
		glEnable(GL_STENCIL_TEST);

		GLint ref = 1;
		GLuint mask = 0xFF;
		glStencilFunc(GL_ALWAYS, ref, mask); // always pass stencil

		GLenum stencil_fail = GL_KEEP;
		GLenum depth_fail = GL_KEEP;
		GLenum depth_pass = GL_REPLACE;
		glStencilOp(stencil_fail, depth_fail, depth_pass); // as soon as a pixel is rendered, REPLACE the stencil buffer by REF = 1

		// XXX : the stencil is here to ensure that the debug strings is not erased by the sky box
		//       (debug string needs to be rendered first so it can use the conditional rendering from previous frame)
		//query->BeginConditionalRendering(true, false);
		//imgui_user_message.Display(render_context, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));
		//query->EndConditionalRendering();

		// XXX : render the cubemap. Use previous frame query for conditinal rendering
		ref = 0;
		mask = 0xFF;
		glStencilFunc(GL_EQUAL, ref, mask); // stencil must be 0 to render a pixel (debug string is not on the screen)

		stencil_fail = GL_KEEP;
		depth_fail = GL_KEEP;
		depth_pass = GL_KEEP;
		glStencilOp(stencil_fail, depth_fail, depth_pass); // do not modify the stencil anymore

		chaos::GPURenderParams render_params;

		//query->BeginQuery();
		mesh->DisplayWithProgram(program.get(), render_context, &main_uniform_provider, render_params);
		//query->EndQuery();

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh = nullptr;
		texture = nullptr;
		//query = nullptr;

		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::Application* application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();

		imgui_user_message.AddLine("Press +/- to change cubemap");
		//imgui_user_message.AddLine("If the cubemap has no pixel on screen, ");
		//imgui_user_message.AddLine("this text will disappear (conditional rendering)");

		texture = GenerateCubeMap(0);
		if (texture == nullptr)
			return false;

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader_cube.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / "vertex_shader.txt");

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUBoxMeshGenerator(b).GenerateMesh(GetGPUDevice());
		if (mesh == nullptr)
			return false;

		//query = new chaos::GLSamplesPassedQuery(this);
		//if (query == nullptr)
		//	return false;

		return true;
	}

	virtual bool TraverseInputReceiver(chaos::InputReceiverTraverser & in_traverser, chaos::InputDeviceInterface const * in_input_device) override
	{
		if (in_traverser.Traverse(&fps_view_controller, in_input_device))
			return true;
		return chaos::Window::TraverseInputReceiver(in_traverser, in_input_device);
	}

	virtual bool OnMouseButtonImpl(chaos::MouseButtonEvent const &mouse_button_event) override
	{
		if (mouse_button_event.IsKeyReleased(chaos::MouseButton::BUTTON_2))
		{
			imgui_user_message.AddLine("HelloWorld");
			return true;
		}
		return false;
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

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	//chaos::shared_ptr<chaos::GPUQuery> query;

	chaos::FPSViewController fps_view_controller;

	chaos::ImGuiUserMessageObject imgui_user_message;

	int cubemap_index = 0;
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}


