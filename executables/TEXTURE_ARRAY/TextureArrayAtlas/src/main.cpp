#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		if (bitmap_index >= bitmap_layouts.size())
			bitmap_index = bitmap_layouts.size() - 1;

		chaos::AtlasBitmapLayout const & layout = bitmap_layouts[bitmap_index];

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

								  // XXX : the scaling is used to avoid the near plane clipping
		chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		static float FOV = 60.0f;
		glm::mat4 projection      = glm::perspectiveFov(chaos::MathTools::DegreeToRadian(FOV), float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);

		main_uniform_provider.AddVariable("projection",      projection);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera);
		main_uniform_provider.AddVariable("local_to_world",  local_to_world);
		main_uniform_provider.AddVariable("texture_slice", (float)layout.bitmap_index);

		main_uniform_provider.AddTexture("material", atlas.GetTexture());

		glm::vec2 atlas_dimension = atlas.GetAtlasDimension();

		glm::vec2 entry_start = glm::vec2((float)layout.x, (float)layout.y);
		glm::vec2 entry_size  = glm::vec2((float)layout.width, (float)layout.height);
		glm::vec2 entry_end   = entry_start + entry_size;

		glm::vec2 bottomleft;
		glm::vec2 topright;

		bottomleft.x = entry_start.x / atlas_dimension.x;
		topright.x = entry_end.x / atlas_dimension.x;

		topright.y = 1.0f - (entry_start.y / atlas_dimension.y);
		bottomleft.y = 1.0f - (entry_end.y / atlas_dimension.y);  // BITMAP coordinates and OpenGL textures coordinates are inverted

		main_uniform_provider.AddVariable("bottomleft", bottomleft);
		main_uniform_provider.AddVariable("topright", topright);

		chaos::GPURenderParams render_params;
		mesh_box->DisplayWithProgram(program_box.get(), render_context, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		mesh_box    = nullptr;
		program_box = nullptr;
		atlas.Clear();

		chaos::Window::Finalize();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}


	bool LoadTextureArray(boost::filesystem::path const & resources_path)
	{
		return atlas.LoadAtlas(resources_path / "Atlas" / "MyAtlas.json");
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();

		// load texture
		if (!LoadTextureArray(resources_path))
			return false;

		atlas.CollectEntries(bitmap_layouts, true);
		if (bitmap_layouts.size() == 0)
			return false;

		// load programs
		program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
		if (program_box == nullptr)
			return false;

		// create meshes
		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		chaos::GPUMultiMeshGenerator generators;
		generators.AddGenerator(new chaos::GPUBoxMeshGenerator(b), mesh_box);

		if (!generators.GenerateMeshes(GetGPUDevice()))
			return false;

		// place camera
		fps_view_controller.fps_view.position.y = 0.0f;
		fps_view_controller.fps_view.position.z = 10.0f;

		// initial display
		imgui_user_message.AddLine("Draw a box with a texture array : \n  Use +/- to change slice.\n  Array composed of images with different size and BPP");

		return true;
	}

	virtual bool TraverseInputReceiver(chaos::InputReceiverTraverser & in_traverser, chaos::InputDeviceInterface const * in_input_device) override
	{
		if (in_traverser.Traverse(&fps_view_controller, in_input_device))
			return true;
		return chaos::Window::TraverseInputReceiver(in_traverser, in_input_device);
	}

	virtual bool EnumerateInputActions(chaos::InputActionEnumerator & in_action_enumerator, chaos::EnumerateInputActionContext in_context) override
	{
		if (in_action_enumerator.CheckAndProcess(KeyPressed(chaos::Key::KP_ADD), "Next Bitmap Index", [this]()
		{
			++bitmap_index;
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(KeyPressed(chaos::Key::KP_SUBTRACT), "Previous Bitmap Index", [this]()
		{
			if (bitmap_index > 0)
				--bitmap_index;
		}))
		{
			return true;
		}

		return chaos::Window::EnumerateInputActions(in_action_enumerator, in_context);
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

	// rendering for the box
	chaos::shared_ptr<chaos::GPUMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	chaos::GPUAtlas atlas;

	size_t bitmap_index = 0;
	std::vector<chaos::AtlasBitmapLayout> bitmap_layouts;

	chaos::FPSViewController fps_view_controller;

	chaos::ImGuiUserMessageObject imgui_user_message;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}