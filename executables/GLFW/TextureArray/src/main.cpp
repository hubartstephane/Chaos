#include <chaos/Chaos.h> 

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS2(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

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
		uniform_provider.AddVariableValue("texture_slice",   (float)texture_slice);
		uniform_provider.AddVariableTexture("material", texture);

		chaos::GPURenderParams render_params;
		mesh_box->Render(renderer, program_box.get(), &uniform_provider, render_params);

		debug_display.Display(renderer, (int)(2.0f * viewport.half_size.x), (int)(2.0f * viewport.half_size.y));

		return true;
	}

	virtual void Finalize() override
	{
		ReleaseBitmaps();

		mesh_box    = nullptr;
		program_box = nullptr;
		texture     = nullptr;

		debug_display.Finalize();
		chaos::Window::Finalize();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config, config_path))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path font_path = resources_path / "font.png";

		// initialize debug font display 
		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path               = font_path;
		debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line   = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width            = 20;
		debug_params.spacing                    = glm::ivec2( 0, 0);
		debug_params.crop_texture               = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		// load the initial bitmaps
		if (!LoadBitmaps(resources_path))
			return false;

		// generate the texture
		ChangePixelFormat(0);
		if (texture == nullptr)
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

		// initial display
		debug_display.AddLine("Draw a box with a texture array :");
		debug_display.AddLine("  Use +/- to change slice.");
		debug_display.AddLine("  Use 1/2 to change pixel format.");

		return true;
	}

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 0;
		hints.decorated = 1;
	}

	virtual bool Tick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateTextureArray(chaos::PixelFormat pixel_format)
	{
		if (!pixel_format.IsValid())
			return nullptr;

		chaos::TextureArrayGenerator generator;
		for (size_t i = 0 ; i < bitmaps.size() ; ++i)
			generator.AddGenerator(new chaos::TextureArraySliceGenerator_Image(bitmaps[i], false));

		chaos::PixelFormatMergeParams merge_params;
		merge_params.pixel_format = pixel_format;

		chaos::shared_ptr<chaos::GPUTexture> result = generator.GenTextureObject(merge_params);
		if (result != nullptr)
			texture_slice_count = (int)bitmaps.size();

		return result;
	}

	void ChangePixelFormat(int delta)
	{
		std::vector<chaos::PixelFormat> pixel_formats =
		{
			chaos::PixelFormat::Gray,
			chaos::PixelFormat::GrayFloat,
			chaos::PixelFormat::BGR,
			chaos::PixelFormat::BGRA,
			chaos::PixelFormat::RGBFloat,
			chaos::PixelFormat::RGBAFloat
		};

		int next_format = (current_pixel_format + delta + int(pixel_formats.size())) % int(pixel_formats.size());

		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateTextureArray(pixel_formats[next_format]);
		if (new_texture != nullptr)
		{
			texture = new_texture;
			current_pixel_format = next_format;
		}		
	}

	void ChangeSlice(int delta)
	{
		texture_slice = (texture_slice + delta + texture_slice_count) % texture_slice_count;	
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(GLFW_KEY_KP_ADD))
		{
			ChangeSlice(+1);
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_SUBTRACT))
		{
			ChangeSlice(-1);
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_1))
		{
			ChangePixelFormat(+1);
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_2))
		{
			ChangePixelFormat(-1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	bool LoadBitmaps(boost::filesystem::path const & resources_path)
	{
		ReleaseBitmaps();

		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = chaos::FileTools::GetDirectoryIterator(resources_path / "images"); it != end; ++it)
		{
			FIBITMAP * bitmap = chaos::ImageTools::LoadImageFromFile(it->path());
			if (bitmap == nullptr)
				continue;
			bitmaps.push_back(bitmap);		
		}  

		return (bitmaps.size() > 0);
	}

	void ReleaseBitmaps()
	{
		for (FIBITMAP * bitmap : bitmaps)
			FreeImage_Unload(bitmap);
		bitmaps.clear();		
	}

protected:

	// the bitmap used to generate the slices
	std::vector<FIBITMAP *> bitmaps;

	int current_pixel_format = 0;


	// rendering for the box  
	chaos::shared_ptr<chaos::GPUSimpleMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	int texture_slice = 0;
	int texture_slice_count = 0;

	chaos::FPSViewInputController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 1200;
	params.height = 600;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


