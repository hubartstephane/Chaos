namespace chaos
{	
	namespace TiledMap
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// Manager : container for maps and tileset
		// ==========================================

		class Manager : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			Manager() : BaseObject(nullptr) {}

			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, bool store_object = true);
			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, Buffer<char> buffer, bool store_object = true);
			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object = true);

			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path, bool store_object = true);
			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path, Buffer<char> buffer, bool store_object = true);
			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object = true);

			/** load a object type set */
			ObjectTypeSet * LoadObjectTypeSet(FilePathParam const & path, bool store_object = true);
			/** load a object type set */
			ObjectTypeSet * LoadObjectTypeSet(FilePathParam const & path, Buffer<char> buffer, bool store_object = true);
			/** load a object type set */
			ObjectTypeSet * LoadObjectTypeSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object = true);

			/** find tiled map */
			Map * FindMap(FilePathParam const & path);
			Map const * FindMap(FilePathParam const & path) const;
			/** find tiled map set */
			TileSet * FindTileSet(FilePathParam const & path);
			TileSet const * FindTileSet(FilePathParam const & path) const;
			/** findobject type set */
			ObjectTypeSet * FindObjectTypeSet(FilePathParam const & path);
			ObjectTypeSet const * FindObjectTypeSet(FilePathParam const & path) const;

			/** find the property in an ObjectTypeSet */
			virtual Property const * FindObjectProperty(char const * type, char const * name, PropertyType type_id = PropertyType::ANY) const;

			/** returns the number of map */
			size_t GetMapCount() const { return maps.size();}
			/** returns the number of tileset */
			size_t GetTileSetCount() const { return tile_sets.size();}
			/** returns the number of 'object type set' */
			size_t GetObjectTypeSetCount() const { return object_type_sets.size();}


			/** get a map by its index */
			Map * GetMap(size_t index){ return maps[index].get();}
			/** get a map by its index */
			Map const * GetMap(size_t index) const { return maps[index].get();}

			/** get a tile set by its index */
			TileSet * GetTileSet(size_t index){ return tile_sets[index].get();}
			/** get a tile set by its index */
			TileSet const * GetTileSet(size_t index) const { return tile_sets[index].get();}

			/** get a object type set by its index */
			ObjectTypeSet * GetObjectTypeSet(size_t index){ return object_type_sets[index].get();}
			/** get a object type set by its index */
			ObjectTypeSet const * GetObjectTypeSet(size_t index) const { return object_type_sets[index].get();}

		protected:

			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, bool store_object);
			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, Buffer<char> buffer, bool store_object);
			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object);

			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path, bool store_object);
			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path, Buffer<char> buffer, bool store_object);
			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object);

			/** internal method to load a object type set (with no search for exisiting items) */
			ObjectTypeSet * DoLoadObjectTypeSet(FilePathParam const & path, bool store_object);
			/** internal method to load a object type set (with no search for exisiting items) */
			ObjectTypeSet * DoLoadObjectTypeSet(FilePathParam const & path, Buffer<char> buffer, bool store_object);
			/** internal method to load a object type set (with no search for exisiting items) */
			ObjectTypeSet * DoLoadObjectTypeSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_object);

		public:

			/** the maps */
			std::vector<shared_ptr<Map>> maps;
			/** the assets */
			std::vector<shared_ptr<TileSet>> tile_sets;
			/** the assets */
			std::vector<shared_ptr<ObjectTypeSet>> object_type_sets;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos


