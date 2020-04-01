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
		static bool IsObjectOfType(TiledMap::PropertyOwner const * property_owner, char const * type);
		/** search whether the object has a given type or a boolean/integer property set to true */
		static bool IsObjectOfType(TiledMap::GeometricObject const * object_geometric, char const * type);

		/** returns true whether the object defines an explicit world bounding */
		static bool IsWorldBoundingBox(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object defines an explicit layer bounding */
		static bool IsLayerBoundingBox(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a player start */
		static bool IsPlayerStartObject(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a camera */
		static bool IsCameraObject(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a finish trigger */
		static bool IsFinishTrigger(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a checpoint trigger */
		static bool IsCheckpointTrigger(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a notification trigger */
		static bool IsNotificationTrigger(TiledMap::GeometricObject const* object_geometric);
		/** returns true whether the object is a sound */
		static bool IsSoundTrigger(TiledMap::GeometricObject const* object_geometric);




		/** try to read world bounding box from object if possible */
		static bool GetExplicitWorldBoundingBox(TiledMap::GeometricObject const * object_geometric, box2 & result, bool world_system);
		/** try to read layer bounding box from object if possible */
		static bool GetExplicitLayerBoundingBox(TiledMap::GeometricObject const * object_geometric, box2 & result, bool world_system);
	}; 

}; // namespace chaos
