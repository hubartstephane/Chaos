#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/Object.h>
#include <chaos/XMLTools.h>
#include <chaos/TiledMap.h>
#include <chaos/BoostTools.h>

// tiled_map : it use tile set that are ordered
//             each tile as a number that correspond to 
//             - a tileset.
//             - a tile in the tileset
// in a tile_set
//    top-left = 1

bool LoadTileMap(chaos::TiledMap::Manager & manager)
{

	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return false;

	boost::filesystem::path const & resource_path = application->GetResourcesPath();


	chaos::TiledMap::TileSet * map_set = nullptr;
	chaos::TiledMap::Map * map = nullptr;

#if 1

	boost::filesystem::path set_path = resource_path / "Example1" / "tile set 1.tsx";
	boost::filesystem::path map_path = resource_path / "Example1" / "tiled_map.tmx";

	chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(set_path, false);
	if (buffer1 != nullptr)
		map_set = manager.LoadTileSet(set_path, buffer1);

#else

	boost::filesystem::path map_path = resource_path / "Example2" / "map.tmx";

#endif

	chaos::Buffer<char> buffer2 = chaos::FileTools::LoadFile(map_path, false);
	if (buffer2 != nullptr)
		map = manager.LoadMap(map_path, buffer2);

	return (map != nullptr);
}

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		chaos::TiledMap::Manager manager;
		LoadTileMap(manager);

		chaos::WinTools::PressToContinue();

		return true;
	}
};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


