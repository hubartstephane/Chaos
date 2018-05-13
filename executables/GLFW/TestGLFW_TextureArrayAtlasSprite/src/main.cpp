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
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/MathTools.h>
#include <chaos/GLMTools.h>
#include <chaos/Hotpoint.h>
#include <chaos/ParticleManager.h>
#include <chaos/RenderMaterial.h>
#include <chaos/ParticleTools.h>
#include <chaos/ParticleDefault.h>

class MyParticleTrait : public chaos::ParticleDefault::ParticleTrait
{
public:

	MyParticleTrait()
	{
		dynamic_particles = false;
	}
};

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  void GenerateParticles(float w, float h)
  {
		static int PARTICLE_COUNT = 100;

		if (particles_allocation != nullptr)
			return;

		chaos::ParticleLayer * layer = particle_manager->FindLayer(0);
		if (layer == nullptr)
			return;

		particles_allocation = layer->SpawnParticles(PARTICLE_COUNT * 2);
		if (particles_allocation == nullptr)
			return;
		
		chaos::ParticleDefault::Particle * particles = (chaos::ParticleDefault::Particle*)particles_allocation->GetParticleBuffer();
		if (particles == nullptr)
			return;

		glm::vec2 screen_size = glm::vec2(w, h);
		float     particle_size = max(w, h);

		// add characters
		auto const & character_sets = atlas->GetCharacterSets();
		if (character_sets.size() > 0)
		{
			chaos::BitmapAtlas::CharacterSet const * character_set = character_sets.at(0).get();

			size_t element_count = character_set->elements.size();
			if (element_count > 0)
			{
				for (int i = 0; i < PARTICLE_COUNT; ++i)
				{
					chaos::BitmapAtlas::CharacterEntry const * entry = &character_set->elements[rand() % element_count];

					glm::vec2 position = screen_size * chaos::GLMTools::RandVec2();
					glm::vec2 size = glm::vec2(particle_size * (0.01f + 0.05f * chaos::MathTools::RandFloat()));
					glm::vec3 color = chaos::GLMTools::RandVec3();

					particles->particle_box.position  = position;
					particles->particle_box.half_size = size * 0.5f;
					particles->color.r = color.r;
					particles->color.g = color.g;
					particles->color.b = color.b;
					particles->color.a = 1.0f;

					particles->texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, atlas->GetAtlasDimension());					
					++particles;
				}
			}
		}

		// add bitmap
		auto const & bitmap_sets = atlas->GetBitmapSets();
		if (bitmap_sets.size() > 0)
		{
			chaos::BitmapAtlas::BitmapSet const * bitmap_set = bitmap_sets.at(0).get();

			size_t element_count = bitmap_set->elements.size();
			if (element_count > 0)
			{
				for (int i = 0; i < PARTICLE_COUNT; ++i)
				{
					chaos::BitmapAtlas::BitmapEntry const * entry = &bitmap_set->elements[rand() % element_count];

					glm::vec2 position = screen_size * chaos::GLMTools::RandVec2();
					glm::vec2 size = glm::vec2(particle_size * (0.01f + 0.05f * chaos::MathTools::RandFloat()));
					glm::vec3 color = chaos::GLMTools::RandVec3();

					particles->particle_box.position = position;
					particles->particle_box.half_size = size * 0.5f;
					particles->color.r = color.r;
					particles->color.g = color.g;
					particles->color.b = color.b;
					particles->color.a = 1.0f;

					particles->texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, atlas->GetAtlasDimension());
					++particles;
				}
			}
		}
  }

  virtual bool OnDraw(glm::ivec2 size) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

                              // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    float w = (float)size.x;
    float h = (float)size.y;
		GenerateParticles(w, h);

    glm::vec3 scale = glm::vec3(2.0f / w, 2.0f / h, 1.0f);
    glm::vec3 tr    = glm::vec3(-1.0f, -1.0f, 0.0f);

    glm::mat4 local_to_cam = glm::translate(tr) * glm::scale(scale);

    chaos::DisableLastReferenceLost<chaos::GPUProgramProvider> uniform_provider;
    uniform_provider.AddVariableValue("local_to_cam", local_to_cam);

		particle_manager->Display(&uniform_provider);

    return true;
  }

  virtual void Finalize() override
  {
		material = nullptr;
		atlas = nullptr;
		program = nullptr;
		particle_manager = nullptr;
  }

  bool LoadAtlas(boost::filesystem::path const & resources_path)
  {
		atlas = new chaos::BitmapAtlas::TextureArrayAtlas();
		if (atlas == nullptr)
			return false;

    return atlas->LoadAtlas(resources_path / "MyAtlas.json");
  }

  bool InitializeParticleManager(boost::filesystem::path const & resources_path)
  {
		// create the program
		program = chaos::ParticleDefault::GenDefautParticleProgram();
		if (program == nullptr)
			return false;

		// create the material
		material = new chaos::RenderMaterial;
		if (material == nullptr)
			return false;		
		material->SetProgram(program.get());

		// create the particle manager
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(atlas.get());
		// create the layer
		chaos::ParticleLayer * layer = particle_manager->AddLayer(new chaos::TypedParticleLayerDesc<chaos::ParticleDefault::ParticleTrait>());
		if (layer == nullptr)
			return false;
		layer->SetLayerID(0);
		layer->SetRenderMaterial(material.get());

    return true;
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {
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

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

		particle_manager->Tick((float)delta_time);
    fps_view_controller.Tick(glfw_window, delta_time);		

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {

  }


protected:

	// the particle manager
	boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
	/** the texture atlas */
	boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> atlas;
	/** the material */
	boost::intrusive_ptr<chaos::RenderMaterial> material;
	/** the program */
	boost::intrusive_ptr<chaos::GPUProgram> program;
	/** allocation */
	boost::intrusive_ptr<chaos::ParticleAllocation> particles_allocation;
  // the camera
  chaos::FPSViewInputController fps_view_controller;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


