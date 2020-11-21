#include <chaos/Chaos.h> 

// --------------------------------------------------------------------

class WindowOpenGLTest : public chaos::Window
{

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		static float FOV = 60.0f;
		glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, 2.0f * viewport.half_size.x, 2.0f * viewport.half_size.y, 1.0f, far_plane);
		glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

		class MyProvider : public chaos::GPUProgramProviderBase
		{
		protected: 

			virtual bool DoProcessAction(char const * name, chaos::GPUProgramAction & action, chaos::GPUProgramProviderBase const * top_level) const override
			{
				if (name != nullptr && chaos::StringTools::Stricmp("local_to_cam", name) == 0)
				{
					glm::mat4 translate_mat;
					glm::mat4 scale_mat;

					if (top_level->GetValue<glm::mat4>("translate_mat", translate_mat))
						if (top_level->GetValue<glm::mat4>("scale_mat", scale_mat))
							return action.Process("local_to_cam", translate_mat * scale_mat, this);
				}         
				return false;
			}
		};

		float w = (float)window_size.x;
		float h = (float)window_size.y;
		glm::vec3 scale = glm::vec3(2.0f / w, 2.0f / h, 1.0f);
		glm::vec3 tr = glm::vec3(-1.0f, -1.0f, 0.0f);

    chaos::shared_ptr<MyProvider> dynamic_provider = new MyProvider;
    chaos::shared_ptr<chaos::GPUProgramProvider> uniform_provider = new chaos::GPUProgramProvider;
		uniform_provider->AddVariableValue("translate_mat", glm::translate(tr));
		uniform_provider->AddVariableValue("scale_mat", glm::scale(scale));
		uniform_provider->AddVariableValue("toto", glm::vec2(5.0f, 6.0f));
    uniform_provider->AddVariableProvider(dynamic_provider.get());

#if 0
		glm::mat4 m1;
		glm::dmat4 m2;
		glm::mat3x2 m3;
		glm::dmat4x2 m4;
		glm::mat4 m5;
		glm::dmat4 m6;
		glm::mat3x2 m7;
		glm::dmat4x2 m8;
		glm::vec2    v1;
		glm::vec2    v2;
		glm::vec3    v3;
		glm::vec4    v4;
		glm::tvec2<GLint> v5;
		glm::tvec3<GLint> v6;
		glm::tvec4<GLint> v7;

		bool b1 = uniform_provider->GetValue("local_to_cam", m1);
		bool b2 = uniform_provider->GetValue("local_to_cam", m2);
		bool b3 = uniform_provider->GetValue("local_to_cam", m3);
		bool b4 = uniform_provider->GetValue("local_to_cam", m4);
		bool b5 = uniform_provider->GetValue("local_to_cam", v1);

		bool b6 = uniform_provider->GetValue("toto", m5);
		bool b7 = uniform_provider->GetValue("toto", m6);
		bool b8 = uniform_provider->GetValue("toto", m7);
		bool b9 = uniform_provider->GetValue("toto", m8);
		bool b10 = uniform_provider->GetValue("toto", v2);
		bool b11 = uniform_provider->GetValue("toto", v3);
		bool b12 = uniform_provider->GetValue("toto", v4);
		bool b13 = uniform_provider->GetValue("toto", v5);
		bool b14 = uniform_provider->GetValue("toto", v6);
		bool b15 = uniform_provider->GetValue("toto", v7);
#endif

		chaos::GPURenderParams render_params;
		particle_manager->Display(renderer, uniform_provider.get(), render_params);

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		atlas = nullptr;
		material = nullptr;
		particles_allocation = nullptr;
		chaos::Window::Finalize();
	}

	bool LoadAtlas(boost::filesystem::path const & resources_path)
	{
		atlas = new chaos::BitmapAtlas::TextureArrayAtlas;
		if (atlas == nullptr)
			return false;
		return atlas->LoadAtlas(resources_path / "Atlas" / "MyAtlas.json");
	}

	bool InitializeParticleManager(boost::filesystem::path const & resources_path)
	{
		// create the material
		material = chaos::GenDefaultParticleMaterial();
		if (material == nullptr)
			return false;
		// create the particle manager
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(atlas.get());
		// create the layer
		int render_order = 0;
		int layer_id = 0;
		chaos::ParticleLayerBase * layer = particle_manager->AddLayer<chaos::ParticleDefaultLayerTrait>(render_order, layer_id, material.get());
		if (layer == nullptr)
			return false;

		// parse the text
		chaos::ParticleTextGenerator::Generator generator(*atlas);
		generator.AddColor("red", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		generator.AddColor("blue", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		generator.AddColor("green", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		chaos::BitmapAtlas::FolderInfo const * folder = atlas->GetFolderInfo("folder_input1");
		if (folder != nullptr)
		{
			chaos::BitmapAtlas::BitmapInfo const * bitmap = folder->GetBitmapInfo("xboxControllerButtonA");
			if (bitmap != nullptr)
				generator.AddBitmap("BUTTON", bitmap);
		}
		
		generator.AddFontInfo("C1", atlas->GetFontInfo("font_info1"));
		generator.AddFontInfo("C2", atlas->GetFontInfo("font_info2"));

		chaos::ParticleTextGenerator::GeneratorParams generator_params;
		generator_params.line_height = 50;
		generator_params.font_info_name = "font_info1"; // the default character set
		generator_params.position = glm::vec2(0.0f, 0.0f);
		generator_params.hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
		generator_params.character_spacing = 0.0f;
		generator_params.line_spacing = 0.0f;
		generator_params.bitmap_padding.x = 1.0f * 30.0f;
		generator_params.bitmap_padding.y = 5.0f;
		generator_params.alignment = chaos::TextAlignment::LEFT;
		generator_params.justify_space_factor = 8.0f;
		generator_params.max_text_width = 200.0f;
		generator_params.word_wrap = true;

		//std::string message = "Hi [RED Ben]\nHere is a first example\nof my own stuff with [button][button][button] embedded";

		std::string message = "HELLO [GREEN Hi [RED] [C2] Ben [BLUE] truc] bidule";

		chaos::ParticleTextGenerator::GeneratorResult generator_result;
		generator.Generate(message.c_str(), generator_result, generator_params);

		// create a text allocation
		particles_allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, generator_result);

		return true;
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

		// initialize the atlas
		if (!LoadAtlas(resources_path))
			return false;

		// initialize the sprite manager
		if (!InitializeParticleManager(resources_path))
			return false;

		// place camera
		fps_view_controller.fps_controller.position.y = 0.0f;
		fps_view_controller.fps_controller.position.z = 10.0f;

		return true;
	}

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel = 0;
		hints.decorated = 1;
	}

	virtual bool Tick(float delta_time) override
	{
		particle_manager->Tick(delta_time);
		fps_view_controller.Tick(glfw_window, delta_time);

		return true; // refresh
	}

protected:

	// the particle manager
	chaos::shared_ptr<chaos::ParticleManager> particle_manager;
	/** the texture atlas */
	chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> atlas;
	/** the material */
	chaos::shared_ptr<chaos::GPURenderMaterial> material;
	/** allocation */
	chaos::shared_ptr<chaos::ParticleAllocationBase> particles_allocation;

	// the camera
	chaos::FPSViewInputController fps_view_controller;
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



