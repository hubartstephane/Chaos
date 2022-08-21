#include "chaos/Chaos.h"

namespace chaos
{
	namespace TiledMap
	{

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		void EightBitsModeTileFlagProcessor::Process(TileLayer* in_layer)
		{
			for (TileLayerChunk& chunk : in_layer->tile_chunks)
			{
				// iterate over all tiles in current chunk
				int count = int(chunk.tile_indices.size());
				for (int i = 0; i < count; ++i)
				{
					Tile& tile = chunk.tile_indices[i];
					tile.flags |= ParticleFlags::EIGHT_BITS_MODE;
				}
			}
		}

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		bool ComputeNeighbourFlagProcessor::ShouldProcessedTiled(Map const* map, Tile tile) const
		{
			// no type declared : process
			if (types.size() == 0)
				return true;
			// unknown tile ?
			TileInfo info = map->FindTileInfo(tile.gid);
			if (!info.IsValid())
				return false;
			// wanted type ?
			for (std::string const& type : types)
				if (info.tiledata->IsObjectOfType(type.c_str()))
					return true;
			return false;
		}

		void ComputeNeighbourFlagProcessor::Process(TileLayer* in_layer)
		{
			Map const* map = in_layer->GetMap();
			if (map == nullptr)
				return;

			int const extra_flags[4] = { TileParticleFlags::NEIGHBOUR_LEFT, TileParticleFlags::NEIGHBOUR_RIGHT, TileParticleFlags::NEIGHBOUR_TOP, TileParticleFlags::NEIGHBOUR_BOTTOM };
			glm::ivec2 const offsets[4] = { glm::ivec2(-1, 0), glm::ivec2(+1, 0), glm::ivec2(0, -1), glm::ivec2(0, +1) };

			// XXX : it could be a good idea to precompute neighbouring chunks.
			//       Nevertheless, nothing seems to guarantee that chunks are well aligned on a grid
			//
			//        +-------+
			//        |       |
			//        |       |+-------+
			//        |       ||       |
			//        +-------+|       |

			for (TileLayerChunk& chunk : in_layer->tile_chunks)
			{
				// iterate over all tiles in current chunk
				int count = int(chunk.tile_indices.size());
				for (int i = 0; i < count; ++i)
				{
					Tile& tile = chunk.tile_indices[i];

					// do not flag empty tile
					if (tile.gid == 0 || !ShouldProcessedTiled(map, tile))
						continue;

					// search all 4 possible neighbours ...
					glm::ivec2 pos = glm::ivec2(
						chunk.offset.x + (i % chunk.size.x),
						chunk.offset.y + (i / chunk.size.x));
					
					int neighbour_flags = 0;
					for (int k = 0; k < 4; ++k)
					{
						glm::ivec2 neighbour = pos + offsets[k];

						// search first in current chunk 
						Tile neighbour_tile;
						if (chunk.ContainTile(neighbour))
							neighbour_tile = chunk.GetTile(neighbour);
						// ... then make a global search if not found
						else
							neighbour_tile = in_layer->GetTile(neighbour);

						if (neighbour_tile.gid == 0 || !ShouldProcessedTiled(map, neighbour_tile))
							continue;
						neighbour_flags |= extra_flags[k];
					}
					tile.flags |= neighbour_flags;
				}
			}
		}

		bool ComputeNeighbourFlagProcessor::SerializeIntoJSON(nlohmann::json& json) const
		{
			if (!TileFlagProcessor::SerializeIntoJSON(json))
				return false;
			// insert all key-flag
			if (types.size())
				JSONTools::SetAttribute(json, "types", types);
			return true;
		}

		bool ComputeNeighbourFlagProcessor::SerializeFromJSON(nlohmann::json const& json)
		{
			if (!TileFlagProcessor::SerializeFromJSON(json))
				return false;
			// get the types of interrest
			JSONTools::GetAttribute(json, "types", types);
			// decrypt the strings
			if (types.size() > 0)
			{
				std::vector<std::string> new_types;
				for (std::string& t : types)
				{
					std::vector<std::string> other_types;
					NameFilter::AddNames(t.c_str(), other_types);

					for (std::string& other_t : other_types)
						new_types.push_back(std::move(other_t));
				}
				types = std::move(new_types);
			}
			return true;
		}

		// ==========================================
		// ComputeCustomFlagProcessor
		// ==========================================

		// while we may want to apply several flags for a same type, and while we can't have identical keys in a JSON object, we use the following syntax
		//
		//	"custom_flags": [
		//		{ "TYPE1, TYPE2, TYPE3": 256 },
		//		{ "TYPE1": 512 }
		//	]
		//
		//  => an array of simple objects (a single KEY-VALUE pair in each object)

		bool SaveIntoJSON(nlohmann::json& json, ComputeCustomFlagProcessorEntry const& src)
		{
			if (!json.is_object())
				json = nlohmann::json::object();
			JSONTools::SetAttribute(json, src.type.c_str(), src.flag);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const& json, ComputeCustomFlagProcessorEntry& dst)
		{
			if (!json.is_object())
				return false;

			auto it = json.begin();
			if (it != json.end())
			{
				try
				{
					dst.flag = it.value().get<int>();
					dst.type = it.key();
				}
				catch (...)
				{
				}
			}
			return true;
		}

		void ComputeCustomFlagProcessor::Process(TileLayer* in_layer)
		{
			Map const* map = in_layer->GetMap();
			if (map == nullptr)
				return;

			for (TileLayerChunk& chunk : in_layer->tile_chunks)
			{
				// iterate over all tiles in current chunk
				for (Tile & tile : chunk.tile_indices)
				{
					TileInfo info = map->FindTileInfo(tile.gid);
					if (!info.IsValid())
						continue;
					// for all declared types, search whether the given tile is of that
					for (ComputeCustomFlagProcessorEntry const & entry : custom_flags)
						if (info.tiledata->IsObjectOfType(entry.type.c_str()))
							tile.flags |= entry.flag;
				}
			}
		}

		bool ComputeCustomFlagProcessor::SerializeIntoJSON(nlohmann::json& json) const
		{
			if (!TileFlagProcessor::SerializeIntoJSON(json))
				return false;
			// insert all key-flag
			if (custom_flags.size())
				JSONTools::SetAttribute(json, "custom_flags", custom_flags);
			return true;
		}
		
		bool ComputeCustomFlagProcessor::SerializeFromJSON(nlohmann::json const& json)
		{
			if (!TileFlagProcessor::SerializeFromJSON(json))
				return false;
			// extract all flag key-flag
			JSONTools::GetAttribute(json, "custom_flags", custom_flags);
			// decrypt all data (a key may be a comma separated value)
			std::vector<ComputeCustomFlagProcessorEntry> final_custom_flags;
			for (ComputeCustomFlagProcessorEntry& old_entry : custom_flags)
			{
				std::vector<std::string> types;
				NameFilter::AddNames(old_entry.type.c_str(), types);
				for (std::string& t : types)
				{
					ComputeCustomFlagProcessorEntry e;
					e.flag = old_entry.flag;
					e.type = std::move(t);
					final_custom_flags.push_back(std::move(e));
				}
			}
			custom_flags = std::move(final_custom_flags);
			return true;
		}

	};  // namespace TiledMap

}; // namespace chaos
