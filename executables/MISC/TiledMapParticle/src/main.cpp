#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>
#include <chaos/XMLTools.h>
#include <chaos/TiledMap.h>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/TiledMapTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GLTextureTools.h> 
#include <chaos/GPUTextureLoader.h> 
#include <chaos/GPUFramebuffer.h> 
#include <chaos/GPUFramebufferGenerator.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 

#define DUMP_DEBUG_DATA 1 // _DEBUG

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
protected:

	virtual bool Tick(float delta_time) override
	{
#if 0
		// XINPUT1_4.dll

		int gamepad = 0;

		XINPUT_STATE xs;
		if (XInputGetState(gamepad, &xs) == 0)
		{
			int left = 1;
			int right = 1;

			XINPUT_VIBRATION vib;
			ZeroMemory(&vib, sizeof(vib));
			vib.wLeftMotorSpeed = (WORD)(left * 65535);
			vib.wRightMotorSpeed = (WORD)(right * 65535);
			XInputSetState(gamepad, &vib);
		}
#endif
		return false;
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		tiledmap_manager = nullptr;
		texture_atlas = nullptr;
		tiled_map = nullptr;
		chaos::MyGLFW::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

		chaos::ImageDescription desc;
		desc.pixel_format = chaos::PixelFormat::GetPixelFormat<chaos::PixelDepthStencil>();
		desc.width = 512;
		desc.height = 512;

		chaos::GPUTextureLoader texture_loader;

		chaos::GPUTexture * texture = texture_loader.GenTextureObject(desc);

		chaos::PixelFormat format = chaos::GLTextureTools::GetTexturePixelFormat(texture->GetResourceID(), 0);


		glm::ivec2 null_size (0, 0);
		glm::ivec2 final_size(1024, 1024);

		chaos::GPUFramebufferGenerator framebuffer_generator;
		framebuffer_generator.AddColorAttachment(0, chaos::PixelFormat::GetPixelFormat<chaos::PixelBGRA>(), null_size, "truc");
		framebuffer_generator.AddColorAttachment(1, chaos::PixelFormat::GetPixelFormat<chaos::PixelBGRA>(), null_size, "bidule");
		framebuffer_generator.AddColorAttachment(2, chaos::PixelFormat::GetPixelFormat<chaos::PixelDepthStencil>(), null_size, "machin");


		



		chaos::GPUFramebuffer * framebuffer = framebuffer_generator.GenerateFramebuffer(final_size);

		bool completed = framebuffer->CheckCompletionStatus();

		framebuffer = framebuffer;




		if (!InitializeTiledMapManager())
			return false;

		if (!InitializeParticleManager())
			return false;

		if (!InitializeParticleMap())
			return false;


		return true;
	}

	bool InitializeParticleManager()
	{
		// create the manager
		particle_manager = new chaos::ParticleManager();
		if (particle_manager == nullptr)
			return false;
		// set the atlas
		particle_manager->SetTextureAtlas(texture_atlas.get());

		return true;
	}

	chaos::BitmapAtlas::TextureArrayAtlas * GenerateTextureAtlas(chaos::TiledMap::Manager * const manager)
	{
		// fill the input
		chaos::BitmapAtlas::AtlasInput input;

		chaos::BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder("sprites", 0);
		if (folder_input == nullptr)
			return nullptr;

		if (!chaos::TiledMapTools::AddIntoAtlasInput(manager, folder_input))
			return nullptr;

		// generate the atlas
		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

		char const* dump_atlas_dirname = nullptr;
#if DUMP_DEBUG_DATA
		dump_atlas_dirname = "Atlas";
#endif

		chaos::BitmapAtlas::TextureArrayAtlasGenerator generator;
		return generator.ComputeResult(input, params, dump_atlas_dirname);
	}

	bool InitializeTiledMapManager()
	{
		// get the application
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// create and open a temp directory
		boost::filesystem::path user_temp = application->CreateUserLocalTempDirectory(); // XXX : this directory is necessary for Best score
#if DUMP_DEBUG_DATA
		chaos::WinTools::ShowFile(user_temp);
#endif

		// create the manager
		tiledmap_manager = new chaos::TiledMap::Manager;
		if (tiledmap_manager == nullptr)
			return false;

		// load a tiled map
		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		tiled_map = tiledmap_manager->LoadMap(resource_path / "Map" / "map.tmx");
		if (tiled_map == nullptr)
			return false;

		texture_atlas = GenerateTextureAtlas(tiledmap_manager.get());
		if (texture_atlas == nullptr)
			return false;

		return true;
	}

	bool InitializeParticleMap()
	{
		chaos::GPUResourceManager * gpu_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
		if (gpu_manager == nullptr)
			return nullptr;

		for (size_t i = 0; i < tiled_map->tile_layers.size(); ++i)
		{
			chaos::TiledMap::TileLayer const * tile_layer = tiled_map->tile_layers[i].get();
			if (tile_layer == nullptr)
				continue;

			// find the render material name
			char const * render_material_name = "tile";

			std::string const * render_material_property = tile_layer->FindPropertyString("GPURenderMaterial");
			if (render_material_property != nullptr)
				render_material_name = render_material_property->c_str();

			// find the particle class
			char const * particle_class_name = nullptr;

			std::string const * particle_class_property = tile_layer->FindPropertyString("ParticleClass");
			if (particle_class_property != nullptr)
				particle_class_name = particle_class_property->c_str();

			// find whether the layer is visible
			bool visible = tile_layer->visible;

			// find rendering order
			int rendering_order = (int)i;

			int const * rendering_order_property = tile_layer->FindPropertyInt("RenderingOrder");
			if (rendering_order_property != nullptr)
				rendering_order = *rendering_order_property;

			// find corresponding material
			chaos::GPURenderMaterial * material = gpu_manager->FindRenderMaterial(render_material_name);







			chaos::ParticleLayerBase * particle_layer = nullptr;

			glm::ivec2 size = tile_layer->size;
			for (size_t j = 0; j < tile_layer->tile_indices.size(); ++j)
			{
				int tile_indice = tile_layer->tile_indices[j];
				if (tile_indice == 0)
					continue;

				int x = j % size.x;
				int y = j / size.x;

				chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(tile_indice);
				if (tile_info.tiledata != nullptr)
				{
					if (particle_layer == nullptr)
					{
						int layer_id = 0;
						particle_layer = particle_manager->AddLayer<chaos::ParticleDefault::ParticleTrait>(rendering_order, layer_id, material);
						if (particle_layer == nullptr)
							return false;
						chaos::ParticleAllocationBase * allocation = particle_layer->SpawnParticles(0);
						if (allocation == nullptr)
							return false;
						allocation->Show(visible);
					}




				}


				




				tile_indice = tile_indice;
			}
			 
		
		}






		return true;
	}

protected:

	chaos::shared_ptr<chaos::ParticleManager> particle_manager;

	chaos::shared_ptr<chaos::TiledMap::Manager> tiledmap_manager;

	chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;

	chaos::TiledMap::Map * tiled_map = nullptr;
};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}

