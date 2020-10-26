#pragma once

#include <Chaos/Chaos.h>

namespace death
{
	namespace TMTools
	{
		/** fill BitmapAtlasInput from a TiledMap manager */
		bool AddIntoAtlasInput(chaos::TiledMap::Manager const * manager, chaos::BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a TileSet */
		bool AddIntoAtlasInput(chaos::TiledMap::TileSet const * tile_set, chaos::BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a Map */
		bool AddIntoAtlasInput(chaos::TiledMap::Map const * map, chaos::BitmapAtlas::AtlasInput & input);

		/** fill BitmapAtlasInput from a TiledMap manager */
		bool AddIntoAtlasInput(chaos::TiledMap::Manager const * manager, chaos::BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a TileSet */
		bool AddIntoAtlasInput(chaos::TiledMap::TileSet const * tile_set, chaos::BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Map */
		bool AddIntoAtlasInput(chaos::TiledMap::Map const * map, chaos::BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Layer */
		bool AddIntoAtlasInput(chaos::TiledMap::LayerBase const* layer, chaos::BitmapAtlas::FolderInfoInput* folder_input);

		/** returns true whether the object defines an explicit world bounding */
		bool IsWorldBoundingBox(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object defines an explicit layer bounding */
		bool IsLayerBoundingBox(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a player start */
		bool IsPlayerStart(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a camera template */
		bool IsCameraTemplate(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a finish trigger */
		bool IsFinishTrigger(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a checpoint trigger */
		bool IsCheckpointTrigger(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a notification trigger */
		bool IsNotificationTrigger(chaos::TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a sound */
		bool IsSoundTrigger(chaos::TiledMap::TypedObject const* typed_object);

	}; // namespace TMTools

}; // namespace death
