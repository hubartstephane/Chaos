#include <death/TM.h>

namespace death
{
	namespace TMTools
	{
		bool AddIntoAtlasInput(chaos::TiledMap::Manager const* manager, chaos::BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(manager, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(chaos::TiledMap::TileSet const* tile_set, chaos::BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(tile_set, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(chaos::TiledMap::Map const* map, chaos::BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(map, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(chaos::TiledMap::LayerBase const* layer, chaos::BitmapAtlas::FolderInfoInput* folder_input)
		{
			assert(layer != nullptr);
			assert(folder_input != nullptr);

			if (chaos::TiledMap::ImageLayer const* image_layer = auto_cast(layer))
			{
				if (image_layer->image_path.size() > 0)
					folder_input->AddBitmap(image_layer->image_path, nullptr, 0);
			}
			else if (chaos::TiledMap::GroupLayer const* group_layer = auto_cast(layer))
			{
				size_t count = group_layer->layers.size();
				for (size_t i = 0; i < count; ++i)
					if (!AddIntoAtlasInput(group_layer->layers[i].get(), folder_input))
						return false;
			}
			return true;
		}

		bool AddIntoAtlasInput(chaos::TiledMap::Map const* map, chaos::BitmapAtlas::FolderInfoInput* folder_input)
		{
			assert(map != nullptr);
			assert(folder_input != nullptr);

			// insert all image layers in the map
			size_t count = map->layers.size();
			for (size_t i = 0; i < count; ++i)
				if (!AddIntoAtlasInput(map->layers[i].get(), folder_input))
					return false;
			return true;
		}

		bool AddIntoAtlasInput(chaos::TiledMap::TileSet const* tile_set, chaos::BitmapAtlas::FolderInfoInput* folder_input)
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
				chaos::TiledMap::TileData const* tile_data = tile_set->tiles[j].get();
				if (tile_data == nullptr)
					continue;
				if (tile_data->image_path.size() > 0)
					folder_input->AddBitmap(tile_data->image_path, nullptr, 0);
			}
			return true;
		}

		bool AddIntoAtlasInput(chaos::TiledMap::Manager const* manager, chaos::BitmapAtlas::FolderInfoInput* folder_input)
		{
			assert(manager != nullptr);
			assert(folder_input != nullptr);

			// insert all images in any referenced in TiledSet
			size_t tile_set_count = manager->tile_sets.size();
			for (size_t i = 0; i < tile_set_count; ++i)
			{
				chaos::TiledMap::TileSet const* tile_set = manager->tile_sets[i].get();
				if (tile_set == nullptr)
					continue;
				if (!AddIntoAtlasInput(tile_set, folder_input))
					return false;
			}

			// images in the maps
			size_t map_count = manager->maps.size();
			for (size_t i = 0; i < map_count; ++i)
			{
				chaos::TiledMap::Map const* map = manager->maps[i].get();
				if (map == nullptr)
					continue;
				if (!AddIntoAtlasInput(map, folder_input))
					return false;
			}
			return true;
		}

		bool IsWorldBoundingBox(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("WorldBoundingBox");
		}

		bool IsLayerBoundingBox(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("LayerBoundingBox");
		}

		bool IsPlayerStart(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("PlayerStart");
		}

		bool IsCameraTemplate(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Camera");
		}

		bool IsFinishTrigger(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Finish");
		}

		bool IsCheckpointTrigger(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Checkpoint");
		}

		bool IsNotificationTrigger(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Notification");
		}

		bool IsSoundTrigger(chaos::TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Sound");
		}

	}; // namespace TMTools

}; // namespace death
