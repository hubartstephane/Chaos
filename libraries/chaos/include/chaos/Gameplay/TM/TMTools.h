namespace chaos
{
	namespace TMTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** serialize layers into JSON */
		template<typename T>
		/*CHAOS_API*/ void SerializeLayersFromJSON(T* object, nlohmann::json const& json)
		{
			if (nlohmann::json const* layers_json = JSONTools::GetArrayNode(json, "LAYERS"))
			{
				for (size_t i = 0; i < layers_json->size(); ++i)
				{
					if (nlohmann::json const* layer_json = JSONTools::GetObjectNodeByIndex(*layers_json, i))
					{
						int layer_id = 0;
						if (JSONTools::GetAttribute(*layer_json, "LAYER_ID", layer_id))
						{
							TMLayerInstance* layer_instance = object->FindLayerInstanceByID(layer_id);
							if (layer_instance != nullptr)
								LoadFromJSON(*layer_json, *layer_instance); // XXX : the indirection is important to avoid the creation of a new layer_instance
						}
					}
				}
			}
		}

		/** search a layer inside an object by ID */
		template<typename T, typename U>
		/*CHAOS_API*/ auto FindLayerInstanceByID(T* object, U& layer_instances, int in_id, bool recursive) -> decltype(layer_instances[0].get())
		{
			for (auto& layer : layer_instances)
			{
				if (layer != nullptr)
				{
					if (layer->GetLayerID() == in_id)
						return layer.get();
					if (recursive)
						if (auto result = layer->FindLayerInstanceByID(in_id, recursive))
							return result;
				}
			}
			return nullptr;
		}

		/** search a layer inside an object by request */
		template<typename T, typename U>
		/*CHAOS_API*/ auto FindLayerInstance(T* object, U& layer_instances, ObjectRequest request, bool recursive) -> decltype(layer_instances[0].get())
		{
			for (auto& layer : layer_instances)
			{
				if (layer != nullptr)
				{
					if (request.Match(*layer.get()))
						return layer.get();
					if (recursive)
						if (TMLayerInstance* result = layer->FindLayerInstance(request, recursive))
							return result;
				}
			}
			return nullptr;
		}

		/** fill BitmapAtlasInput from a TiledMap manager */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a TileSet */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input);
		/** fill BitmapAtlasInput from a Map */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input);

		/** fill BitmapAtlasInput from a TiledMap manager */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a TileSet */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Map */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::FolderInfoInput * folder_input);
		/** fill BitmapAtlasInput from a Layer */
		CHAOS_API bool AddIntoAtlasInput(TiledMap::LayerBase const* layer, BitmapAtlas::FolderInfoInput* folder_input);

		/** returns true whether the object defines an explicit world bounding */
		CHAOS_API bool IsWorldBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object defines an explicit layer bounding */
		CHAOS_API bool IsLayerBoundingBox(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a player start */
		CHAOS_API bool IsPlayerStart(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a camera template */
		CHAOS_API bool IsCameraTemplate(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a finish trigger */
		CHAOS_API bool IsFinishTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a checpoint trigger */
		CHAOS_API bool IsCheckpointTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a notification trigger */
		CHAOS_API bool IsNotificationTrigger(TiledMap::TypedObject const* typed_object);
		/** returns true whether the object is a sound */
		CHAOS_API bool IsSoundTrigger(TiledMap::TypedObject const* typed_object);

#endif

	}; // namespace TMTools

}; // namespace chaos