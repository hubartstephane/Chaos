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
		bool GenerateAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input, char const * set_name = "sprites");
		/** fill BitmapAtlasInput from a TileSet */
		bool GenerateAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input, char const * set_name = "sprites");
		/** fill BitmapAtlasInput from a Map */
		bool GenerateAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input, char const * set_name = "sprites");


	}; // namespace TiledMapTools

}; // namespace chaos
