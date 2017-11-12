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
#include <chaos/HTMLTools.h>


class TiledMapObjectBase : public chaos::ReferencedObject
{

};

class TiledMap : public TiledMapObjectBase
{


};

class TiledMapSet : public TiledMapObjectBase
{


};

class TiledMapManager
{
public:

  /** load a tiled map */
  TiledMap * LoadTiledMap(char const * filename);
  /** load a tiled map */
  TiledMap * LoadTiledMap(chaos::Buffer<char> buffer);
  /** load a tiled map */
  TiledMap * LoadTiledMap(tinyxml2::XMLDocument * doc);

  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * filename);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(chaos::Buffer<char> buffer);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(tinyxml2::XMLDocument * doc);



  /*
	TiledMap * FindTiledMap(char const * name);

  TiledMapSet * FindTiledMapSet(char const * name);
*/



protected:

	/** the maps */
	std::vector<boost::intrusive_ptr<TiledMap>> tiled_maps;
	/** the assets */
	std::vector<boost::intrusive_ptr<TiledMapSet>> tiled_sets;
};




TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * filename)
{
  assert(filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(filename, true);
  if (buffer != nullptr)
    return LoadTiledMapSet(buffer);
  return nullptr;
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(chaos::Buffer<char> buffer)
{
  TiledMapSet * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = LoadTiledMapSet(doc);      
    delete(doc);
  }
  return result;

}

TiledMapSet * TiledMapManager::LoadTiledMapSet(tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);











  return nullptr;
}


TiledMap * TiledMapManager::LoadTiledMap(char const * filename)
{
  assert(filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(filename, true);
  if (buffer != nullptr)
    return LoadTiledMap(buffer);
  return nullptr;
}

TiledMap * TiledMapManager::LoadTiledMap(chaos::Buffer<char> buffer)
{
  TiledMap * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = LoadTiledMap(doc);
    delete(doc);
  }
  return result;

}

TiledMap * TiledMapManager::LoadTiledMap(tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);










  return nullptr;
}
















/*

TiledMap * TiledMapManager::FindTiledMap(char const * name)
{

	return nullptr;
}

TiledMap * TiledMapManager::FindTiledMapAsset(char const * name)
{

	return nullptr;
}

*/









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


