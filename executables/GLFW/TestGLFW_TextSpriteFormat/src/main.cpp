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
#include <chaos/ParticleManager.h>
#include <chaos/RenderMaterial.h>
#include <chaos/ParticleDefault.h>
#include <chaos/ParticleTextGenerator.h>


// --------------------------------------------------------------------

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

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

		class MyProvider : public chaos::GPUProgramProviderBase
		{
		protected: 

			virtual bool DoProcessAction(char const * name, chaos::GPUProgramAction & action, chaos::GPUProgramProviderBase const * top_level) const override
			{
				if (strcmp("local_to_cam", name) == 0)
				{
					glm::mat4 translate_mat;
					glm::mat4 scale_mat;

					if (top_level->GetValue<glm::mat4>("translate_mat", translate_mat))
						if (top_level->GetValue<glm::mat4>("scale_mat", scale_mat))
							return action.Process("local_to_cam", translate_mat * scale_mat);
				}         
				return false;
			}
		};

		float w = (float)size.x;
		float h = (float)size.y;
		glm::vec3 scale = glm::vec3(2.0f / w, 2.0f / h, 1.0f);
		glm::vec3 tr = glm::vec3(-1.0f, -1.0f, 0.0f);

    boost::intrusive_ptr<MyProvider> dynamic_provider = new MyProvider;
    boost::intrusive_ptr<chaos::GPUProgramProvider> uniform_provider = new chaos::GPUProgramProvider;
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

		particle_manager->Display(uniform_provider.get());

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		atlas = nullptr;
		material = nullptr;
		program = nullptr;
		particles_allocation = nullptr;
	}

	bool LoadAtlas(boost::filesystem::path const & resources_path)
	{
		atlas = new chaos::BitmapAtlas::TextureArrayAtlas;
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

		// parse the text
		chaos::ParticleTextGenerator::Generator generator(*atlas);
		generator.AddColor("red", glm::vec3(1.0f, 0.0f, 0.0f));
		generator.AddBitmap("BUTTON", atlas->GetBitmapSet("bitmap_set1")->GetEntry("xboxControllerButtonA.tga"));
		generator.AddCharacterSet("C1", atlas->GetCharacterSet("character_set1"));
		generator.AddCharacterSet("C2", atlas->GetCharacterSet("character_set2"));

		chaos::ParticleTextGenerator::GeneratorParams generator_params;
		generator_params.line_height = 50;
		generator_params.character_set_name = "character_set1"; // the default character set
		generator_params.position = glm::vec2(0.0f, 0.0f);
		generator_params.hotpoint_type = chaos::Hotpoint::BOTTOM_LEFT;
		generator_params.character_spacing = 0.0f;
		generator_params.line_spacing = 0.0f;
		generator_params.bitmap_padding.x = 10.0f;
		generator_params.bitmap_padding.y = 5.0f;
		generator_params.alignment = chaos::ParticleTextGenerator::GeneratorParams::ALIGN_LEFT;
		generator_params.justify_space_factor = 8.0f;
		generator_params.max_text_width = 200.0f;
		generator_params.word_wrap = true;

		chaos::ParticleTextGenerator::GeneratorResult generator_result;
		generator.Generate(
			"Hi [RED Ben]\nHere is a first example\nof my own stuff with [button][button][button] embedded",
			generator_result,
			generator_params);

		// create a text allocation
		particles_allocation = layer->SpawnParticles(generator_result.GetTokenCount());
		if (particles_allocation == nullptr)
			return false;

		chaos::ParticleDefault::Particle * particles = (chaos::ParticleDefault::Particle*)particles_allocation->GetParticleBuffer();
		if (particles == nullptr)
			return false;

		// convert the text
		size_t k = 0;
		for (size_t i = 0; i < generator_result.token_lines.size(); ++i)
		{
			chaos::ParticleTextGenerator::TokenLine const & line = generator_result.token_lines[i];
			for (size_t j = 0; j < line.size(); ++j)
			{
				chaos::ParticleTextGenerator::Token const & token = line[j];

				particles[k].bounding_box = chaos::box2(std::make_pair(token.corners.bottomleft, token.corners.topright));
				particles[k].texcoords = token.texcoords;
				particles[k].color =  glm::vec4(token.color.r, token.color.g, token.color.b, 1.0f);
				++k;
			}
		}

		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
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



