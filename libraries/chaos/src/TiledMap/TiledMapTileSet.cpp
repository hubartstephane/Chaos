#include <chaos/Chaos.h>

namespace chaos
{
	namespace TiledMap
	{
		// ==========================================
		// Wangset methods
		// ==========================================

		bool WangEdgeColor::DoLoad(tinyxml2::XMLElement const* element)
		{
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "tile", tile_id);
			XMLTools::ReadAttribute(element, "probability", probability);
			ReadXMLColor(element, "color", color);
			return true;
		}

		bool WangTile::DoLoad(tinyxml2::XMLElement const* element)
		{
			XMLTools::ReadAttribute(element, "tileid", tile_id);
			ReadAttributeHEX(element, "wangid", wang_id);
			return true;
		}

		int WangTile::GetCornerValue(Corner corner) const
		{
			return (int)((wang_id >> ((int)corner * 8 + 4)) & 0xF); // each byte encode a CORNER + EDGE (so, x 8)
		}

		int WangTile::GetEdgeValue(Edge edge) const
		{
			return (int)((wang_id >> ((int)edge * 8)) & 0xF); // each byte encode a CORNER + EDGE (so, x 8)
		}

		void WangTile::ApplyParticleFlags(int flags)
		{
			// early exit
			if ((flags & (ParticleFlags::TEXTURE_DIAGONAL_FLIP | ParticleFlags::TEXTURE_HORIZONTAL_FLIP | ParticleFlags::TEXTURE_VERTICAL_FLIP)) == 0)
				return;
			
			// get values
			int top_edge    = GetEdgeValue(Edge::TOP); 
			int right_edge  = GetEdgeValue(Edge::RIGHT);
			int bottom_edge = GetEdgeValue(Edge::BOTTOM);
			int left_edge   = GetEdgeValue(Edge::LEFT);

			int topright_corner    = GetCornerValue(Corner::TOP_RIGHT);
			int bottomright_corner = GetCornerValue(Corner::BOTTOM_RIGHT);
			int bottomleft_corner  = GetCornerValue(Corner::BOTTOM_LEFT);
			int topleft_corner     = GetCornerValue(Corner::TOP_LEFT);

			// apply transforms
			if ((flags & ParticleFlags::TEXTURE_DIAGONAL_FLIP) != 0)
			{
				std::swap(bottomleft_corner, topright_corner);
				std::swap(bottom_edge, right_edge);
				std::swap(left_edge, top_edge);
			}
			if ((flags & ParticleFlags::TEXTURE_HORIZONTAL_FLIP) != 0)
			{
				std::swap(bottomleft_corner, bottomright_corner);
				std::swap(topleft_corner, topright_corner);
				std::swap(left_edge, right_edge);
			}
			if ((flags & ParticleFlags::TEXTURE_VERTICAL_FLIP) != 0)
			{
				std::swap(topleft_corner, bottomleft_corner);
				std::swap(topright_corner, bottomright_corner);
				std::swap(top_edge, bottom_edge);
			}

			// merge values into a single flags
			wang_id =
				(top_edge << 0) |
				(right_edge << 8) |
				(bottom_edge << 16) |
				(left_edge << 24) |
				(topright_corner << 4) |
				(bottomright_corner << 12) |
				(bottomleft_corner << 20) |
				(topleft_corner << 28);
		}

		bool Wangset::DoLoad(tinyxml2::XMLElement const* element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "tile", tile_id);
			XMLTools::ReadAttribute(element, "name", name);

			DoLoadObjectListHelper(element, wang_edge_colors, "wangedgecolor", nullptr, this);
			DoLoadObjectListHelper(element, wang_corner_colors, "wangcornercolor", nullptr, this);
			DoLoadObjectListHelper(element, wang_tiles, "wangtile", nullptr, this);

			// ensure list are sorted for faster search
			std::sort(wang_tiles.begin(), wang_tiles.end(), [](WangTile const& src1, WangTile const& src2) {return src1.tile_id < src2.tile_id; });

			return true;
		}

		WangTile Wangset::GetWangTile(int tile_id) const
		{
			// suppose list sorted
			auto it = std::lower_bound(wang_tiles.begin(), wang_tiles.end(), tile_id, [](WangTile const& wang_tile, int id) { return wang_tile.tile_id < id ; });
			if (it != wang_tiles.end() && it->tile_id == tile_id) // maybe the iterator is invalid or does not point exactly the element whose ID is the one searched (this is a lower_bound !!)
				return *it;
			// returns empty element
			return {};
		}

		// ==========================================
		// GroundData methods
		// ==========================================

		bool GroundData::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "tile", tile_id);
			XMLTools::ReadAttribute(element, "name", name);
			return true;
		}

		// ==========================================
		// TileData methods
		// ==========================================

		bool TileData::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!TypedObject::DoLoad(element))
				return false;

			XMLTools::ReadAttribute(element, "id", id);
			XMLTools::ReadAttribute(element, "probability", probability);

			std::string terrain;
			if (XMLTools::ReadAttribute(element, "terrain", terrain))
				if (!ComputeTerrainIndices(terrain.c_str()))
					return false;

			// some tilesets have a single image (representing an atlas with elements in a grid cells)
			// some tilesets have individual images per tiles
			tinyxml2::XMLElement const * image_element = element->FirstChildElement("image");
			if (image_element != nullptr)
			{
				if (!XMLTools::ReadAttribute(image_element, "source", image_path))
					return false;
				if (!XMLTools::ReadAttribute(image_element, "width", image_size.x))
					return false;
				if (!XMLTools::ReadAttribute(image_element, "height", image_size.y))
					return false;

				image_path = BoostTools::FindAbsolutePath(GetOwnerPath(), image_path);
				atlas_key = BoostTools::PathToName(image_path);
			}

			DoLoadObjectListHelper(element, object_layers, "objectgroup", nullptr, this);

			return true;
		}

		// "0,,," => top left
		// ",0,," => top right
		// ",,0," => bottom left
		// ",,,0" => bottom right
		bool TileData::ComputeTerrainIndices(char const * str)
		{
			assert(str != nullptr);

			// by default initialization
			for (size_t index = 0; index < 4; ++index)
				terrain_indices[index] = -1;

			// parse the string
			size_t index = 0;

			size_t i = 0;
			while (str[i] != 0)
			{
				if (std::isdigit(str[i]))
				{
					terrain_indices[index] = atoi(&str[i]);
					i = StringTools::SkipNumber(&str[i]) - str;
					if (index == 4)
						break;
				}
				else
				{
					if (str[i] == ',')
						++index;
					++i;
				}
			}
			return true;
		}

		// ==========================================
		// TileSet methods
		// ==========================================

		TileData const * TileSet::FindTileData(int id) const
		{
			auto it = std::lower_bound(tiles.begin(), tiles.end(), id, [](shared_ptr<TileData> const& tile_data, int id) { return tile_data->id < id; });
			if (it == tiles.end() || (*it)->id != id) // maybe the iterator is invalid or does not point exactly the element whose ID is the one searched (this is a lower_bound !!)
				return nullptr;
			return (*it).get();
		}

#define CHAOS_IMPL_FIND_TILE_DATA(func_name, arg_type, member_name)\
		TileData const * TileSet::func_name(arg_type arg_name) const\
		{\
			size_t count = tiles.size();\
			for (size_t i = 0; i < count; ++i)\
			{\
				TileData const * tile_data = tiles[i].get();\
				if (tile_data == nullptr)\
					continue;\
				if (tile_data->member_name == arg_name)\
					return tile_data;\
			}\
			return nullptr;\
		}

		CHAOS_IMPL_FIND_TILE_DATA(FindTileData, char const*, type);
		CHAOS_IMPL_FIND_TILE_DATA(FindTileDataFromAtlasKey, char const*, atlas_key);
#undef CHAOS_IMPL_FIND_TILE_DATA

		Wangset const * TileSet::FindWangset(char const* name) const
		{
			size_t count = wangsets.size();
			for (size_t i = 0; i < count; ++i)
			{
				Wangset const * wang_set = wangsets[i].get();
				if (wang_set == nullptr)
					continue;
				if (wang_set->name == name)
					return wang_set;
			}
			return nullptr;
		}

		bool TileSet::DoLoadGrounds(tinyxml2::XMLElement const * element)
		{
			if (!DoLoadObjectListHelper(element, grounds, "terrain", "terraintypes", this))
				return false;
			return true;
		}

		bool TileSet::DoLoadWangsets(tinyxml2::XMLElement const* element)
		{
			if (!DoLoadObjectListHelper(element, wangsets, "wangset", "wangsets", this))
				return false;
			return true;
		}

		bool TileSet::DoLoadTiles(tinyxml2::XMLElement const * element)
		{
			if (!DoLoadObjectListHelper(element, tiles, "tile", nullptr, this))
				return false;

			// ensure faster access to tiles by sorted them
			size_t tile_count = tiles.size();
			if (tile_count > 0)
			{
				std::sort(tiles.begin(), tiles.end(), [](shared_ptr<TileData> const& src1, shared_ptr<TileData> const& src2) { return src1->id < src2->id; });

				min_tile_id = tiles[0]->id;
				max_tile_id = tiles[tile_count - 1]->id;
			}
			return true;
		}

		bool TileSet::DoLoadMembers(tinyxml2::XMLElement const * element)
		{
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "tilewidth", tile_size.x);
			XMLTools::ReadAttribute(element, "tileheight", tile_size.y);
			XMLTools::ReadAttribute(element, "tilecount", tile_count);
			XMLTools::ReadAttribute(element, "columns", columns);
			XMLTools::ReadAttribute(element, "margin", image_margin);
			XMLTools::ReadAttribute(element, "spacing", image_spacing);

			// XXX : theses tokens are not exactly the same that for JSON serialization !!!
			static std::vector<std::pair<Hotpoint, char const*>> const hotpoint_map =
			{				
				{ Hotpoint::TOP, "top" },
				{ Hotpoint::BOTTOM, "bottom" },
				{ Hotpoint::LEFT, "left" },
				{ Hotpoint::RIGHT, "right" },
				{ Hotpoint::TOP_LEFT, "topleft" },
				{ Hotpoint::TOP_RIGHT, "topright" },
				{ Hotpoint::BOTTOM_LEFT, "bottomleft" },
				{ Hotpoint::BOTTOM_RIGHT, "bottomright" },
				{ Hotpoint::CENTER, "center" },
				{ Hotpoint::BOTTOM_LEFT, nullptr }				
			};
			XMLTools::ReadEnumAttribute(element, "objectalignment", hotpoint_map, object_alignment);

			ReadXMLColor(element, "backgroundcolor", background_color);

			tinyxml2::XMLElement const * grid_element = element->FirstChildElement("grid");
			if (grid_element != nullptr)
			{
				static std::vector<std::pair<TileSetOrientation, char const*>> const orientation_map = {
					{ TileSetOrientation::ISOMETRIC, "isometric" },
					{ TileSetOrientation::ORTHOGONAL, nullptr }
				};
				XMLTools::ReadEnumAttribute(grid_element, "orientation", orientation_map, orientation);

				XMLTools::ReadAttribute(grid_element, "width", size.x);
				XMLTools::ReadAttribute(grid_element, "height", size.y);
			}

			tinyxml2::XMLElement const * image_element = element->FirstChildElement("image");
			if (image_element != nullptr)
			{
				XMLTools::ReadAttribute(image_element, "source", image_path);
				image_path = BoostTools::FindAbsolutePath(GetOwnerPath(), image_path);

				ReadXMLColor(image_element, "trans", transparent_color);

				XMLTools::ReadAttribute(image_element, "width", image_size.x);
				XMLTools::ReadAttribute(image_element, "height", image_size.y);
			}

			return true;
		}

		bool TileSet::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!ManagerObject::DoLoad(element))
				return false;
			if (!DoLoadTiles(element))
				return false;
			if (!DoLoadGrounds(element))
				return false;
			if (!DoLoadWangsets(element))
				return false;
			return true;
		}

	};  // namespace TiledMap

}; // namespace chaos
