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
		static bool IsObjectOfType(TiledMap::TypedObject const * typed_object, char const * type);

		/** returns true whether the object defines an explicit world bounding */
		static bool IsWorldBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object defines an explicit layer bounding */
		static bool IsLayerBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a player start */
		static bool IsPlayerStartObject(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a camera */
		static bool IsCameraObject(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a finish trigger */
		static bool IsFinishTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a checpoint trigger */
		static bool IsCheckpointTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a notification trigger */
		static bool IsNotificationTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a sound */
		static bool IsSoundTrigger(TiledMap::TypedObject const* typed_object);

		/** split the tile pseudo GID into GID/H-FLIP/V-FLIP */
		static int GetTileGID(int pseudo_gid, bool* horizontal_flip = nullptr, bool* vertical_flip = nullptr, bool * diagonal_flip = nullptr);
	}; 

}; // namespace chaos
