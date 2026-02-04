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
			Orthogonal = 0,
			Isometric = 1,
			Staggered = 2,
			Hexagonal = 3,
		};

		CHAOS_DECLARE_ENUM_METHOD(MapOrientation, CHAOS_API);

		enum class StaggerAxis : int
		{
			AxisX = 0,
			AxisY = 1
		};

		CHAOS_DECLARE_ENUM_METHOD(StaggerAxis, CHAOS_API);

		enum class StaggerIndex : int
		{
			Odd = 0,
			Even = 1
		};

		CHAOS_DECLARE_ENUM_METHOD(StaggerIndex, CHAOS_API);

		enum class RenderOrder : int
		{
			RightUp = 0,
			RightDown = 1,
			LeftUp = 2,
			LeftDown = 3
		};

		CHAOS_DECLARE_ENUM_METHOD(RenderOrder, CHAOS_API);

		// ==========================================
		// TileSetData
		// ==========================================

		class CHAOS_API TileSetData
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

		class CHAOS_API TileInfo
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

		class CHAOS_API Map : public ManagerObject
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
			MapOrientation orientation = MapOrientation::Orthogonal;
			/** object information */
			glm::ivec2 size = glm::ivec2(100, 100);
			/** object information */
			glm::ivec2 tile_size = glm::ivec2(32, 32);
			/** object information */
			bool infinite = false;
			/** object information */
			int hex_side_length = 0;
			/** object information */
			StaggerAxis stagger_axis = StaggerAxis::AxisY;
			/** object information */
			StaggerIndex stagger_index = StaggerIndex::Odd;
			/** object information */
			RenderOrder render_order = RenderOrder::RightDown;
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