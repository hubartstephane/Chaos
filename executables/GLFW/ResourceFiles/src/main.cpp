#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}

	virtual bool DoTick(float delta_time) override
	{

		return false; // no redraw
	}


	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		using TexturePtr = chaos::shared_ptr<chaos::GPUTexture>;
		using ProgramPtr = chaos::shared_ptr<chaos::GPUProgram>;

		chaos::WindowApplication * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		chaos::GPUResourceManager * gpu_manager = application->GetGPUResourceManager();

		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		boost::filesystem::path const & user_local = application->GetUserLocalPath();

		// ==========================================================

		// independant loading (NOT GPUResourceManager) : always good
		TexturePtr independant_texture1 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "texture1.json");
		TexturePtr independant_texture2 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "texture2.json");
		TexturePtr independant_texture3 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "texture3.json");
		TexturePtr independant_texture4 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "texture4.json");
		TexturePtr independant_texture5 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "texture5.json");
		TexturePtr independant_texture6 = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(resource_path / "textures" / "space.png");

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
		ProgramPtr program1 = chaos::GPUProgramLoader(GetGPUDevice()).GenProgramObject(resource_path / "programs" / "program1.json");
		ProgramPtr program2 = chaos::GPUProgramLoader(GetGPUDevice()).GenProgramObject(resource_path / "programs" / "program2.json");
		ProgramPtr program3 = chaos::GPUProgramLoader(GetGPUDevice()).GenProgramObject(resource_path / "programs" / "program3.json");
		ProgramPtr program4 = chaos::GPUProgramLoader(GetGPUDevice()).GenProgramObject(resource_path / "programs" / "program4.json");


		program1->Release();
		program1->Release();


		boost::filesystem::path dir_path = chaos::JSONTools::DumpConfigFile(config.default_config);
		chaos::WinTools::ShowFile(dir_path);
		return true;
	}
};



// ===============================================


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}