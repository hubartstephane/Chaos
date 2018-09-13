#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TiledMap.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>

namespace chaos
{
	namespace TiledMapTools
	{
		/** fill BitmapAtlasInput from a TiledMap manager */
		bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a TileSet */
		bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a Map */
		bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input);

		/** fill BitmapAtlasInput from a TiledMap manager */
		bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a TileSet */
		bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Map */
		bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::FolderInfoInput * folder_input);

	}; // namespace TiledMapTools

}; // namespace chaos
