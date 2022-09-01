namespace chaos
{
	namespace TiledMap
	{
#ifdef CHAOS_FORWARD_DECLARATION

		enum class DrawOrder;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// LayerBase
		// ==========================================

		class CHAOS_API LayerBase : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the layer ID (used for Checkpoints) */
			int GetLayerID() const { return id; }
			/** get the name of the layer */
			char const* GetName() const { return name.c_str(); }

			/** override */
			virtual Property const* FindProperty(char const* name, PropertyType type_id) const override;

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const* element);

		public:

			/** object information */
			int id = 0;
			/** the name of the layer */
			std::string name;
			/** whether the layer is visible */
			bool visible = true;
			/** whether the layer is locked */
			bool locked = false;
			/** the opacity */
			float opacity = 1.0f;
			/** the offset of the layer */
			glm::vec2 offset = { 0.0f, 0.0f };
			/** the parallax value for the layer */
			glm::vec2 parallax_factor = { 1.0f, 1.0f };

		};

		// ==========================================
		// ImageLayer
		// ==========================================

		class CHAOS_API ImageLayer : public LayerBase
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using LayerBase::LayerBase;

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			boost::filesystem::path image_path;
			/** object information */
			glm::ivec2 size = glm::ivec2(0, 0);
			/** object information */
			glm::vec4 transparent_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		};

		// ==========================================
		// ObjectLayer
		// ==========================================

		enum class CHAOS_API DrawOrder : int
		{
			MANUAL = 0,
			TOPDOWN = 1
		};

		CHAOS_DECLARE_ENUM_METHOD(DrawOrder);

		class CHAOS_API ObjectLayer : public LayerBase
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using LayerBase::LayerBase;

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** the loading method */
			bool DoLoadObjects(tinyxml2::XMLElement const* element);
			/** the loading method */
			GeometricObject* DoLoadOneObject(tinyxml2::XMLElement const* element);

		public:

			/** object information */
			glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			/** object information */
			DrawOrder draw_order = DrawOrder::MANUAL;

			/** the properties of the object */
			std::vector<shared_ptr<GeometricObject>> geometric_objects;
		};

		// ==========================================
		// Tile
		// ==========================================

		class CHAOS_API Tile
		{
		public:

			/** the id of the tile */
			int gid = 0;
			/** some flags encoded inside */
			int flags = 0;
		};

		// ==========================================
		// TileLayerChunk
		// ==========================================

		class CHAOS_API TileLayerChunk
		{
		public:

			/** get the tile at given position */
			Tile GetTile(glm::ivec2 pos) const;
			/** returns true whether wanted tile is in this chunk */
			bool ContainTile(glm::ivec2 const& pos) const;

		public:

			/** the dimension of the chunk */
			glm::ivec2 size = glm::ivec2(0, 0);
			/** the offset of the chunk */
			glm::ivec2 offset = glm::ivec2(0, 0);
			/** the indices for this chunk */
			std::vector<Tile> tile_indices;
		};

		// ==========================================
		// TileLayer
		// ==========================================

		class CHAOS_API TileLayer : public LayerBase
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the bounding box of a tile */
			box2 GetTileBoundingBox(glm::ivec2 const tile_coord, glm::vec2 const& image_size, int particle_flags, bool world_system) const;

			/** get the position of the tile */
			glm::ivec2 GetTileCoordinate(TileLayerChunk const& chunk, size_t index) const;

			/** get information concerning the tile at given position */
			Tile GetTile(glm::ivec2 const& position) const;
			/** get the chunk for a given tile */
			TileLayerChunk const* GetTileChunk(glm::ivec2 const& pos) const;

		protected:

			/** constructor */
			TileLayer(BaseObject* in_owner, glm::ivec2 const& in_tile_size) :
				LayerBase(in_owner),
				tile_size(in_tile_size)
			{}

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** the loading method */
			bool DoLoadTileBuffer(tinyxml2::XMLElement const* element);
			/** load all chunks of tiles */
			bool DoLoadTileChunk(tinyxml2::XMLElement const* element, char const* encoding, char const* compression);
			/** loading buffer method */
			std::vector<Tile> DoLoadTileChunkFromBuffer(Buffer<char> const& buffer, glm::ivec2 const& chunk_size);
			/** add some flags to tiles */
			virtual void ComputeTileFlags();

		public:

			/** object information */
			glm::ivec2 size = glm::ivec2(0, 0);
			/** the chunk of tiles */
			std::vector<TileLayerChunk> tile_chunks;
			/** cache the tile size for better performance (see TileMap) */
			glm::ivec2 tile_size = glm::ivec2(0, 0);
		};

		// ==========================================
		// GroupLayer
		// ==========================================

		class CHAOS_API GroupLayer : public LayerBase
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using LayerBase::LayerBase;
			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** the map layers */
			std::vector<shared_ptr<LayerBase>> layers;
		};


#endif

	}; // namespace TiledMap

}; // namespace chaos