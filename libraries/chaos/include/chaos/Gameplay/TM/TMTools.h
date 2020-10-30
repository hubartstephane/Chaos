#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace TMTools
	{
	};

}; // namespace chaos

#else

namespace chaos
{
	namespace TMTools
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
		/** fill BitmapAtlasInput from a Layer */
		bool AddIntoAtlasInput(TiledMap::LayerBase const* layer, BitmapAtlas::FolderInfoInput* folder_input);

		/** returns true whether the object defines an explicit world bounding */
		bool IsWorldBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object defines an explicit layer bounding */
		bool IsLayerBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a player start */
		bool IsPlayerStart(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a camera template */
		bool IsCameraTemplate(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a finish trigger */
		bool IsFinishTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a checpoint trigger */
		bool IsCheckpointTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a notification trigger */
		bool IsNotificationTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a sound */
		bool IsSoundTrigger(TiledMap::TypedObject const* typed_object);

	}; // namespace TMTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION