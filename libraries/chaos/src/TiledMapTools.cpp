#include <chaos/TiledMapTools.h>

namespace chaos
{
	namespace TiledMapTools
	{
		bool AddIntoAtlasInput(TiledMap::Manager const* manager, BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(manager, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::TileSet const* tile_set, BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(tile_set, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::Map const* map, BitmapAtlas::AtlasInput& input)
		{
			return AddIntoAtlasInput(map, input.GetRootFolder());
		}

		bool AddIntoAtlasInput(TiledMap::LayerBase const* layer, BitmapAtlas::FolderInfoInput* folder_input)
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

		bool AddIntoAtlasInput(TiledMap::Map const* map, BitmapAtlas::FolderInfoInput* folder_input)
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

		bool AddIntoAtlasInput(TiledMap::TileSet const* tile_set, BitmapAtlas::FolderInfoInput* folder_input)
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

		bool AddIntoAtlasInput(TiledMap::Manager const* manager, BitmapAtlas::FolderInfoInput* folder_input)
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

		bool IsPlayerStartObject(TiledMap::TypedObject const* typed_object)
		{
			return typed_object->IsObjectOfType("PlayerStart");
		}

		bool IsCameraObject(TiledMap::TypedObject const* typed_object)
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

		int DecodeTileGID(int pseudo_gid, bool* horizontal_flip, bool* vertical_flip, bool* diagonal_flip)
		{
			// see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/
			const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
			const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
			const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

			std::int32_t tmp = (std::int32_t)pseudo_gid;

			std::int32_t gid = tmp & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

			if (horizontal_flip != nullptr)
				*horizontal_flip = (tmp & FLIPPED_HORIZONTALLY_FLAG);
			if (vertical_flip != nullptr)
				*vertical_flip = (tmp & FLIPPED_VERTICALLY_FLAG);
			if (diagonal_flip != nullptr)
				*diagonal_flip = (tmp & FLIPPED_DIAGONALLY_FLAG);

			return (int)gid;
		}

	}; // namespace TiledMapTools

}; // namespace chaos
