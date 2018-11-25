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

	bool TiledMapTools::HasFlag(TiledMap::PropertyOwner const * property_owner, char const * property_name)
	{
		assert(property_owner != nullptr);
		// search in properties
		if (property_name != nullptr)
		{
			TiledMap::Property const * property = property_owner->FindProperty(property_name);
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

	bool TiledMapTools::HasFlag(TiledMap::GeometricObject const * object_geometric, char const * name, char const * type, char const * property_name)
	{
		assert(object_geometric != nullptr);
		// name is an indicator
		if (name != nullptr && object_geometric->name == name)
			return true;
		// type is an indicator
		if (type != nullptr && object_geometric->type == type)
			return true;
		// search in properties
		TiledMap::PropertyOwner const * property_owner = object_geometric;
		if (HasFlag(property_owner, property_name))
			return true;
		return false;
	}

	bool TiledMapTools::IsWorldOrigin(TiledMap::GeometricObject const * object_geometric)
	{
		return HasFlag(object_geometric, "world_origin", "world_origin", "WORLD_ORIGIN");
	}

	bool TiledMapTools::IsWorldBounds(TiledMap::GeometricObject const * object_geometric)
	{
		return HasFlag(object_geometric, "world_bounds", "world_bounds", "WORLD_BOUNDS");
	}

	bool TiledMapTools::IsPlayerStart(TiledMap::GeometricObject const * object_geometric)
	{
		return HasFlag(object_geometric, "player_start", "player_start", "PLAYER_START");
	}

	bool TiledMapTools::IsCamera(TiledMap::GeometricObject const * object_geometric)
	{
		return HasFlag(object_geometric, "camera", "camera", "CAMERA");
	}

	bool TiledMapTools::GetExplicitWorldOrigin(TiledMap::GeometricObject const * object_geometric, glm::vec2 & result)
	{
		if (object_geometric == nullptr)
			return false;
		if (IsWorldOrigin(object_geometric))
		{
			result = object_geometric->position;
			return true;
		}
		return false;
	}

	bool TiledMapTools::FindExplicitWorldOrigin(TiledMap::ObjectLayer const * object_layer, glm::vec2 & result, bool absolute_system)
	{
		if (object_layer == nullptr)
			return false;
		for (size_t i = 0; i < object_layer->geometric_objects.size(); ++i)
		{
			TiledMap::GeometricObject const * object = object_layer->geometric_objects[i].get();
			if (GetExplicitWorldOrigin(object, result))
			{
				if (absolute_system)
					result += object_layer->offset;
				return true;
			}
		}
		return false;
	}

	bool TiledMapTools::FindExplicitWorldOrigin(TiledMap::Map const * tiled_map, glm::vec2 & result)
	{
		if (tiled_map == nullptr)
			return false;
		for (size_t i = 0; i < tiled_map->object_layers.size(); ++i)
		{
			TiledMap::ObjectLayer const * object_layer = tiled_map->object_layers[i].get();
			if (FindExplicitWorldOrigin(object_layer, result, true))
				return true;
		}
		return false;
	}

	bool TiledMapTools::GetExplicitWorldBounds(TiledMap::GeometricObject const * object_geometric, box2 & result)  // expressed in layer coordinates
	{
		if (object_geometric == nullptr)
			return false;
		TiledMap::GeometricObjectSurface const * object_surface = object_geometric->GetObjectSurface();
		if (object_surface == nullptr)
			return false;
		if (TiledMapTools::IsWorldBounds(object_surface))
		{
			result = object_surface->GetBoundingBox();
			return true;
		}
		return false;
	}

	bool TiledMapTools::FindExplicitWorldBounds(TiledMap::ObjectLayer const * object_layer, box2 & result, bool absolute_system)
	{
		if (object_layer == nullptr)
			return false;
		for (size_t i = 0; i < object_layer->geometric_objects.size(); ++i)
		{
			TiledMap::GeometricObject const * object = object_layer->geometric_objects[i].get();
			if (GetExplicitWorldBounds(object, result))
			{
				if (absolute_system)
					result.position += object_layer->offset;
				return true;
			}
		}
		return false;
	}

	bool TiledMapTools::FindExplicitWorldBounds(TiledMap::Map const * tiled_map, box2 & result) // expressed in map coordinates
	{
		if (tiled_map == nullptr)
			return false;
		for (size_t i = 0; i < tiled_map->object_layers.size(); ++i)
		{
			TiledMap::ObjectLayer const * object_layer = tiled_map->object_layers[i].get();
			if (FindExplicitWorldBounds(object_layer, result, true))
				return true;
		}
		return false;
	}

}; // namespace chaos
