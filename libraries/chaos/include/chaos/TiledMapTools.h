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

		/** returns true whether the object defines an explicit world bounding */
		static bool IsWorldBoundingBox(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object defines an explicit layer bounding */
		static bool IsLayerBoundingBox(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a player start */
		static bool IsPlayerStart(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a trigger surface */
		static bool IsTriggerSurface(TiledMap::GeometricObject const * object_geometric);
		/** returns true whether the object is a camera */
		static bool IsCamera(TiledMap::GeometricObject const * object_geometric);

		/** try to read world bounding box from object if possible */
		static bool GetExplicitWorldBoundingBox(TiledMap::GeometricObject const * object_geometric, box2 & result, bool world_system);
		/** try to read layer bounding box from object if possible */
		static bool GetExplicitLayerBoundingBox(TiledMap::GeometricObject const * object_geometric, box2 & result, bool world_system);
	}; 

}; // namespace chaos
