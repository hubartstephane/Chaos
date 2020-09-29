#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>
#include <chaos/Hotpoint.h>
#include <chaos/Edge.h>
#include <chaos/Corner.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Metaprogramming.h>
#include <chaos/JSONSerializable.h>

namespace chaos
{
	namespace TiledMap
	{

		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define CHAOS_TILEDMAP_CLASSES \
(BaseObject) \
(Property) \
(PropertyOwner) \
(TypedObject) \
(GeometricObject) \
(GeometricObjectPoint) \
(GeometricObjectSurface) \
(GeometricObjectRectangle) \
(GeometricObjectEllipse) \
(GeometricObjectPolygon) \
(GeometricObjectPolyline) \
(GeometricObjectText) \
(GeometricObjectTile) \
(Wangset) \
(WangEdgeColor) \
(WangTile) \
(WangEdgeColor) \
(GroundData) \
(TileInfo) \
(TileData) \
(TileFlagProcessor) \
(ComputeNeighbourFlagProcessor) \
(LayerBase) \
(ImageLayer) \
(ObjectLayer) \
(TileLayer) \
(GroupLayer) \
(ManagerObject) \
(ObjectTypeDefinition) \
(ObjectTypeSet) \
(TileSet) \
(Map) \
(Manager)

		// forward declaration
#define CHAOS_TILEDMAP_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(CHAOS_TILEDMAP_FORWARD_DECL, _, CHAOS_TILEDMAP_CLASSES)

			// friendship macro
#define CHAOS_TILEDMAP_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_TILEDMAP_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_TILEDMAP_FRIEND_DECL, _, CHAOS_TILEDMAP_CLASSES)

		// ==========================================
		// TileParticleFlags
		// ==========================================

	// XXX : see ParticleFlags to avoid flag collisions
	namespace TileParticleFlags
	{		
		static constexpr int NEIGHBOUR_LEFT   = (1 << 5);
		static constexpr int NEIGHBOUR_RIGHT  = (1 << 6);
		static constexpr int NEIGHBOUR_TOP    = (1 << 7);
		static constexpr int NEIGHBOUR_BOTTOM = (1 << 8);
	};

		// ==========================================
		// Global
		// ==========================================

		inline glm::vec2 const REVERSE_Y_AXIS = glm::vec2(1.0f, -1.0f);

		// ==========================================
		// Functions
		// ==========================================

		/** split the tile pseudo GID into GID/H-FLIP/V-FLIP */
		int DecodeTileGID(int pseudo_gid, int* particle_flags = nullptr);

		// ==========================================
		// TiledMap coordinate system
		// ==========================================
		//
		//        OBJECT
		//  +-----------------+
		//  |                 |
		//  |                 |
		// 0,0                |
		//  +----+-------------------------------->
		//  |tile|            |
		//  | 0  |            |
		//  o----+------------+
		//  | \ 
		//  |  \ 
		//  |   +-> on object on tile (0, 0) has its bottom left corner here
		//  |       (beware, this point is were Y is max)
		//  |
		//  v
		//

		class BaseObject : public Object
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the path */
			boost::filesystem::path GetOwnerPath() const;

			/** get a parent in the hierarchy by class */
			template<typename T> 
			T * GetOwner(bool recursive = true)
			{
				BaseObject * object = this;
				T * result = auto_cast(object);
				while (result == nullptr && object->owner != nullptr && recursive)
				{
					object = object->owner;
					result = auto_cast(object);
				}
				return result;
			}

			/** get a parent in the hierarchy by class */
			template<typename T>
			T const * GetOwner(bool recursive = true) const
			{
				BaseObject const * object = this;
				T const * result = auto_cast(object);
				while (result == nullptr && object->owner != nullptr && recursive)
				{
					object = object->owner;
					result = auto_cast(object);
				}
				return result;
			}

			/** get the manager */
			Manager * GetManager();
			/** get the manager */
			Manager const * GetManager() const;
			/** get the map */
			Map * GetMap();
			/** get the map */
			Map const * GetMap() const;

		protected:

			/** constructor */
			BaseObject(BaseObject * in_owner);

			/** utility function to load objects */
			template<typename T, typename ...PARAMS>
			static bool DoLoadObjectListHelper(tinyxml2::XMLElement const * element, std::vector<T> & result, char const * element_name, char const * container_name, PARAMS...params)
			{
				if (container_name != nullptr) // is there an intermediate node to contain all objects
				{
					element = element->FirstChildElement(container_name);
					if (element == nullptr)
						return true;
				}

				tinyxml2::XMLElement const * e = element->FirstChildElement(element_name);
				for (; e != nullptr; e = e->NextSiblingElement(element_name))
				{
					if constexpr (is_shared_ptr_v<T>)
						DoLoadObjectAndInsertInList<std::remove_reference_t<decltype(*result[0].get())>>(e, result, params...); // decltype returns a REFERENCE !! this must be removed from function template argument
					else
						DoLoadObjectAndInsertInList<T>(e, result, params...);
				}
				return true;
			}

			template<typename T, typename ...PARAMS>
			static T * DoLoadObjectAndInsertInList(tinyxml2::XMLElement const * element, std::vector<T> & result_vector, PARAMS...params)
			{
				T result;
				if (result.DoLoad(element))
				{
					result_vector.push_back(std::move(result));
					return &result_vector.back();
				}
				return nullptr;
			}

			template<typename T, typename U, typename ...PARAMS>
			static T* DoLoadObjectAndInsertInList(tinyxml2::XMLElement const* element, std::vector<shared_ptr<U>>& result_vector, PARAMS...params)
			{
				T * result = new T(params...);
				if (result == nullptr)
					return nullptr;
				if (!result->DoLoad(element))
				{
					delete(result);
					return nullptr;
				}
				result_vector.push_back(result);
				return result;
			}

			/** utility function to load a layer */
			bool DoLoadLayersImpl(tinyxml2::XMLElement const* element, std::vector<shared_ptr<LayerBase>>& result);

		protected:

			/** owning object */
			BaseObject * owner = nullptr;
		};

		// ==========================================
		// Property : base class for some properties
		// ==========================================

			/** types of particle */
		enum class PropertyType : int
		{
			ANY = 0,
			INT = 1,
			FLOAT = 2,
			BOOL = 3,
			STRING = 4,
			COLOR = 5,
			OBJECT = 6
		};

		class Property : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using BaseObject::BaseObject;

			/** returns a pointer on the int property */
			int const* GetPropertyInt() const;
			/** returns a pointer on the float property */
			float const* GetPropertyFloat() const;
			/** returns a pointer on the bool property */
			bool const* GetPropertyBool() const;
			/** returns a pointer on the string property */
			std::string const* GetPropertyString() const;
			/** returns a pointer on the color property */
			glm::vec4 const* GetPropertyColor() const;
			/** returns a pointer on the object property */
			int const* GetPropertyObject() const;

			/** returns whether the property is of type int */
			bool IsPropertyInt() const { return (type == PropertyType::INT); }
			/** returns whether the property is of type float */
			bool IsPropertyFloat() const { return (type == PropertyType::FLOAT); }
			/** returns whether the property is of type bool */
			bool IsPropertyBool() const { return (type == PropertyType::BOOL); }
			/** returns whether the property is of type string */
			bool IsPropertyString() const { return (type == PropertyType::STRING); }
			/** returns whether the property is of type color */
			bool IsPropertyColor() const { return (type == PropertyType::COLOR); }
			/** returns whether the property is of type object */
			bool IsPropertyObject() const { return (type == PropertyType::OBJECT); }

			/** returns the name of the property */
			char const * GetName() const { return name.c_str(); }
			/** returns the property type */
			PropertyType GetPropertyType() const { return type; }

		protected:

			/** the name of the property */
			std::string name;
			/** the type of the property */
			PropertyType type = PropertyType::ANY;
		};

		// ==========================================
		// PropertyTemplate : templated specialization for properties
		// ==========================================

		template<typename T, PropertyType TYPE_ID>
		class PropertyTemplate : public Property
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** the type of the property */
			using property_type = T;

			/** constructor */
			PropertyTemplate(BaseObject * in_owner) :
				Property(in_owner)
			{
				type = TYPE_ID;
			}

			/** returns the value of the property */
			property_type GetValue() { return value; }

		public:

			/** the value of the property */
			property_type value;
		};

		// ==========================================
		// Specialization of properties
		// ==========================================

		using PropertyInt = PropertyTemplate<int, PropertyType::INT>;
		using PropertyFloat = PropertyTemplate<float, PropertyType::FLOAT>;
		using PropertyBool = PropertyTemplate<bool, PropertyType::BOOL>;
		using PropertyString = PropertyTemplate<std::string, PropertyType::STRING>;
		using PropertyColor = PropertyTemplate<glm::vec4, PropertyType::COLOR>;
		using PropertyObject = PropertyTemplate<int, PropertyType::OBJECT>;

		// ==========================================
		// PropertyOwner : some objects that have dynamic properties
		// ==========================================

		class PropertyOwner : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using BaseObject::BaseObject;

			/** find property by name */
			virtual Property const * FindProperty(char const * name, PropertyType type_id = PropertyType::ANY) const;

			/** find property without looking elsewhere than our own table */
			Property const * FindInternalProperty(char const * name, PropertyType type_id) const;

			/** find a property of type int */
			int const * FindPropertyInt(char const * name) const;
			/** find a property of type float */
			float const * FindPropertyFloat(char const * name) const;
			/** find a property of type bool */
			bool const * FindPropertyBool(char const * name) const;
			/** find a property of type string */
			std::string const * FindPropertyString(char const * name) const;
			/** find a property of type color */
			glm::vec4 const* FindPropertyColor(char const* name) const;
			/** find a property of type object */
			int const* FindPropertyObject(char const* name) const;

			/** find a typed property with default value */
			int GetPropertyValueInt(char const * name, int default_value) const;
			/** find a typed property with default value */
			float GetPropertyValueFloat(char const * name, float default_value) const;
			/** find a typed property with default value */
			bool GetPropertyValueBool(char const * name, bool default_value) const;
			/** find a typed property with default value */
			std::string GetPropertyValueString(char const * name, char const * default_value) const;
			/** find a typed property with default value */
			glm::vec4 GetPropertyValueColor(char const* name, glm::vec4 const& default_value) const;
			/** find a typed property with default value */
			int GetPropertyValueObject(char const* name, int default_value) const;

			/** create property */
			PropertyInt* CreatePropertyInt(char const* name, int value);
			/** create property */
			PropertyFloat* CreatePropertyFloat(char const* name, float value);
			/** create property */
			PropertyBool* CreatePropertyBool(char const* name, bool value);
			/** create property */
			PropertyString* CreatePropertyString(char const* name, char const* value);
			/** create property */
			PropertyColor* CreatePropertyColor(char const* name, glm::vec4 const & value);
			/** create property */
			PropertyObject* CreatePropertyObject(char const* name, int value);

			/** check whether an object is of given type */
			virtual bool IsObjectOfType(char const* in_type) const;

		protected:

			/** the method to override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element);

			/** load all properties */
			virtual bool DoLoadProperties(tinyxml2::XMLElement const * element);
			/** returns the container of properties */
			virtual tinyxml2::XMLElement const * GetPropertiesChildNode(tinyxml2::XMLElement const * element) const;

		protected:

			/** the properties of the object */
			std::vector<shared_ptr<Property>> properties;
		};

		// ==========================================
		// TypedObject
		// ==========================================

		class TypedObject : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** override */
			virtual Property const* FindProperty(char const* name, PropertyType type_id) const override;
			/** override */
			virtual bool IsObjectOfType(char const* in_type) const override;

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;
			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			std::string type;
		};

		// ==========================================
		// GeometricObject
		// ==========================================

		class GeometricObject : public TypedObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** cast method into its subtype */
			GeometricObjectPoint * GetObjectPoint() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectPoint const * GetObjectPoint() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectSurface * GetObjectSurface() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectSurface const * GetObjectSurface() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectRectangle * GetObjectRectangle() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectRectangle const * GetObjectRectangle() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectEllipse * GetObjectEllipse() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectEllipse const * GetObjectEllipse() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectPolygon * GetObjectPolygon() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectPolygon const * GetObjectPolygon() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectPolyline * GetObjectPolyline() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectPolyline const * GetObjectPolyline() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectText * GetObjectText() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectText const * GetObjectText() const { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectTile * GetObjectTile() { return auto_cast(this); }
			/** cast method into its subtype */
			GeometricObjectTile const * GetObjectTile() const { return auto_cast(this); }

			/** get the object ID (used for Checkpoints) */
			int GetObjectID() const { return id; }

		protected:

			/** constructor */
			using TypedObject::TypedObject;

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

			/** loading method from XML */
			std::vector<glm::vec2> GetPointArray(tinyxml2::XMLElement const * element, char const * attribute_name);

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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
		};

		// ==========================================
		// GeometricObjectSurface
		// ==========================================

		class GeometricObjectSurface : public GeometricObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the bounding box */
			virtual box2 GetBoundingBox(bool world_system) const;

		protected:

			/** constructor */
			using GeometricObject::GeometricObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			std::vector<glm::vec2> points;
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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			std::vector<glm::vec2> points;
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

		enum class VerticalTextAlignment : int
		{
			TOP = 0,
			CENTER = 1,
			BOTTOM = 2
		};

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
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

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
			glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		};

		// ==========================================
		// GeometricObjectTile
		// ==========================================

		class GeometricObjectTile : public GeometricObjectSurface
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** override */
			virtual box2 GetBoundingBox(bool world_system) const override;
			/** override */
			virtual Property const * FindProperty(char const * name, PropertyType type_id) const override;			
			/** override */
			virtual bool IsObjectOfType(char const* in_type) const override;

			/** get the tile info for this object */
			TileInfo FindTileInfo() const;

		protected:

			/** constructor */
			using GeometricObjectSurface::GeometricObjectSurface;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			int gid = 0;
			/** tile flags (whether particle is flipped vertical/horizontal/diagonal flipped) */
			int particle_flags = 0;
		};

		// ==========================================
		// GroundData
		// ==========================================

		class GroundData : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			int tile_id = 0;
			/** object information */
			std::string name;
		};

		// ==========================================
		// WangEdgeColor/WangCornerColor
		// ==========================================

		class WangEdgeColor
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

		class WangTile
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

		class Wangset : public PropertyOwner
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

		class TileData : public TypedObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using TypedObject::TypedObject;

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

			/** initialize terrain indices from string */
			bool ComputeTerrainIndices(char const * str);

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
		// LayerBase
		// ==========================================

		class LayerBase : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** cast method into its subtype */
			ImageLayer * GetImageLayer() { return auto_cast(this); }
			/** cast method into its subtype */
			ImageLayer const * GetImageLayer() const { return auto_cast(this); }

			/** cast method into its subtype */
			ObjectLayer * GetObjectLayer() { return auto_cast(this); }
			/** cast method into its subtype */
			ObjectLayer const * GetObjectLayer() const { return auto_cast(this); }

			/** cast method into its subtype */
			TileLayer * GetTileLayer() { return auto_cast(this); }
			/** cast method into its subtype */
			TileLayer const * GetTileLayer() const { return auto_cast(this); }

			/** cast method into its subtype */
			GroupLayer* GetGroupLayer() { return auto_cast(this); }
			/** cast method into its subtype */
			GroupLayer const* GetGroupLayer() const { return auto_cast(this); }

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
			virtual bool DoLoad(tinyxml2::XMLElement const * element);

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
			glm::vec2 offset = glm::vec2(0.0f, 0.0f);
		};

		// ==========================================
		// ImageLayer
		// ==========================================

		class ImageLayer : public LayerBase
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using LayerBase::LayerBase;

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

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

		enum class DrawOrder : int
		{
			MANUAL = 0,
			TOPDOWN = 1
		};

		class ObjectLayer : public LayerBase
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** constructor */
			using LayerBase::LayerBase;

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** the loading method */
			bool DoLoadObjects(tinyxml2::XMLElement const * element);
			/** the loading method */
			GeometricObject * DoLoadOneObject(tinyxml2::XMLElement const * element);

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

		class Tile
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

		class TileLayerChunk
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
		// TileFlagProcessor
		// ==========================================

		class TileFlagProcessor : public Object, public JSONSerializable
		{
			CHAOS_OBJECT_DECLARE_CLASS2(TileFlagProcessor, Object);

		public:

			/** the method to be apply on the layer */
			virtual void Process(TileLayer* in_layer) {}
		};

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		class ComputeNeighbourFlagProcessor : public TileFlagProcessor
		{
			CHAOS_OBJECT_DECLARE_CLASS2(ComputeNeighbourFlagProcessor, TileFlagProcessor);

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

		protected:

			/** internal method to detect whether the tile is of interrest for the processing */
			bool ShouldProcessedTiled(Map const* map, Tile tile) const;

		protected:

			/** the types that are searched for neighbours */
			std::vector<std::string> types;
		};

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		class ComputeCustomFlagProcessorEntry
		{
		public:

			/** the type of the tile */
			std::string type;
			/** the flag applyed */
			int flag = 0;
		};

		/** JSON serialization */
		bool SaveIntoJSON(nlohmann::json& json_entry, ComputeCustomFlagProcessorEntry const& src);
		/** JSON serialization */
		bool LoadFromJSON(nlohmann::json const& json_entry, ComputeCustomFlagProcessorEntry& dst);


		class ComputeCustomFlagProcessor : public TileFlagProcessor
		{
			CHAOS_OBJECT_DECLARE_CLASS2(ComputeCustomFlagProcessor, TileFlagProcessor);

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

		protected:

			/** additionnal flags per type */
			std::vector<ComputeCustomFlagProcessorEntry> custom_flags;
		};

		// ==========================================
		// TileLayer
		// ==========================================

		class TileLayer : public LayerBase
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the bounding box of a tile */
			box2 GetTileBoundingBox(glm::ivec2 const tile_coord, glm::vec2 const & image_size, int particle_flags, bool world_system) const;

			/** get the position of the tile */
			glm::ivec2 GetTileCoordinate(TileLayerChunk const& chunk, size_t index) const;

			/** get information concerning the tile at given position */
			Tile GetTile(glm::ivec2 const& position) const;
			/** get the chunk for a given tile */
			TileLayerChunk const* GetTileChunk(glm::ivec2 const& pos) const;

		protected:

			/** constructor */
			TileLayer(BaseObject * in_owner, glm::ivec2 const & in_tile_size) :
				LayerBase(in_owner),
				tile_size(in_tile_size)
				{}

			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** the loading method */
			bool DoLoadTileBuffer(tinyxml2::XMLElement const * element);
			/** load all chunks of tiles */
			bool DoLoadTileChunk(tinyxml2::XMLElement const* element, char const * encoding, char const * compression);
			/** loading buffer method */
			std::vector<Tile> DoLoadTileChunkFromBuffer(Buffer<char> const & buffer, glm::ivec2 const & chunk_size);
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

		class GroupLayer : public LayerBase
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
			   
		// ==========================================
		// ManagerObject : objects control by the manager (Map & TileSet)
		// ==========================================

		class ManagerObject : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** returns true whether the name match the resource name */
			bool IsMatchingName(boost::filesystem::path const & in_path) const;
			/** get the path */
			boost::filesystem::path const & GetPath() const { return path; }

		protected:

			/** the constructor */
			ManagerObject(BaseObject * in_owner, boost::filesystem::path in_path);

			/** loading method from XML */
			virtual bool DoLoadDocument(tinyxml2::XMLDocument const * doc);
			/** the method to override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** the method to override */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) { return true; }
			/** get the name of the expected markup */
			virtual char const * GetXMLMarkupName() const { return nullptr; }

		public:

			/** the filename */
			boost::filesystem::path path;
		};

		// ==========================================
		// ObjectTypeDefinition
		// ==========================================

		class ObjectTypeDefinition : public PropertyOwner
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using PropertyOwner::PropertyOwner;

		protected:

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual tinyxml2::XMLElement const * GetPropertiesChildNode(tinyxml2::XMLElement const * element) const override 
			{ 
				return element; // XXX: the properties are not contained by a 'properties' node
			} 

		public:

			/** object information */
			std::string name;
			/** the color in the editor */
			glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		};

		// ==========================================
		// ObjectTypeSet
		// ==========================================

		class ObjectTypeSet : public ManagerObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** find an object type */
			ObjectTypeDefinition * FindObjectType(char const * name);
			/** find an object type */
			ObjectTypeDefinition const * FindObjectType(char const * name) const;

			/** find the property in an ObjectType */
			Property const * FindObjectProperty(char const * type, char const * name, PropertyType type_id = PropertyType::ANY) const;


			/** returns the number of object type */
			size_t GetObjectTypeDefinitionCount() const { return object_types.size(); }
			/** returns the object type by its index */
			ObjectTypeDefinition * GetObjectTypeDefinition(size_t index) { return object_types[index].get(); }
			/** returns the object type by its index */
			ObjectTypeDefinition const * GetObjectTypeDefinition(size_t index) const { return object_types[index].get(); }

		protected:

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual char const * GetXMLMarkupName() const override { return "objecttypes"; }

			/** load all types */
			bool DoLoadObjectTypes(tinyxml2::XMLElement const * element);

		protected:

			/** object set information */
			std::string name;
			/** all types of object */
			std::vector<shared_ptr<ObjectTypeDefinition>> object_types;
		};

		// ==========================================
		// TileSet
		// ==========================================

		enum class TileSetOrientation : int
		{
			ORTHOGONAL = 0,
			ISOMETRIC = 1
		};

		class TileSet : public ManagerObject
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** loading method from XML */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
			/** loading method from XML */
			bool DoLoadTiles(tinyxml2::XMLElement const * element);
			/** loading method from XML */
			bool DoLoadGrounds(tinyxml2::XMLElement const * element);
			/** loading method from XML */
			bool DoLoadWangsets(tinyxml2::XMLElement const* element);

			/** get the name of the expected markup */
			virtual char const * GetXMLMarkupName() const override { return "tileset"; }

		public:

			/** find TileData from its id */
			TileData const * FindTileData(int gid) const;
			/** find TileData from its type */
			TileData const * FindTileData(char const * type) const;
			/** find TileData from its atlas key */
			TileData const * FindTileDataFromAtlasKey(char const * atlas_key) const;

			/** find the wangset for this name */
			Wangset const * FindWangset(char const* name) const;

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

		// ==========================================
		// Map : some map class
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

		class TileInfo
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** default constructor */
			TileInfo() = default;
			/** other constuctor */
			TileInfo(int in_gid, TileSet const * in_tileset, TileData const * in_tiledata):
				id(in_gid), tileset(in_tileset), tiledata(in_tiledata){}

			/** returns whether the information is valid or not */
			bool IsValid() const { return (id > 0); }

		public:

			/** the final gid of the search tile */
			int id = 0;
			/** the tileset considered */
			TileSet const * tileset = nullptr;
			/** the tiledata in the tileset considered */
			TileData const * tiledata = nullptr;
		};

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

		class Map : public ManagerObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		protected:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** get the name of the expected markup */
			virtual char const * GetXMLMarkupName() const override { return "map"; }

			/** load internal data */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
			/** load the tile sets */
			bool DoLoadTileSet(tinyxml2::XMLElement const * element);
			/** load all the layers */
			bool DoLoadLayers(tinyxml2::XMLElement const * element);

		public:

			/** find tileset data for a given gid */
			TileInfo FindTileInfo(int gid) const;
			/** find tileset data from its type */
			TileInfo FindTileInfo(char const * type) const;
			/** find tileset data from its atlas key */
			TileInfo FindTileInfoFromAtlasKey(char const * atlas_key) const;

			/** find a layer by its name */
			LayerBase * FindLayer(char const * in_name);
			/** find a layer by its name */
			LayerBase const * FindLayer(char const * in_name) const;

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

	}; // namespace TiledMap


	   // undefine macros
#undef CHAOS_TILEDMAP_CLASSES
#undef CHAOS_TILEDMAP_FORWARD_DECL
#undef CHAOS_TILEDMAP_FRIEND_DECL
#undef CHAOS_TILEDMAP_ALL_FRIENDS

}; // namespace chaos
