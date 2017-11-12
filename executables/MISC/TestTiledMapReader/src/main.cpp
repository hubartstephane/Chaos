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
public:

  /** returns true whether the name match the resource name */
  bool IsMatchingName(char const * filename) const;

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

  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * filename);
  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * name, chaos::Buffer<char> buffer);
  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * name, tinyxml2::XMLDocument * doc);

  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * filename);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * filename, chaos::Buffer<char> buffer);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * filename, tinyxml2::XMLDocument * doc);


  /** find tiled map */
	TiledMap * FindTiledMap(char const * filename);
  TiledMap const * FindTiledMap(char const * filename) const;
  /** find tiled map set */
  TiledMapSet * FindTiledMapSet(char const * filename);
  TiledMapSet const * FindTiledMapSet(char const * filename) const;


protected:

  /** internal method to load a tiled map set (with no search for exisiting items) */
  TiledMap * DoLoadTiledMap(char const * filename);
  /** internal method to load a tiled map set (with no search for exisiting items) */
  TiledMap * DoLoadTiledMap(char const * name, chaos::Buffer<char> buffer);
  /** internal method to load a tiled map set (with no search for exisiting items) */
  TiledMap * DoLoadTiledMap(char const * name, tinyxml2::XMLDocument * doc);

  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * filename);
  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * filename, chaos::Buffer<char> buffer);
  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * filename, tinyxml2::XMLDocument * doc);

protected:

	/** the maps */
	std::vector<boost::intrusive_ptr<TiledMap>> tiled_maps;
	/** the assets */
	std::vector<boost::intrusive_ptr<TiledMapSet>> tiled_sets;
};





bool TiledMapObjectBase::IsMatchingName(char const * filename) const
{

  return false;
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * filename)
{
  TiledMapSet * result = FindTiledMapSet(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(filename);
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * filename, chaos::Buffer<char> buffer)
{
  TiledMapSet * result = FindTiledMapSet(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(filename, buffer);
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * filename, tinyxml2::XMLDocument * doc)
{
  TiledMapSet * result = FindTiledMapSet(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(filename, doc);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * filename)
{
  TiledMap * result = FindTiledMap(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(filename);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * filename, chaos::Buffer<char> buffer)
{
  TiledMap * result = FindTiledMap(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(filename, buffer);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * filename, tinyxml2::XMLDocument * doc)
{
  TiledMap * result = FindTiledMap(filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(filename, doc);
}

TiledMap * TiledMapManager::FindTiledMap(char const * filename)
{
  size_t count = tiled_maps.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_maps[i]->IsMatchingName(filename))
      return tiled_maps[i].get();
  return nullptr;
}

TiledMap const * TiledMapManager::FindTiledMap(char const * filename) const
{
  size_t count = tiled_maps.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_maps[i]->IsMatchingName(filename))
      return tiled_maps[i].get();
  return nullptr;
}

TiledMapSet * TiledMapManager::FindTiledMapSet(char const * filename)
{
  size_t count = tiled_sets.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_sets[i]->IsMatchingName(filename))
      return tiled_sets[i].get();
  return nullptr;
}

TiledMapSet const * TiledMapManager::FindTiledMapSet(char const * filename) const
{
  size_t count = tiled_sets.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_sets[i]->IsMatchingName(filename))
      return tiled_sets[i].get();
  return nullptr;
}












TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * filename)
{
  assert(filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(filename, true);
  if (buffer != nullptr)
    return DoLoadTiledMapSet(filename, buffer);
  return nullptr;
}

TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * filename, chaos::Buffer<char> buffer)
{
  TiledMapSet * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = DoLoadTiledMapSet(filename, doc);
    delete(doc);
  }
  return result;

}

TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * filename, tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);











  return nullptr;
}












TiledMap * TiledMapManager::DoLoadTiledMap(char const * filename)
{
  assert(filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(filename, true);
  if (buffer != nullptr)
    return DoLoadTiledMap(filename, buffer);
  return nullptr;
}

TiledMap * TiledMapManager::DoLoadTiledMap(char const * filename, chaos::Buffer<char> buffer)
{
  TiledMap * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = DoLoadTiledMap(filename, doc);
    delete(doc);
  }
  return result;

}

TiledMap * TiledMapManager::DoLoadTiledMap(char const * filename, tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);










  return nullptr;
}
















/*



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


