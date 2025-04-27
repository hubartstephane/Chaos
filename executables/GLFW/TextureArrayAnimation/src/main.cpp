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

		static int k = 0;
		char const * bitmap_name = (k == 0)? "walking" : "moving";

		chaos::BitmapAtlas::BitmapInfo const * info = atlas->GetBitmapInfo(bitmap_name, true);
		if (info == nullptr)
			return true;

		float frame_duration = info->GetFrameDuration();
		if (frame_duration <= 0.0f)
			frame_duration = 1.0f / 10.0f;

		int image = (int)(time / frame_duration);

		chaos::BitmapAtlas::BitmapLayout layout = info->GetAnimationLayout(image, chaos::WrapMode::WRAP);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

								  // XXX : the scaling is used to avoid the near plane clipping
		chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		static float FOV = 60.0f;
		glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);

		main_uniform_provider.AddVariable("projection",      projection);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera);
		main_uniform_provider.AddVariable("local_to_world",  local_to_world);
		main_uniform_provider.AddVariable("texture_slice", (float)layout.bitmap_index);
		main_uniform_provider.AddTexture("material", atlas->GetTexture());

		glm::vec2 atlas_dimension = atlas->GetAtlasDimension();

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
		atlas = nullptr;
		chaos::Window::Finalize();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}


	chaos::GPUAtlas * LoadTextureArray(boost::filesystem::path const & resources_path)
	{
		chaos::GPUAtlasGenerator generator;

		chaos::BitmapAtlas::AtlasInput input;
		input.AddBitmap(resources_path / "moving_gif.gif", "moving", 0);
		input.AddBitmap(resources_path / "walking_5x2s0.png", "walking", 1);

		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());
		return generator.ComputeResult(input, params);
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
		atlas = LoadTextureArray(resources_path);
		if (atlas == nullptr)
			return false;

		// load programs
		program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
		if (program_box == nullptr)
			return false;

		// create meshes
		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		chaos::GPUMultiMeshGenerator generators;
		generators.AddGenerator(new chaos::GPUBoxMeshGenerator(b), mesh_box);

		if (!generators.GenerateMeshes(GetDevice()))
			return false;

		// place camera
		fps_view_controller.fps_view.position.y = 0.0f;
		fps_view_controller.fps_view.position.z = 10.0f;

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		time += delta_time;
		fps_view_controller.Tick(glfw_window, delta_time);

		return true; // refresh
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & key_event) override
	{
		if (key_event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			++bitmap_index;
			return true;
		}
		else if (key_event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			if (bitmap_index > 0)
				--bitmap_index;
			return true;
		}
		return chaos::Window::OnKeyEventImpl(key_event);
	}

protected:

	// rendering for the box
	chaos::shared_ptr<chaos::GPUMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	chaos::shared_ptr<chaos::GPUAtlas> atlas;

	size_t bitmap_index = 0;

	chaos::FPSViewController fps_view_controller;

	double time = 0.0;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}