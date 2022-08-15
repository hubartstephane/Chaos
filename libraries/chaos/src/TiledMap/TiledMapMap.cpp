#include <chaos/Chaos.h>

namespace chaos
{
	namespace TiledMap
	{
		static std::vector<std::pair<MapOrientation, char const*>> const orient_map = {
			{ MapOrientation::ORTHOGONAL, "orthogonal" }, //default
			{ MapOrientation::ISOMETRIC, "isometric" },
			{ MapOrientation::STAGGERED, "staggered" },
			{ MapOrientation::HEXAGONAL, "hexagonal" }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(MapOrientation, orient_map);

		static std::vector < std::pair<StaggerAxis, char const*>> const stagger_axis_map = {
			{ StaggerAxis::AXIS_X, "X",  }, // default
			{ StaggerAxis::AXIS_Y, "Y",  }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(StaggerAxis, stagger_axis_map);

		static std::vector<std::pair<StaggerIndex, char const*>> const stagger_index_map = {
			{ StaggerIndex::ODD, "odd" }, // default
			{ StaggerIndex::EVEN, "even" }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(StaggerIndex, stagger_index_map);

		static std::vector<std::pair<RenderOrder, char const*>> const render_order_map = {
			{ RenderOrder::RIGHT_UP, "right-up" }, // default
			{ RenderOrder::RIGHT_DOWN, "right-down" },
			{ RenderOrder::LEFT_UP, "left-up" },
			{ RenderOrder::LEFT_DOWN, "left-down" }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(RenderOrder, render_order_map);

		// ==========================================
		// Map methods
		// ==========================================

		bool Map::DoLoadMembers(tinyxml2::XMLElement const * element)
		{

			orientation = MapOrientation::ORTHOGONAL;
			XMLTools::ReadAttribute(element, "orientation", orientation);
			stagger_axis = StaggerAxis::AXIS_X;
			XMLTools::ReadAttribute(element, "staggeraxis", stagger_axis);
			stagger_index = StaggerIndex::ODD;
			XMLTools::ReadAttribute(element, "staggerindex", stagger_index);
			render_order = RenderOrder::RIGHT_UP;
			XMLTools::ReadAttribute(element, "renderorder", render_order);
			XMLTools::ReadAttribute(element, "compressionlevel", compressionlevel);
			XMLTools::ReadAttribute(element, "version", version);
			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);
			XMLTools::ReadAttribute(element, "tilewidth", tile_size.x);
			XMLTools::ReadAttribute(element, "tileheight", tile_size.y);
			XMLTools::ReadAttribute(element, "infinite", infinite);
			XMLTools::ReadAttribute(element, "hexsidelength", hex_side_length);

			ReadXMLColor(element, "backgroundcolor", background_color);

			return true;
		}

		bool Map::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!ManagerObject::DoLoad(element))
				return false;
			if (!DoLoadTileSet(element))
				return false;
			if (!DoLoadLayers(element))
				return false;
			return true;
		}

		bool Map::DoLoadTileSet(tinyxml2::XMLElement const * element)
		{
			assert(element != nullptr);

			// get the manager
			Manager * manager = GetOwner<Manager>();
			if (manager == nullptr)
				return false;

			tinyxml2::XMLElement const * tileset_element = element->FirstChildElement("tileset");
			for (; tileset_element != nullptr; tileset_element = tileset_element->NextSiblingElement("tileset"))
			{
				int first_gid = 0;
				std::string source;

				if (!XMLTools::ReadAttribute(tileset_element, "firstgid", first_gid)) // firstgid is mandatory (map would be incomplete)
					return false;

				// external tileset
				TileSet * tileset = nullptr;

				if (XMLTools::ReadAttribute(tileset_element, "source", source)) // source is mandatory (map would be incomplete)
				{
					boost::filesystem::path tileset_path = PathTools::FindAbsolutePath(path, source); // compute the path of the tileset relative to this

					tileset = manager->LoadTileSet(tileset_path);
					if (tileset == nullptr)
						return false;
				}
				// embedded titleset
				else
				{
					tileset = new TileSet(this, boost::filesystem::path());
					if (tileset == nullptr)
						return false;

					if (!tileset->DoLoad(tileset_element))
					{
						delete(tileset);
						return false;
					}
					manager->tile_sets.push_back(tileset);
				}

				TileSetData data;
				data.first_gid   = first_gid;
				data.min_tile_id = first_gid + tileset->min_tile_id;
				data.max_tile_id = first_gid + tileset->max_tile_id;
				data.tileset = tileset;
				tilesets.push_back(data);
			}
			return true;
		}

		size_t Map::GetLayerCount() const
		{
			return layers.size();
		}

		bool Map::DoLoadLayers(tinyxml2::XMLElement const * element)
		{
			return DoLoadLayersImpl(element, layers);
		}

		TileInfo Map::FindTileInfo(int gid) const
		{
			// early exit
			if (gid <= 0)
				return {};
			// search the tileset that contains this GLOBAL ID
			auto it = STLTools::FindSortedVector(tilesets, gid, [](TileSetData const& ts, int gid)
			{
				if (gid < ts.min_tile_id)
					return -1;
				if (gid > ts.max_tile_id)
					return +1;
				return 0;
			});
			// no such tileset
			if (it == tilesets.end())
				return {};
			// search the tile data in the tileset
			TileData const * tiledata = it->tileset->FindTileData(gid - it->first_gid);
			if (tiledata == nullptr)
				return {};
				
			return TileInfo((gid - it->first_gid), it->tileset.get(), tiledata);
		}

		LayerBase* Map::FindLayer(char const* in_name)
		{
			for (auto& layer : layers)
				if (StringTools::Stricmp(layer->GetName(), in_name) == 0)
					return layer.get();
			return nullptr;
		}

		LayerBase const * Map::FindLayer(char const* in_name) const
		{
			for (auto& layer : layers)
				if (StringTools::Stricmp(layer->GetName(), in_name) == 0)
					return layer.get();
			return nullptr;
		}

#define CHAOS_IMPL_FIND_TILE_INFO(func_name, sub_funcname, arg_type)\
		TileInfo Map::func_name(arg_type arg_name) const\
		{\
			size_t count = tilesets.size();\
			for (size_t i = 0 ; i < count; ++i)\
			{\
				TileSetData const & data = tilesets[i];\
				if (data.tileset != nullptr)\
				{\
					TileData const * tiledata = data.tileset->sub_funcname(arg_name);\
					if (tiledata != nullptr)\
						return TileInfo(-1, data.tileset.get(), tiledata);\
				}\
			}\
			return TileInfo();\
		}
		CHAOS_IMPL_FIND_TILE_INFO(FindTileInfo, FindTileData, char const*);
		CHAOS_IMPL_FIND_TILE_INFO(FindTileInfoFromAtlasKey, FindTileDataFromAtlasKey, char const*);
#undef CHAOS_IMPL_FIND_TILE_INFO

	};  // namespace TiledMap

}; // namespace chaos
