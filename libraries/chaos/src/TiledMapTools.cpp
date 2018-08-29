#include <chaos/TiledMapTools.h>

namespace chaos
{
	namespace TiledMapTools
	{

		bool GenerateAtlasInput(TiledMap::Map const * map, BitmapAtlas::AtlasInput & input, char const * folder_name)
		{
			assert(map != nullptr);
			assert(folder_name != nullptr);

			// create the FolderInfoInput (if not already existing)
			BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder(folder_name, 0);
			if (folder_input == nullptr)
				return false;

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

		bool GenerateAtlasInput(TiledMap::TileSet const * tile_set, BitmapAtlas::AtlasInput & input, char const * folder_name)
		{
			assert(tile_set != nullptr);
			assert(folder_name != nullptr);

			// create the FolderInfoInput (if not already existing)
			BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder(folder_name, 0);
			if (folder_input == nullptr)
				return false;

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

		bool GenerateAtlasInput(TiledMap::Manager const * manager, BitmapAtlas::AtlasInput & input, char const * folder_name)
		{
			assert(manager != nullptr);
			assert(folder_name != nullptr);

			// create the FolderInfoInput (if not already existing)
			BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder(folder_name, 0);
			if (folder_input == nullptr)
				return false;

			// insert all images in any referenced in TiledSet
			size_t tile_set_count = manager->tile_sets.size();
			for (size_t i = 0; i < tile_set_count; ++i)
			{
				TiledMap::TileSet const * tile_set = manager->tile_sets[i].get();
				if (tile_set == nullptr)
					continue;
				if (!GenerateAtlasInput(tile_set, input, folder_name))
					return false;
			}

			// images in the maps
			size_t map_count = manager->maps.size();
			for (size_t i = 0; i < map_count; ++i)
			{
				TiledMap::Map const * map = manager->maps[i].get();
				if (map == nullptr)
					continue;
				if (!GenerateAtlasInput(map, input, folder_name))
					return false;
			}
			return true;
		}



	};  // namespace TiledMapTools

}; // namespace chaos
