namespace chaos
{
	namespace TiledMap
	{
#ifdef CHAOS_FORWARD_DECLARATION

		enum class HorizontalTextAlignment;
		enum class VerticalTextAlignment;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// GeometricObject
		// ==========================================

		class GeometricObject : public TypedObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the object ID (used for Checkpoints) */
			int GetObjectID() const { return id; }

			/** get the bounding box */
			box2 GetBoundingBox(bool world_system) const;

		protected:

			/** constructor */
			using TypedObject::TypedObject;

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

			/** get the local bounding box */
			virtual box2 DoGetBoundingBox() const;

		public:

			/** object information */
			int id = 0;
			/** object information */
			std::string name;
			/** object information */
			bool visible = true;
			/** object information */
			glm::vec2 position = glm::vec2(0.0f, 0.0f); // XXX : bottomleft, but due to inverted axis Y, this is the point the greatest Y

			// XXX : the position is :
			//         - Ellipse, Rectangle, Text : TOP-LEFT corner (this is the rotation pivot)
			//         - Polyline, Polygon : Position of the very first point (this is the rotation pivot)
			//         - Tile : it depends on the object_alignment of the tileset (BOTTOM-LEFT by default)

			/** object information */
			float rotation = 0.0f;
		};

		// ==========================================
		// GeometricObjectPoint
		// ==========================================

		class GeometricObjectPoint : public GeometricObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObject::GeometricObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
		};

		// ==========================================
		// GeometricObjectSurface
		// ==========================================

		class GeometricObjectSurface : public GeometricObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObject::GeometricObject;

			/** override */
			virtual box2 DoGetBoundingBox() const override;
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			glm::vec2 size = glm::vec2(0.0f, 0.0f);
		};

		// ==========================================
		// GeometricObjectRectangle
		// ==========================================

		class GeometricObjectRectangle : public GeometricObjectSurface
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObjectSurface::GeometricObjectSurface;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
		};

		// ==========================================
		// GeometricObjectEllipse
		// ==========================================

		class GeometricObjectEllipse : public GeometricObjectSurface
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObjectSurface::GeometricObjectSurface;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
		};

		// ==========================================
		// GeometricObjectPolygon
		// ==========================================

		class GeometricObjectPolygon : public GeometricObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObject::GeometricObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** override */
			virtual box2 DoGetBoundingBox() const override;

		public:

			/** object information */
			std::vector<glm::vec2> points;
			/** object information */
			glm::vec2 size = glm::vec2(0.0f, 0.0f);
		};

		// ==========================================
		// GeometricObjectPolyline
		// ==========================================

		class GeometricObjectPolyline : public GeometricObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObject::GeometricObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;
			/** override */
			virtual box2 DoGetBoundingBox() const override;

		public:

			/** object information */
			std::vector<glm::vec2> points;
			/** object information */
			glm::vec2 size = glm::vec2(0.0f, 0.0f);
		};

		// ==========================================
		// GeometricObjectText
		// ==========================================

		enum class HorizontalTextAlignment : int
		{
			LEFT = 0,
			CENTER = 1,
			RIGHT = 2,
			JUSTIFY = 3
		};

		CHAOS_DECLARE_ENUM_METHOD(HorizontalTextAlignment);

		enum class VerticalTextAlignment : int
		{
			TOP = 0,
			CENTER = 1,
			BOTTOM = 2
		};

		CHAOS_DECLARE_ENUM_METHOD(VerticalTextAlignment);

		// XXX : The GeometricObjectText is to be considered as a rectangle containing a text.
		//       The size of that rectangle may be greater than the size of the text
		//       The halign/valign members indicate where to place the text inside that rectangle

		class GeometricObjectText : public GeometricObjectSurface
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using GeometricObjectSurface::GeometricObjectSurface;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			HorizontalTextAlignment halign = HorizontalTextAlignment::LEFT;
			/** object information */
			VerticalTextAlignment valign = VerticalTextAlignment::TOP;
			/** object information */
			std::string fontfamily;
			/** object information */
			std::string text;
			/** object information */
			int pixelsize = 0;
			/** object information */
			int wrap = 0;
			/** object information */
			glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
		};

		// ==========================================
		// GeometricObjectTile
		// ==========================================

		class GeometricObjectTile : public GeometricObjectSurface
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** override */
			virtual Property const* FindProperty(char const* name, PropertyType type_id) const override;
			/** override */
			virtual bool IsObjectOfType(char const* in_type) const override;

			/** get the tile info for this object */
			TileInfo FindTileInfo() const;

		protected:

			/** constructor */
			using GeometricObjectSurface::GeometricObjectSurface;

			/** override */
			virtual box2 DoGetBoundingBox() const override;
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			int gid = 0;
			/** tile flags (whether particle is flipped vertical/horizontal/diagonal flipped) */
			int particle_flags = 0;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos