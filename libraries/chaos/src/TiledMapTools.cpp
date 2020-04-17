#include <chaos/TiledMapTools.h>

namespace chaos
{
	bool TiledMapTools::AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input)
	{
		return AddIntoAtlasInput(manager, input.GetRootFolder());
	}

	bool TiledMapTools::AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input)
	{
		return AddIntoAtlasInput(tile_set, input.GetRootFolder());
	}

	bool TiledMapTools::AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input)
	{
		return AddIntoAtlasInput(map, input.GetRootFolder());
	}

	bool TiledMapTools::AddIntoAtlasInput(TiledMap::Map const * map, BitmapAtlas::FolderInfoInput * folder_input)
	{
		assert(map != nullptr);
		assert(folder_input != nullptr);

		// insert all image layers in the map
		size_t image_layer = map->image_layers.size();
		for (size_t i = 0; i < image_layer; ++i)
		{
			TiledMap::ImageLayer const * image_layer = map->image_layers[i].get();
			if (image_layer != nullptr)
				continue;
			if (image_layer->image_path.size() > 0)
				folder_input->AddBitmap(image_layer->image_path, nullptr, 0);
		}
		return true;
	}

	bool TiledMapTools::AddIntoAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::FolderInfoInput * folder_input)
	{
		assert(tile_set != nullptr);
		assert(folder_input != nullptr);

		// the 'single' image for the whole tile set
		if (tile_set->image_path.size() > 0)
			folder_input->AddBitmap(tile_set->image_path, nullptr, 0);

		// enumerate all TileData
		size_t tile_count = tile_set->tiles.size();
		for (size_t j = 0; j < tile_count; ++j)
		{
			TiledMap::TileData const * tile_data = tile_set->tiles[j].get();
			if (tile_data == nullptr)
				continue;
			if (tile_data->image_path.size() > 0)
				folder_input->AddBitmap(tile_data->image_path, nullptr, 0);
		}
		return true;
	}

	bool TiledMapTools::AddIntoAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::FolderInfoInput * folder_input)
	{
		assert(manager != nullptr);
		assert(folder_input != nullptr);

		// insert all images in any referenced in TiledSet
		size_t tile_set_count = manager->tile_sets.size();
		for (size_t i = 0; i < tile_set_count; ++i)
		{
			TiledMap::TileSet const * tile_set = manager->tile_sets[i].get();
			if (tile_set == nullptr)
				continue;
			if (!AddIntoAtlasInput(tile_set, folder_input))
				return false;
		}

		// images in the maps
		size_t map_count = manager->maps.size();
		for (size_t i = 0; i < map_count; ++i)
		{
			TiledMap::Map const * map = manager->maps[i].get();
			if (map == nullptr)
				continue;
			if (!AddIntoAtlasInput(map, folder_input))
				return false;
		}
		return true;
	}

	bool TiledMapTools::IsObjectOfType(TiledMap::PropertyOwner const * property_owner, char const * type)
	{
		assert(property_owner != nullptr);
		// search in properties
		if (type != nullptr)
		{
			TiledMap::Property const * property = property_owner->FindProperty(type);
			if (property != nullptr)
			{
				bool const * property_bool = property->GetBoolProperty();
				if (property_bool != nullptr)
					return *property_bool;

				int const * property_int = property->GetIntProperty();
				if (property_int != nullptr)
					return (*property_int > 0);
			}
		}
		return false;
	}

	bool TiledMapTools::IsObjectOfType(TiledMap::TypedObject const * typed_object, char const * type)
	{
		assert(typed_object != nullptr);
		// type is an indicator
		if (type != nullptr && chaos::StringTools::Stricmp(typed_object->type, type) == 0)
			return true;
		// search in properties
		TiledMap::PropertyOwner const * property_owner = typed_object;
		if (IsObjectOfType(property_owner, type))
			return true;
		return false;
	}

	bool TiledMapTools::IsWorldBoundingBox(TiledMap::TypedObject const * typed_object)
	{
		return IsObjectOfType(typed_object, "WorldBoundingBox");
	}

	bool TiledMapTools::IsLayerBoundingBox(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "LayerBoundingBox");
	}

	bool TiledMapTools::IsPlayerStartObject(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "PlayerStart");
	}

	bool TiledMapTools::IsCameraObject(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "Camera");
	}

	bool TiledMapTools::IsFinishTrigger(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "Finish");
	}

	bool TiledMapTools::IsCheckpointTrigger(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "Checkpoint");
	}

	bool TiledMapTools::IsNotificationTrigger(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "Notification");
	}

	bool TiledMapTools::IsSoundTrigger(TiledMap::TypedObject const* typed_object)
	{
		return IsObjectOfType(typed_object, "Sound");
	}

	int TiledMapTools::GetTileGID(int pseudo_gid, bool* horizontal_flip, bool* vertical_flip)
	{
		int gid = (pseudo_gid & ~((1 << 31) | (1 << 30)));

		if (horizontal_flip != nullptr)
			*horizontal_flip = ((pseudo_gid & (1 << 31)) != 0);
		if (vertical_flip != nullptr)
			*vertical_flip = ((pseudo_gid & (1 << 30)) != 0);

		return gid;
	}

}; // namespace chaos
