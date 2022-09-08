namespace chaos
{
	namespace TiledMap
	{
#ifdef CHAOS_FORWARD_DECLARATION

		enum class TileSetOrientation;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// GroundData
		// ==========================================

		class CHAOS_API GroundData : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			int tile_id = 0;
			/** object information */
			std::string name;
		};

		// ==========================================
		// WangEdgeColor/WangCornerColor
		// ==========================================

		class CHAOS_API WangEdgeColor
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** loading method from XML */
			bool DoLoad(tinyxml2::XMLElement const* element); // XXX : not a virtual function, this is the simplest class possible

		public:

			/** object information */
			int tile_id = -1;
			/** object information */
			std::string name;
			/** object information */
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			/** object information */
			float probability = 1.0f;
		};

		using WangCornerColor = WangEdgeColor; // XXX : same data

		// ==========================================
		// WangTile
		// ==========================================

		// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tmx-wangtile
		//
		// wangid: The Wang ID, which is a 32-bit unsigned integer stored in the format 0xCECECECE (where each C is a corner color and each E is an edge color, from right to left clockwise, starting with the top edge)
		//

		class CHAOS_API WangTile
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** gets the corner value */
			int GetCornerValue(Corner corner) const;
			/** gets the edge value */
			int GetEdgeValue(Edge edge) const;

			/** the particle flags represents some geometric transformation, that should change the meaning of the Tile */
			void ApplyParticleFlags(int flags);

			/** returns whether this object is correctly initialized */
			bool IsValid() const { return (tile_id >= 0); }

		protected:

			/** loading method from XML */
			bool DoLoad(tinyxml2::XMLElement const* element); // XXX : not a virtual function, this is the simplest class possible

		public:

			/** object information */
			int tile_id = -1;
			/** object information */
			unsigned int wang_id = 0;
		};

		// ==========================================
		// Wangset
		// ==========================================

		class CHAOS_API Wangset : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** search wang information for a given tile */
			WangTile GetWangTile(int tile_id) const;

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			int tile_id = -1;
			/** object information */
			std::string name;
			/** object information */
			std::vector<WangEdgeColor> wang_edge_colors;
			/** object information */
			std::vector<WangCornerColor> wang_corner_colors;
			/** object information */
			std::vector<WangTile> wang_tiles;
		};

		// ==========================================
		// TileData
		// ==========================================

		class CHAOS_API TileData : public TypedObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using TypedObject::TypedObject;

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

			/** initialize terrain indices from string */
			bool ComputeTerrainIndices(char const* str);

		public:

			/** object information */
			int id = 0;
			/** object information */
			float probability = 1.0f;

			/** terrain index */
			int terrain_indices[4] = { -1, -1, -1, -1 };

			/** object information */
			boost::filesystem::path image_path;
			/** object information */
			glm::vec2 image_size = glm::vec2(0.0f, 0.0f);

			/** deduced reduced image/path (can be used as a key in atlas) */
			std::string atlas_key;

			/** the collision layer */
			std::vector<shared_ptr<ObjectLayer>> object_layers;
		};

		// ==========================================
		// TileSetOrientation
		// ==========================================

		enum class CHAOS_API TileSetOrientation : int
		{
			ORTHOGONAL = 0,
			ISOMETRIC = 1
		};

		CHAOS_DECLARE_ENUM_METHOD(TileSetOrientation);

		// ==========================================
		// TileSet
		// ==========================================

		class CHAOS_API TileSet : public ManagerObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** loading method from XML */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const* element) override;
			/** loading method from XML */
			bool DoLoadTiles(tinyxml2::XMLElement const* element);
			/** loading method from XML */
			bool DoLoadGrounds(tinyxml2::XMLElement const* element);
			/** loading method from XML */
			bool DoLoadWangsets(tinyxml2::XMLElement const* element);

			/** get the name of the expected markup */
			virtual char const* GetXMLMarkupName() const override { return "tileset"; }

		public:

			/** find TileData from its id */
			TileData const* FindTileData(int gid) const;
			/** find TileData from its type */
			TileData const* FindTileData(char const* type) const;
			/** find TileData from its atlas key */
			TileData const* FindTileDataFromAtlasKey(char const* atlas_key) const;

			/** find the wangset for this name */
			Wangset const* FindWangset(char const* name) const;

		public:

			/** object information */
			std::string name;
			/** object information */
			TileSetOrientation orientation = TileSetOrientation::ORTHOGONAL;
			/** object information */
			glm::ivec2  size = glm::ivec2(32, 32);
			/** object information */
			glm::ivec2  tile_size = glm::ivec2(0, 0);
			/** object information */
			int         columns = 0;
			/** object information */
			int         tile_count = 0;
			/** object information */
			glm::vec4   background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			/** object information */
			boost::filesystem::path image_path;
			/** object information */
			glm::ivec2	image_size = glm::vec2(0, 0);
			/** object information */
			int			image_margin = 0;
			/** object information */
			int			image_spacing = 0;
			/** object information */
			glm::vec4	transparent_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			/** object information */
			Hotpoint	object_alignment = Hotpoint::BOTTOM_LEFT;

			/** the data for the tiles */
			std::vector<shared_ptr<TileData>> tiles;
			/** the data for the tiles */
			std::vector<shared_ptr<GroundData>> grounds;
			/** the data for the tiles */
			std::vector<shared_ptr<Wangset>> wangsets;

			/** internal members to faster tile access */
			int min_tile_id = 0;
			/** internal members to faster tile access */
			int max_tile_id = 0;
		};


#endif

	}; // namespace TiledMap

}; // namespace chaos