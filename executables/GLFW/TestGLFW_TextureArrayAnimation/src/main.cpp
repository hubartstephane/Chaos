#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/GPUSimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/BitmapAtlasInput.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/GPUSimpleMesh.h>
#include <chaos/GPUMultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderParams.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
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

		double frame_time = info->GetFrameTime();
		if (frame_time <= 0.0)
			frame_time = 1.0 / 10.0;

		int image = (int)(time / frame_time);
		
		chaos::BitmapAtlas::BitmapLayout layout = info->GetAnimationLayout(image, chaos::BitmapAtlas::GetBitmapLayoutFlag::wrap);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

								  // XXX : the scaling is used to avoid the near plane clipping
		chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		static float FOV = 60.0f;
		glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, 2.0f * viewport.half_size.x, 2.0f * viewport.half_size.y, 1.0f, far_plane);
		glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

		chaos::GPUProgramProvider uniform_provider;

		uniform_provider.AddVariableValue("projection",      projection);
		uniform_provider.AddVariableValue("world_to_camera", world_to_camera);
		uniform_provider.AddVariableValue("local_to_world",  local_to_world);
		uniform_provider.AddVariableValue("texture_slice", (float)layout.bitmap_index);

		uniform_provider.AddVariableTexture("material", atlas->GetTexture());

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

		uniform_provider.AddVariableValue("bottomleft", bottomleft);
		uniform_provider.AddVariableValue("topright", topright);

		chaos::GPURenderParams render_params;
		mesh_box->Render(renderer, program_box.get(), &uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		mesh_box    = nullptr;
		program_box = nullptr;
		atlas = nullptr;
		chaos::MyGLFW::Window::Finalize();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}


	chaos::BitmapAtlas::TextureArrayAtlas * LoadTextureArray(boost::filesystem::path const & resources_path)
	{
		chaos::BitmapAtlas::TextureArrayAtlasGenerator generator;

		chaos::BitmapAtlas::AtlasInput input;
		input.AddBitmap(resources_path / "moving_gif.gif", "moving", 0);
		input.AddBitmap(resources_path / "walking_5x2s0.png", "walking", 1);

		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());
		return generator.ComputeResult(input, params);
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
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
		generators.AddGenerator(new chaos::GPUCubeMeshGenerator(b), mesh_box);

		if (!generators.GenerateMeshes())
			return false;

		// place camera
		fps_view_controller.fps_controller.position.y = 0.0f;
		fps_view_controller.fps_controller.position.z = 10.0f;

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 0;
		hints.decorated = 1;
	}

	virtual bool Tick(double delta_time) override
	{
		time += delta_time;
		fps_view_controller.Tick(glfw_window, delta_time);

		return true; // refresh
	}

	virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{
			++bitmap_index;
			return true;
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			if (bitmap_index > 0)
				--bitmap_index;
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}

protected:

	// rendering for the box  
	chaos::shared_ptr<chaos::GPUSimpleMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> atlas;

	size_t bitmap_index = 0;

	chaos::FPSViewInputController fps_view_controller;

	double time = 0.0;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 1200;
	params.height = 600;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


