#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>


class TiledMapObjectBase : public chaos::ReferencedObject
{

};

class TiledMap : public TiledMapObjectBase
{


};

class TiledMapAsset : public TiledMapObjectBase
{


};

class TiledMapManager
{
public:

	TiledMap * FindTiledMap(char const * name);

	TiledMap * FindTiledMapAsset(char const * name);




protected:

	/** the maps */
	std::vector<boost::intrusive_ptr<TiledMap>> tiled_maps;
	/** the assets */
	std::vector<boost::intrusive_ptr<TiledMapAsset>> tiled_assets;
};


TiledMap * TiledMapManager::FindTiledMap(char const * name)
{

	return nullptr;
}

TiledMap * TiledMapManager::FindTiledMapAsset(char const * name)
{

	return nullptr;
}











// tiled_map : it use tile set that are ordered
//             each tile as a number that correspond to 
//             - a tileset.
//             - a tile in the tileset
// in a tile_set
//    top-left = 1

void LoadTileMap(TiledMapManager & manager)
{
	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return;

	boost::filesystem::path const & resource_path = application->GetResourcesPath();

	chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resource_path / "tiled_map.tmx", false);
	if (buffer != nullptr)
	{

	}
}

int _tmain(int argc, char ** argv, char ** env)
{
	TiledMapManager manager;


	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();



	LoadTileMap(manager);

	chaos::WinTools::PressToContinue();

	return 0;
}


