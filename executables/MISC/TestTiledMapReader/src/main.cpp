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
#include <chaos/XMLTools.h>
#include <chaos/TiledMapReader.h>

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

  chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(set_path.string().c_str(), false);
  if (buffer1 != nullptr)
  {
    chaos::TiledMap::TileSet * map_set = manager.LoadTileSet(set_path.string().c_str(), buffer1);

  }

  boost::filesystem::path map_path = resource_path / "tiled_map.tmx";

	chaos::Buffer<char> buffer2 = chaos::FileTools::LoadFile(map_path.string().c_str(), false);
	if (buffer2 != nullptr)
	{
    chaos::TiledMap::Map * map = manager.LoadMap(map_path.string().c_str(), buffer2);

	}
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::TiledMap::Manager manager;

  size_t t = sizeof(std::string);

  boost::filesystem::path p;

  chaos::TiledMap::PropertyInt pi;
  chaos::TiledMap::PropertyInt const pic;

  chaos::TiledMap::PropertyString ps;
  chaos::TiledMap::PropertyString const psc;

  auto p1 = pi.GetBoolProperty();
  auto p2 = pi.GetIntProperty();
  auto p3 = pi.GetFloatProperty();
  auto p4 = pi.GetStringProperty();

  auto p5 = pic.GetBoolProperty();
  auto p6 = pic.GetIntProperty();
  auto p7 = pic.GetFloatProperty();
  auto p8 = pic.GetStringProperty();

  auto s1 = ps.GetBoolProperty();
  auto s2 = ps.GetIntProperty();
  auto s3 = ps.GetFloatProperty();
  auto s4 = ps.GetStringProperty();

  auto s5 = psc.GetBoolProperty();
  auto s6 = psc.GetIntProperty();
  auto s7 = psc.GetFloatProperty();
  auto s8 = psc.GetStringProperty();

	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();



	LoadTileMap(manager);

	chaos::WinTools::PressToContinue();

	return 0;
}


