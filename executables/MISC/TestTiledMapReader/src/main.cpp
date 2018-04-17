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

// tiled_map : it use tile set that are ordered
//             each tile as a number that correspond to 
//             - a tileset.
//             - a tile in the tileset
// in a tile_set
//    top-left = 1

void LoadTileMap(chaos::TiledMap::Manager & manager)
{



	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return;

	boost::filesystem::path const & resource_path = application->GetResourcesPath();

	boost::filesystem::path set_path = resource_path / "tile set 1.tsx";
	boost::filesystem::path map_path = resource_path / "tiled_map.tmx";

	chaos::TiledMap::TileSet * map_set = nullptr;
	chaos::TiledMap::Map * map = nullptr;

	chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(set_path.string().c_str(), false);
	if (buffer1 != nullptr)
		map_set = manager.LoadTileSet(set_path.string().c_str(), buffer1);

	
	chaos::Buffer<char> buffer2 = chaos::FileTools::LoadFile(map_path.string().c_str(), false);
	if (buffer2 != nullptr)
		map = manager.LoadMap(map_path.string().c_str(), buffer2);
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


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


