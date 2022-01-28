#include <chaos/Chaos.h>

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  void GenerateParticles(float w, float h)
  {
		// get the existing bitmaps
		std::vector<chaos::BitmapAtlas::BitmapLayout> bitmaps;
		atlas->CollectEntries(bitmaps, true);

		size_t bitmap_count = bitmaps.size();
		if (bitmap_count == 0)
			return;

		// early exit
		if (particles_allocation != nullptr)
			return;

		int layer_id = 0;
		chaos::ParticleLayerBase * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return;

		// spawn particles
		particles_allocation = layer->SpawnParticles(bitmap_count);
		if (particles_allocation == nullptr)
			return;

		glm::vec2 screen_size = glm::vec2(w, h);
		float     particle_size = std::max(w, h);

		// initialize the particles

		chaos::ParticleAccessor<chaos::ParticleDefault> particle_accessor = particles_allocation->GetParticleAccessor();
		for (size_t i = 0; i < bitmap_count; ++i)
		{
			chaos::ParticleDefault & p = particle_accessor[i];

			glm::vec2 position = screen_size * chaos::GLMTools::RandVec2();
			glm::vec2 size = glm::vec2(particle_size * (0.01f + 0.05f * chaos::MathTools::RandFloat()));
			glm::vec3 color =
				((i & 1) == 0)? glm::vec3(1.0f, 1.0f, 1.0f) : chaos::GLMTools::RandVec3();

			p.bounding_box.position = position;
			p.bounding_box.half_size = size * 0.5f;
			p.color.r = color.r;
			p.color.g = color.g;
			p.color.b = color.b;
			p.color.a = 1.0f;

			p.texcoords = bitmaps[i].GetTexcoords();
		}
  }

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
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
    glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    float w = (float)draw_params.full_size.x;
    float h = (float)draw_params.full_size.y;
		GenerateParticles(w, h);

    glm::vec3 scale = glm::vec3(2.0f / w, 2.0f / h, 1.0f);
    glm::vec3 tr    = glm::vec3(-1.0f, -1.0f, 0.0f);

    glm::mat4 local_to_cam = glm::translate(tr) * glm::scale(scale);

    chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
	main_uniform_provider.AddVariable("local_to_cam", local_to_cam);

		chaos::GPURenderParams render_params;
		particle_manager->Display(renderer, &main_uniform_provider, render_params);

    return true;
  }

  virtual void Finalize() override
  {
		material = nullptr;
		atlas = nullptr;
		particle_manager = nullptr;
		chaos::Window::Finalize();
  }

  bool LoadAtlas(boost::filesystem::path const & resources_path)
  {
		atlas = new chaos::BitmapAtlas::TextureArrayAtlas();
		if (atlas == nullptr)
			return false;

    return atlas->LoadAtlas(resources_path / "Atlas" / "MyAtlas.json");
  }

  bool InitializeParticleManager(boost::filesystem::path const & resources_path)
  {
		// create the material
		material = chaos::DefaultParticleProgram::GetMaterial();
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

    return true;
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
  {
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();

    // initialize the atlas
    if (!LoadAtlas(resources_path))
      return false;

    // initialize the particle manager
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


