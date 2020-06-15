#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>
#include <chaos/GeometryFramework.h>

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
(GroundData) \
(TileInfo) \
(TileData) \
(LayerBase) \
(ImageLayer) \
(ObjectLayer) \
(TileLayer) \
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


		// TiledMap coordinate system
		//
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

			/** utility function to load a layer */
			template<typename T, typename ...PARAMS>
			static bool DoLoadObjectListHelper(tinyxml2::XMLElement const * element, std::vector<shared_ptr<T>> & result, char const * element_name, char const * container_name, PARAMS...params)
			{
				if (container_name != nullptr) // is there an intermediate node to contain all objects
				{
					element = element->FirstChildElement(container_name);
					if (element == nullptr)
						return true;
				}

				tinyxml2::XMLElement const * e = element->FirstChildElement(element_name);
				for (; e != nullptr; e = e->NextSiblingElement(element_name))
					DoLoadObjectAndInserInList(e, result, params...);
				return true;
			}

			template<typename T, typename ...PARAMS>
			static T * DoLoadObjectAndInserInList(tinyxml2::XMLElement const * element, std::vector<shared_ptr<T>> & result_vector, PARAMS...params)
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
			STRING = 4
		};

		class Property : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using BaseObject::BaseObject;

			/** returns a pointer on the int property */
			virtual int * GetIntProperty() { return nullptr; }
			virtual int const * GetIntProperty() const { return nullptr; }
			/** returns a pointer on the float property */
			virtual float * GetFloatProperty() { return nullptr; }
			virtual float const * GetFloatProperty() const { return nullptr; }
			/** returns a pointer on the bool property */
			virtual bool * GetBoolProperty() { return nullptr; }
			virtual bool const * GetBoolProperty() const { return nullptr; }
			/** returns a pointer on the string property */
			virtual std::string * GetStringProperty() { return nullptr; }
			virtual std::string const * GetStringProperty() const { return nullptr; }

			/** returns whether the property is of type int */
			bool IsIntProperty() const { return GetIntProperty() != nullptr; }
			/** returns whether the property is of type float */
			bool IsFloatProperty() const { return GetFloatProperty() != nullptr; }
			/** returns whether the property is of type bool */
			bool IsBoolProperty() const { return GetBoolProperty() != nullptr; }
			/** returns whether the property is of type string */
			bool IsStringProperty() const { return GetStringProperty() != nullptr; }

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

			/** constructor */
			PropertyTemplate(BaseObject * in_owner) :
				Property(in_owner)
			{
				type = TYPE_ID;
			}

			/** the type of the property */
			using property_type = T;

			/** returns a pointer on the int property */
			virtual int * GetIntProperty() override { return CastPropertyTo<int>(); }
			virtual int const * GetIntProperty() const override { return CastPropertyTo<int>(); }
			/** returns a pointer on the float property */
			virtual float * GetFloatProperty() override { return CastPropertyTo<float>(); }
			virtual float const * GetFloatProperty() const override { return CastPropertyTo<float>(); }
			/** returns a pointer on the int property */
			virtual bool * GetBoolProperty() override { return CastPropertyTo<bool>(); }
			virtual bool const * GetBoolProperty() const override { return CastPropertyTo<bool>(); }
			/** returns a pointer on the int property */
			virtual std::string * GetStringProperty() override { return CastPropertyTo<std::string>(); }
			virtual std::string const * GetStringProperty() const override { return CastPropertyTo<std::string>(); }

			/** returns the value of the property */
			property_type GetValue() { return value; }

		protected:

			/** cast property to wanted type */
			template<typename U>
			U * CastPropertyTo()
			{
				if constexpr (std::is_same_v<U, property_type>)
					return &value;
				return nullptr; 
			}

			template<typename U>
			U const* CastPropertyTo() const
			{
				if constexpr (std::is_same_v<U, property_type>)
					return &value;
				return nullptr;
			}

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
			virtual Property * FindProperty(char const * name, PropertyType type_id = PropertyType::ANY);
			/** find property by name */
			virtual Property const * FindProperty(char const * name, PropertyType type_id = PropertyType::ANY) const;

			/** find property without looking elsewhere than our own table */
			Property * FindInternalProperty(char const * name, PropertyType type_id);
			/** find property without looking elsewhere than our own table */
			Property const * FindInternalProperty(char const * name, PropertyType type_id) const;

			/** find a property of type int */
			int * FindPropertyInt(char const * name);
			/** find a property of type int */
			int const * FindPropertyInt(char const * name) const;

			/** find a property of type float */
			float * FindPropertyFloat(char const * name);
			/** find a property of type float */
			float const * FindPropertyFloat(char const * name) const;

			/** find a property of type bool */
			bool * FindPropertyBool(char const * name);
			/** find a property of type bool */
			bool const * FindPropertyBool(char const * name) const;

			/** find a property of type string */
			std::string * FindPropertyString(char const * name);
			/** find a property of type string */
			std::string const * FindPropertyString(char const * name) const;

			/** find a typed property with default value */
			int GetPropertyValueInt(char const * name, int default_value) const;
			/** find a typed property with default value */
			float GetPropertyValueFloat(char const * name, float default_value) const;
			/** find a typed property with default value */
			bool GetPropertyValueBool(char const * name, bool default_value) const;
			/** find a typed property with default value */
			std::string GetPropertyValueString(char const * name, char const * default_value) const;

			/** create property */
			PropertyInt* InsertProperty(char const* name, int value);
			/** create property */
			PropertyFloat* InsertProperty(char const* name, float value);
			/** create property */
			PropertyBool* InsertProperty(char const* name, bool value);
			/** create property */
			PropertyString* InsertProperty(char const* name, char const* value);

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
		// PropertyOwnerOverride : an utility class to capture the properties of a source 
		// ==========================================

		template<typename T>
		class PropertyOwnerOverride : public T
		{
		public:

			/** constructor */
			PropertyOwnerOverride(BaseObject* in_owner, PropertyOwner* in_property_owner) :
				T(in_owner),
				property_owner(in_property_owner) {}

			/** override */
			virtual Property* FindProperty(char const* name, PropertyType type_id) override
			{
				Property* result = nullptr;
				if (property_owner != nullptr)
					result = property_owner->FindProperty(name, type_id);
				if (result == nullptr)
					result = T::FindProperty(name, type_id);
				return result;
			}
			/** override */
			virtual Property const* FindProperty(char const* name, PropertyType type_id) const override
			{
				Property const* result = nullptr;
				if (property_owner != nullptr)
					result = property_owner->FindProperty(name, type_id);
				if (result == nullptr)
					result = T::FindProperty(name, type_id);
				return result;
			}

		protected:

			/** a substitute property owner to fake the system */
			PropertyOwner* property_owner = nullptr;
		};

		// ==========================================
		// GeometricObject
		// ==========================================

		class TypedObject : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** override */
			virtual Property* FindProperty(char const* name, PropertyType type_id) override;
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
			/** object information */
			float rotation = 0.0f; // clockwise rotation in degree
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
			virtual Property * FindProperty(char const * name, PropertyType type_id) override;
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
			/** object information */
			bool horizontal_flip = false;
			/** object information */
			bool vertical_flip = false;
			/** object information */
			bool diagonal_flip = false;
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
			int tile_index = 0;
			/** object information */
			std::string name;
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

			/** get the layer ID (used for Checkpoints) */
			int GetLayerID() const { return id; }

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
			/** the zorder of the layer in the stack */
			int zorder = 0;
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
		// TileLayer
		// ==========================================

		class TileLayer : public LayerBase
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the bounding box of a tile */
			box2 GetTileBoundingBox(glm::ivec2 const tile_coord, glm::vec2 const & image_size, bool world_system) const;

			/** get the number of tiles with something inside */
			size_t GetNonEmptyTileCount() const;
			/** get the position of the tile */
			glm::ivec2 GetTileCoordinate(size_t index) const;

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
			/** loading buffer method */
			void DoLoadTileBufferFromBase64(Buffer<char> const & buffer);

		public:

			/** object information */
			glm::ivec2 size = glm::ivec2(0, 0);
			/** the tiles */
			std::vector<int> tile_indices;
			/** cache the tile size for better performance (see TileMap) */
			glm::ivec2 tile_size = glm::ivec2(0, 0);
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
			Property * FindObjectProperty(char const * type, char const * name, PropertyType type_id = PropertyType::ANY);
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

			/** get the name of the expected markup */
			virtual char const * GetXMLMarkupName() const override { return "tileset"; }

		public:

			/** find TileData from its id */
			TileData * FindTileData(int gid);
			/** find TileData from its id */
			TileData const * FindTileData(int gid) const;

			/** find TileData from its type */
			TileData * FindTileData(char const * type);
			/** find TileData from its type */
			TileData const * FindTileData(char const * type) const;

			/** find TileData from its atlas key */
			TileData * FindTileDataFromAtlasKey(char const * atlas_key);
			/** find TileData from its atlas key */
			TileData const * FindTileDataFromAtlasKey(char const * atlas_key) const;

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
			glm::ivec2        image_size = glm::vec2(0, 0);
			/** object information */
			int               image_margin = 0;
			/** object information */
			int               image_spacing = 0;
			/** object information */
			glm::vec4         transparent_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			/** the data for the tiles */
			std::vector<shared_ptr<TileData>> tiles;
			/** the data for the tiles */
			std::vector<shared_ptr<GroundData>> grounds;
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
			TileInfo(int in_gid, TileSet * in_tileset, TileData * in_tiledata):
				gid(in_gid), tileset(in_tileset), tiledata(in_tiledata){}

			/** the final gid of the search tile */
			int gid = 0;
			/** the tileset considered */
			TileSet * tileset = nullptr;
			/** the tiledata in the tileset considered */
			TileData * tiledata = nullptr;
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
			/** fix the layer order (JSON and TileMapEditor give the layers in reverse order */
			bool DoFixLayersZOrder();

		public:

			/** find tileset data for a given gid */
			TileInfo FindTileInfo(int gid);
			/** find tileset data for a given gid */
			TileInfo const FindTileInfo(int gid) const;

			/** find tileset data from its type */
			TileInfo FindTileInfo(char const * type);
			/** find tileset data from its type */
			TileInfo const FindTileInfo(char const * type) const;

			/** find tileset data from its atlas key */
			TileInfo FindTileInfoFromAtlasKey(char const * atlas_key);
			/** find tileset data from its atlas key */
			TileInfo const  FindTileInfoFromAtlasKey(char const * atlas_key) const;


			/** find a layer by its name */
			LayerBase * FindLayerByName(char const * in_name);
			/** find a layer by its name */
			LayerBase const * FindLayerByName(char const * in_name) const;
			/** find a layer by its zorder */
			LayerBase * FindLayerByZOrder(size_t zorder);
			/** find a layer by its zorder */
			LayerBase const * FindLayerByZOrder(size_t zorder) const;

			/** returns the number of layers */
			int GetLayerCount() const;

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
			/** the layers composing the map */
			std::vector<shared_ptr<ImageLayer>> image_layers;
			/** the layers composing the map */
			std::vector<shared_ptr<TileLayer>> tile_layers;
			/** the layers composing the map */
			std::vector<shared_ptr<ObjectLayer>> object_layers;
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
			virtual Property * FindObjectProperty(char const * type, char const * name, PropertyType type_id = PropertyType::ANY);
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
