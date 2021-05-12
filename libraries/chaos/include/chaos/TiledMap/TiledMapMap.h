namespace chaos
{
	namespace TiledMap
	{
#ifdef CHAOS_FORWARD_DECLARATION

		enum class MapOrientation;
		enum class StaggerAxis;
		enum class StaggerIndex;
		enum class RenderOrder;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// Map
		// ==========================================

		enum class MapOrientation : int
		{
			ORTHOGONAL = 0,
			ISOMETRIC = 1,
			STAGGERED = 2,
			HEXAGONAL = 3,
		};

		enum class StaggerAxis : int
		{
			AXIS_X = 0,
			AXIS_Y = 1
		};

		enum class StaggerIndex : int
		{
			ODD = 0,
			EVEN = 1
		};

		enum class RenderOrder : int
		{
			RIGHT_UP = 0,
			RIGHT_DOWN = 1,
			LEFT_UP = 2,
			LEFT_DOWN = 3
		};

		// ==========================================
		// TileSetData
		// ==========================================

		class TileSetData
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** the first gid for the tileset */
			int first_gid = 1;
			/** member to faster tile access */
			int min_tile_id = 0;
			/** member to faster tile access */
			int max_tile_id = 0;

			/** the tileset */
			shared_ptr<TileSet> tileset;
		};

		// ==========================================
		// TileInfo
		// ==========================================

		class TileInfo
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** default constructor */
			TileInfo() = default;
			/** other constuctor */
			TileInfo(int in_id, TileSet const* in_tileset, TileData const* in_tiledata) :
				id(in_id), tileset(in_tileset), tiledata(in_tiledata) {}

			/** returns whether the information is valid or not */
			bool IsValid() const { return (tileset != nullptr && tiledata != nullptr); }

		public:

			/** the final gid of the search tile */
			int id = 0;
			/** the tileset considered */
			TileSet const* tileset = nullptr;
			/** the tiledata in the tileset considered */
			TileData const* tiledata = nullptr;
		};

		// ==========================================
		// Map
		// ==========================================

		class Map : public ManagerObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** get the name of the expected markup */
			virtual char const* GetXMLMarkupName() const override { return "map"; }

			/** load internal data */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const* element) override;
			/** load the tile sets */
			bool DoLoadTileSet(tinyxml2::XMLElement const* element);
			/** load all the layers */
			bool DoLoadLayers(tinyxml2::XMLElement const* element);

		public:

			/** find tileset data for a given gid */
			TileInfo FindTileInfo(int gid) const;
			/** find tileset data from its type */
			TileInfo FindTileInfo(char const* type) const;
			/** find tileset data from its atlas key */
			TileInfo FindTileInfoFromAtlasKey(char const* atlas_key) const;

			/** find a layer by its name */
			LayerBase* FindLayer(char const* in_name);
			/** find a layer by its name */
			LayerBase const* FindLayer(char const* in_name) const;

			/** returns the number of layers */
			size_t GetLayerCount() const;

		public:

			/** object information */
			MapOrientation orientation = MapOrientation::ORTHOGONAL;
			/** object information */
			glm::ivec2 size = glm::ivec2(100, 100);
			/** object information */
			glm::ivec2 tile_size = glm::ivec2(32, 32);
			/** object information */
			bool infinite = false;
			/** object information */
			int hex_side_length = 0;
			/** object information */
			StaggerAxis stagger_axis = StaggerAxis::AXIS_Y;
			/** object information */
			StaggerIndex stagger_index = StaggerIndex::ODD;
			/** object information */
			RenderOrder render_order = RenderOrder::RIGHT_DOWN;
			/** object information */
			int compressionlevel = 0;
			/** object information */
			glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			/** object information */
			std::string version;

			/** the tileset used */
			std::vector<TileSetData> tilesets;

			/** the map layers */
			std::vector<shared_ptr<LayerBase>> layers;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos