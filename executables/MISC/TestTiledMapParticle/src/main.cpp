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
		manager = nullptr;
		texture_atlas = nullptr;


	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!InitializeTiledMapManager())
			return false;


		return true;
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
		manager = new chaos::TiledMap::Manager;
		if (manager == nullptr)
			return false;

		// load a tiled map
		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		map = manager->LoadMap(resource_path / "Map" / "map.tmx");
		if (map == nullptr)
			return false;

		// generate the atlas
		chaos::BitmapAtlas::AtlasInput input;

		chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
		if (bitmap_set == nullptr)
			return false;

		// insert all images in any referenced TiledSet
		for (size_t i = 0; i < map->image_layers.size(); ++i)
		{
			chaos::TiledMap::TileSet tile_set


		}

		// generate the atlas
		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

		chaos::BitmapAtlas::Atlas          atlas;
		chaos::BitmapAtlas::AtlasGenerator generator;
		if (!generator.ComputeResult(input, atlas, params))
			return false;

		// generate texture Atlas
		texture_atlas = new chaos::BitmapAtlas::TextureArrayAtlas;
		if (texture_atlas == nullptr)
			return false;
		if (!texture_atlas->LoadFromBitmapAtlas(atlas))
			return false;

		// dump the atlas
#if _DEBUG
		atlas.SaveAtlas(application->GetUserLocalTempPath() / "LudumAtlas");
#endif

#if 0

		// get the directory where the sprites are
		std::string sprite_directory;
		chaos::JSONTools::GetAttribute(config, "sprite_directory", sprite_directory);

		// get the path of the font
		std::string font_path;
		chaos::JSONTools::GetAttribute(config, "font_path", font_path);

		std::string title_font_path;
		chaos::JSONTools::GetAttribute(config, "title_font_path", title_font_path);

		// Add sprites
		chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
		if (bitmap_set == nullptr)
			return false;

		bitmap_set->AddBitmapFilesFromDirectory(sprite_directory);


		chaos::BitmapAtlas

		// enumerate all layers


		map->object_layers.
#endif

#if 0
		


		chaos::TiledMap::Map * map = nullptr;

		boost::filesystem::path map_path = resource_path / "Map" / "map.tmx";



		chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(map_path, false);
		if (buffer1 != nullptr)
			map = manager.LoadMap(map_path, buffer1);
#endif
		return true;
	}

protected:

	boost::intrusive_ptr<chaos::TiledMap::Manager> manager;

	boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;

	chaos::TiledMap::Map * map = nullptr;
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


