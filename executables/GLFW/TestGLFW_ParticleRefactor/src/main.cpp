#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>
#include <chaos/SoundManager.h>
#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureLoader.h>
#include <chaos/GPUProgramLoader.h>


#include "ParticleManagerRefactor.h"

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}

	virtual void Finalize() override
	{

	}

	virtual bool Tick(double delta_time) override
	{

		return false; // no redraw
	}

	virtual void OnMouseButton(int button, int action, int modifier) override
	{




	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		using TexturePtr = boost::intrusive_ptr<chaos::Texture>;
		using ProgramPtr = boost::intrusive_ptr<chaos::GPUProgram>;

		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return false;

		chaos::GPUResourceManager * gpu_manager = application->GetGPUResourceManager();


		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		boost::filesystem::path const & user_local = application->GetUserLocalPath();

		// ==========================================================

		// independant loading (NOT GPUResourceManager) : always good
		TexturePtr independant_texture1 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "texture1.json");
		TexturePtr independant_texture2 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "texture2.json");
		TexturePtr independant_texture3 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "texture3.json");
		TexturePtr independant_texture4 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "texture4.json");
		TexturePtr independant_texture5 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "texture5.json");
		TexturePtr independant_texture6 = chaos::TextureLoader().GenTextureObject(resource_path / "textures" / "space.png");

		// ==========================================================

		// XXX : texture1.json is a reference on texture1.bmp
		//       GPU.json should load both with success

		// "texture1.bmp" already loaded directly in the GPU.json
		TexturePtr failed_load_texture = gpu_manager->LoadTexture(resource_path / "textures" / "texture1.bmp");
		TexturePtr already_exisiting   = gpu_manager->FindTextureByPath(resource_path / "textures" / "texture1.bmp");
		
		// "texture1.json" already loaded directly in the GPU.json		
		TexturePtr failed_texture1 = gpu_manager->LoadTexture(resource_path / "textures" / "texture1.json");
		
		// in GPU.json there is already a texture named "toto"
		TexturePtr failed_texture3 = gpu_manager->LoadTexture(resource_path / "textures" / "texture3.json", "toto");

		// none of the following textures has been loaded into GPU.json 
		TexturePtr success_texture2 = gpu_manager->LoadTexture(resource_path / "textures" / "texture2.json", "titi");
		TexturePtr success_texture4 = gpu_manager->LoadTexture(resource_path / "textures" / "texture4.json");
		TexturePtr success_texture5 = gpu_manager->LoadTexture(resource_path / "textures" / "texture5.json");
		TexturePtr success_texture6 = gpu_manager->LoadTexture(resource_path / "textures" / "space.png");

		TexturePtr found_texture1 = gpu_manager->FindTexture("toto");
		TexturePtr found_texture2 = gpu_manager->FindTexture("titi");



		// ==========================================================
		ProgramPtr program1 = chaos::GPUProgramLoader().GenProgramObject(resource_path / "programs" / "program1.json");
		ProgramPtr program2 = chaos::GPUProgramLoader().GenProgramObject(resource_path / "programs" / "program2.json");
		ProgramPtr program3 = chaos::GPUProgramLoader().GenProgramObject(resource_path / "programs" / "program3.json");
		ProgramPtr program4 = chaos::GPUProgramLoader().GenProgramObject(resource_path / "programs" / "program4.json");

		program1->AddReleaseCallback(new chaos::GPUResourceAutoCallbacks(
			[](chaos::GPUResource const * resource, bool destruction) {


			return true;

		}));

		program1->Release();
		program1->Release();


		boost::filesystem::path dir_path = chaos::JSONTools::DumpConfigFile(config);
		chaos::WinTools::ShowFile(dir_path);
		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}
};



// ===============================================


int _tmain(int argc, char ** argv, char ** env)
{
	ParticleLayer sl(new ParticleLayerDescExample());

	ParticleRange r = sl.SpawnParticles(17);

	boost::intrusive_ptr<ParticleRangeAllocation> p1 = sl.SpawnParticlesAndKeepRange(5);
	boost::intrusive_ptr<ParticleRangeAllocation> p2 = sl.SpawnParticlesAndKeepRange(10);
	boost::intrusive_ptr<ParticleRangeAllocation> p3 = sl.SpawnParticlesAndKeepRange(20);

	auto c1 = p1->GetParticleCount();
	auto c2 = p2->GetParticleCount();
	auto c3 = p3->GetParticleCount();

	p2 = nullptr;
	sl.TickParticles(0.0f);


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


