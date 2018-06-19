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
#include <chaos/MyGLFWSingleWindowApplication.h> 

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, size.x, size.y);
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


	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!InitializeTiledMapManager())
			return false;


		return true;
	}

	chaos::BitmapAtlas::TextureArrayAtlas * GenerateTextureAtlas(chaos::TiledMap::Manager * const manager)
	{
		// fill the input
		chaos::BitmapAtlas::AtlasInput input;
		if (!chaos::TiledMapTools::GenerateAtlasInput(manager, input, "sprites"))
			return nullptr;

		// generate the atlas
		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

#if _DEBUG
		params.debug_dump_atlas_dirname = "Atlas";
#endif

		chaos::BitmapAtlas::TextureArrayAtlasGenerator generator;
		return generator.ComputeResult(input, params);
	}

	bool InitializeTiledMapManager()
	{
		// get the application
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// create and open a temp directory
		boost::filesystem::path user_temp = application->CreateUserLocalTempDirectory(); // XXX : this directory is necessary for Best score
#if _DEBUG
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



		for (size_t i = 0; i < tiled_map->tile_layers.size(); ++i)
		{
			chaos::TiledMap::TileLayer const * tile_layer = tiled_map->tile_layers[i].get();
			if (tile_layer == nullptr)
				continue;

			glm::ivec2 size = tile_layer->size;

			std::vector<int> tmp;

			for (size_t j = 0; j < tile_layer->tile_indices.size(); ++j)
			{
				int tile_indice = tile_layer->tile_indices[j];
				if (tile_indice == 0)
					continue;

				if (std::find(tmp.begin(), tmp.end(), tile_indice) != tmp.end())
					continue;
				tmp.push_back(tile_indice);


				int x = j % size.x;
				int y = j / size.x;

				chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(tile_indice);


				




				tile_indice = tile_indice;
			}
			 
		
		}






		return true;
	}

protected:

	boost::intrusive_ptr<chaos::ParticleManager> particle_manager;

	boost::intrusive_ptr<chaos::TiledMap::Manager> tiledmap_manager;

	boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;

	chaos::TiledMap::Map * tiled_map = nullptr;
};


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


