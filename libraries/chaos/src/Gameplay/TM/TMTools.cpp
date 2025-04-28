#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace TMTools
	{
		bool AddIntoAtlasInput(TiledMap::Manager const* manager, AtlasInput& input)
		{
			return AddIntoAtlasInput(manager, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::TileSet const* tile_set, AtlasInput& input)
		{
			return AddIntoAtlasInput(tile_set, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::Map const* map, AtlasInput& input)
		{
			return AddIntoAtlasInput(map, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::LayerBase const* layer, AtlasFolderInfoInput* folder_input)
		{
			assert(layer != nullptr);
			assert(folder_input != nullptr);

			if (TiledMap::ImageLayer const* image_layer = auto_cast(layer))
			{
				if (image_layer->image_path.size() > 0)
					folder_input->AddBitmap(image_layer->image_path, nullptr, 0);
			}
			else if (TiledMap::GroupLayer const* group_layer = auto_cast(layer))
			{
				size_t count = group_layer->layers.size();
				for (size_t i = 0; i < count; ++i)
					if (!AddIntoAtlasInput(group_layer->layers[i].get(), folder_input))
						return false;
			}
			return true;
		}

		bool AddIntoAtlasInput(TiledMap::Map const* map, AtlasFolderInfoInput* folder_input)
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

		bool AddIntoAtlasInput(TiledMap::TileSet const* tile_set, AtlasFolderInfoInput* folder_input)
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
				TiledMap::TileData const* tile_data = tile_set->tiles[j].get();
				if (tile_data == nullptr)
					continue;
				if (tile_data->image_path.size() > 0)
					folder_input->AddBitmap(tile_data->image_path, nullptr, 0);
			}
			return true;
		}

		bool AddIntoAtlasInput(TiledMap::Manager const* manager, AtlasFolderInfoInput* folder_input)
		{
			assert(manager != nullptr);
			assert(folder_input != nullptr);

			// insert all images in any referenced in TiledSet
			size_t tile_set_count = manager->tile_sets.size();
			for (size_t i = 0; i < tile_set_count; ++i)
			{
				TiledMap::TileSet const* tile_set = manager->tile_sets[i].get();
				if (tile_set == nullptr)
					continue;
				if (!AddIntoAtlasInput(tile_set, folder_input))
					return false;
			}

			// images in the maps
			size_t map_count = manager->maps.size();
			for (size_t i = 0; i < map_count; ++i)
			{
				TiledMap::Map const* map = manager->maps[i].get();
				if (map == nullptr)
					continue;
				if (!AddIntoAtlasInput(map, folder_input))
					return false;
			}
			return true;
		}

		bool IsWorldBoundingBox(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("WorldBoundingBox");
		}

		bool IsLayerBoundingBox(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("LayerBoundingBox");
		}

		bool IsPlayerStart(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("PlayerStart");
		}

		bool IsCameraTemplate(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Camera");
		}

		bool IsFinishTrigger(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Finish");
		}

		bool IsCheckpointTrigger(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Checkpoint");
		}

		bool IsNotificationTrigger(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Notification");
		}

		bool IsSoundTrigger(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("Sound");
		}

	}; // namespace TMTools

}; // namespace chaos
