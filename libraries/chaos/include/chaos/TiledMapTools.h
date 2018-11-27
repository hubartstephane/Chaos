#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TiledMap.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>

namespace chaos
{
	class TiledMapTools
	{
	public:

		/** fill BitmapAtlasInput from a TiledMap manager */
		static bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a TileSet */
		static bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a Map */
		static bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input);

		/** fill BitmapAtlasInput from a TiledMap manager */
		static bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a TileSet */
		static bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Map */
		static bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::FolderInfoInput * folder_input);

		/** search whether the object has a boolean/integer property set to true */
		static bool HasFlag(TiledMap::PropertyOwner const * property_owner, char const * property_name);
		/** search whether the object has a given name, a given type or a boolean/integer property set to true */
		static bool HasFlag(TiledMap::GeometricObject const * object_geometric, char const * name, char const * type, char const * property_name);
		/** returns true whether the object explicitly define a world origin */
		static bool IsWorldOrigin(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object defines an explicit world bounds */
		static bool IsWorldBounds(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a player start */
		static bool IsPlayerStart(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a trigger surface */
		static bool IsTriggerSurface(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a camera */
		static bool IsCamera(TiledMap::GeometricObject const * object_geometric);

		/** try to read world origin from object if possible (expressed in object layer coordinates) */
		static bool GetExplicitWorldOrigin(TiledMap::GeometricObject const * object_geometric, glm::vec2 & result);
		/** iterate over all layer's object and find the current origin if possible */
		static bool FindExplicitWorldOrigin(TiledMap::ObjectLayer const * object_layer, glm::vec2 & result, bool absolute_system = true);
		/** iterate over all objects of all layers and find one that is world origin (expressed in absolute coordinates) */
		static bool FindExplicitWorldOrigin(TiledMap::Map const * tiled_map, glm::vec2 & result);

		/** try to read world bounds from object if possible (expressed in object layer coordinates) */
		static bool GetExplicitWorldBounds(TiledMap::GeometricObject const * object_geometric, box2 & result);
		/** iterate over all layer's object and find world bounds if possible) */
		static bool FindExplicitWorldBounds(TiledMap::ObjectLayer const * object_layer, box2 & result, bool absolute_system = true);
		/** iterate over all objects of all layers and find one that is world bounds (expressed in absolute coordinates) */
		static bool FindExplicitWorldBounds(TiledMap::Map const * tiled_map, box2 & result);
	}; 

}; // namespace chaos
