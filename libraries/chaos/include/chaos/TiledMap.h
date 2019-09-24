#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{
	namespace TiledMap
	{
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


		class BaseObject : public ReferencedObject
		{
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

		protected:

			/** constructor */
			BaseObject(BaseObject * in_owner) :
				owner(in_owner) {}

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

		class Property : public BaseObject
		{
			friend class PropertyOwner;

		public:

			/** types of particle */
			static int const PROPERTY_TYPEID_ANY    = 0;
			static int const PROPERTY_TYPEID_INT    = 1;
			static int const PROPERTY_TYPEID_FLOAT  = 2;
			static int const PROPERTY_TYPEID_BOOL   = 3;
			static int const PROPERTY_TYPEID_STRING = 4;

			/** constructor */
			Property(BaseObject * in_owner) :
				BaseObject(in_owner) {}

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
			int GetPropertyTypeID() const { return property_type_id; }

		protected:

			/** the name of the property */
			std::string name;
			/** the type of the property */
			int property_type_id = PROPERTY_TYPEID_ANY;
		};

		// ==========================================
		// PropertyTemplate : templated specialization for properties
		// ==========================================

		template<typename T, int TYPE_ID>
		class PropertyTemplate : public Property
		{
		public:

			/** constructor */
			PropertyTemplate(BaseObject * in_owner) :
				Property(in_owner)
			{
				property_type_id = TYPE_ID;
			}

			/** the type of the property */
			using property_type = T;

			/** returns a pointer on the int property */
			virtual int * GetIntProperty() { return CastPropertyTo(&value, boost::mpl::identity<int>()); }
			virtual int const * GetIntProperty() const { return CastPropertyTo(&value, boost::mpl::identity<int>()); }
			/** returns a pointer on the float property */
			virtual float * GetFloatProperty() { return CastPropertyTo(&value, boost::mpl::identity<float>()); }
			virtual float const * GetFloatProperty() const { return CastPropertyTo(&value, boost::mpl::identity<float>()); }
			/** returns a pointer on the int property */
			virtual bool * GetBoolProperty() { return CastPropertyTo(&value, boost::mpl::identity<bool>()); }
			virtual bool const * GetBoolProperty() const { return CastPropertyTo(&value, boost::mpl::identity<bool>()); }
			/** returns a pointer on the int property */
			virtual std::string * GetStringProperty() { return CastPropertyTo(&value, boost::mpl::identity<std::string>()); }
			virtual std::string const * GetStringProperty() const { return CastPropertyTo(&value, boost::mpl::identity<std::string>()); }

			/** returns the value of the property */
			property_type GetValue() { return value; }

		protected:

			/** default template to try casting into given class */
			template<typename U, typename V>
			static typename V::type * CastPropertyTo(U * ptr, V) { return nullptr; }
			/** specialisation if input and wanted class match */
			template<typename U>
			static typename U * CastPropertyTo(U * ptr, boost::mpl::identity<U>) { return ptr; }
			/** specialisation if input and wanted class match (const version) */
			template<typename U>
			static typename U const * CastPropertyTo(U const * ptr, boost::mpl::identity<U>) { return ptr; }

		public:

			/** the value of the property */
			property_type value;
		};

		// ==========================================
		// Specialization of properties
		// ==========================================

		using PropertyInt = PropertyTemplate<int, Property::PROPERTY_TYPEID_INT>;
		using PropertyFloat = PropertyTemplate<float, Property::PROPERTY_TYPEID_FLOAT>;
		using PropertyBool = PropertyTemplate<bool, Property::PROPERTY_TYPEID_BOOL>;
		using PropertyString = PropertyTemplate<std::string, Property::PROPERTY_TYPEID_STRING>;

		// ==========================================
		// PropertyOwner : some objects that have dynamic properties
		// ==========================================

		class PropertyOwner : public BaseObject
		{
			friend class Manager;

		public:

			/** constructor */
			PropertyOwner(BaseObject * in_owner) :
				BaseObject(in_owner) {}

			/** find property by name */
			Property * FindProperty(char const * name, int type_id = Property::PROPERTY_TYPEID_ANY);
			/** find property by name */
			Property const * FindProperty(char const * name, int type_id = Property::PROPERTY_TYPEID_ANY) const;

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
			int FindPropertyInt(char const * name, int default_value) const;
			/** find a typed property with default value */
			float FindPropertyFloat(char const * name, float default_value) const;
			/** find a typed property with default value */
			bool FindPropertyBool(char const * name, bool default_value) const;
			/** find a typed property with default value */
			std::string FindPropertyString(char const * name, char const * default_value) const;

		protected:

			/** the method to override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element);

			/** create property */
			PropertyInt * DoInsertProperty(char const * name, int value);
			/** create property */
			PropertyFloat * DoInsertProperty(char const * name, float value);
			/** create property */
			PropertyBool * DoInsertProperty(char const * name, bool value);
			/** create property */
			PropertyString * DoInsertProperty(char const * name, char const * value);

		protected:

			/** the properties of the object */
			std::vector<shared_ptr<Property>> properties;
		};

		// ==========================================
		// GeometricObject
		// ==========================================

		class GeometricObject : public PropertyOwner
		{
			friend class Manager;
			friend class BaseObject;
			friend class TileSet;
			friend class ObjectLayer;

		public:

			/** cast method into its subtype */
			virtual class GeometricObjectPoint * GetObjectPoint() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectPoint const * GetObjectPoint() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectSurface * GetObjectSurface() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectSurface const * GetObjectSurface() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectRectangle * GetObjectRectangle() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectRectangle const * GetObjectRectangle() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectEllipse * GetObjectEllipse() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectEllipse const * GetObjectEllipse() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectPolygon * GetObjectPolygon() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectPolygon const * GetObjectPolygon() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectPolyline * GetObjectPolyline() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectPolyline const * GetObjectPolyline() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectText * GetObjectText() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectText const * GetObjectText() const { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectTile * GetObjectTile() { return nullptr; }
			/** cast method into its subtype */
			virtual class GeometricObjectTile const * GetObjectTile() const { return nullptr; }

			/** get the layer ID (used for Checkpoints) */
			int GetObjectID() const { return object_id; }

		protected:

			/** constructor */
			GeometricObject(BaseObject * in_owner) :
				PropertyOwner(in_owner) {}
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** loading method from XML */
			std::vector<glm::vec2> GetPointArray(tinyxml2::XMLElement const * element, char const * attribute_name);

		public:

			/** object information */
			int id = 0;
			/** object information */
			std::string name;
			/** object information */
			std::string type;
			/** object information */
			bool visible = true;
			/** object information */
			glm::vec2 position = glm::vec2(0.0f, 0.0f); // XXX : bottomleft, but due to inverted axis Y, this is the point the greatest Y
			/** object information */
			float rotation = 0.0f; // clockwise rotation in degree

			/** an ID deducted from the order in the source JSON */
			int object_id = -1;
		};

		// ==========================================
		// GeometricObjectPoint
		// ==========================================

		class GeometricObjectPoint : public GeometricObject
		{
			friend class ObjectLayer;

		public:

			virtual GeometricObjectPoint * GetObjectPoint() override { return this; }
			virtual GeometricObjectPoint const * GetObjectPoint() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectPoint(BaseObject * in_owner) :
				GeometricObject(in_owner) {}
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
		};

		// ==========================================
		// GeometricObjectSurface
		// ==========================================

		class GeometricObjectSurface : public GeometricObject
		{
			friend class ObjectLayer;


		public:

			virtual class GeometricObjectSurface * GetObjectSurface() override { return this; }
			virtual class GeometricObjectSurface const * GetObjectSurface() const override { return this; }

			/** get the bounding box */
			virtual box2 GetBoundingBox(bool world_system) const;

		protected:

			/** constructor */
			GeometricObjectSurface(BaseObject * in_owner) :
				GeometricObject(in_owner) {}

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
			friend class ObjectLayer;

		public:

			virtual GeometricObjectRectangle * GetObjectRectangle() override { return this; }
			virtual GeometricObjectRectangle const * GetObjectRectangle() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectRectangle(BaseObject * in_owner) :
				GeometricObjectSurface(in_owner) {}

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
		};

		// ==========================================
		// GeometricObjectEllipse
		// ==========================================

		class GeometricObjectEllipse : public GeometricObjectSurface
		{
			friend class ObjectLayer;

		public:

			virtual GeometricObjectEllipse * GetObjectEllipse() override { return this; }
			virtual GeometricObjectEllipse const * GetObjectEllipse() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectEllipse(BaseObject * in_owner) :
				GeometricObjectSurface(in_owner) {}

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
		};

		// ==========================================
		// GeometricObjectPolygon
		// ==========================================

		class GeometricObjectPolygon : public GeometricObject
		{
			friend class ObjectLayer;

		public:

			virtual GeometricObjectPolygon * GetObjectPolygon() override { return this; }
			virtual GeometricObjectPolygon const * GetObjectPolygon() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectPolygon(BaseObject * in_owner) :
				GeometricObject(in_owner) {}

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
			friend class ObjectLayer;

		public:

			virtual GeometricObjectPolyline * GetObjectPolyline() override { return this; }
			virtual GeometricObjectPolyline const * GetObjectPolyline() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectPolyline(BaseObject * in_owner) :
				GeometricObject(in_owner) {}
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			std::vector<glm::vec2> points;
		};

		// ==========================================
		// GeometricObjectText
		// ==========================================

		class GeometricObjectText : public GeometricObjectSurface
		{
			friend class ObjectLayer;

		public:

			static int const HALIGN_LEFT = 0;
			static int const HALIGN_CENTER = 1;
			static int const HALIGN_RIGHT = 2;
			static int const HALIGN_JUSTIFY = 3;

			static int const VALIGN_TOP = 0;
			static int const VALIGN_CENTER = 1;
			static int const VALIGN_BOTTOM = 2;

		public:

			virtual GeometricObjectText * GetObjectText() override { return this; }
			virtual GeometricObjectText const * GetObjectText() const override { return this; }

		protected:

			/** constructor */
			GeometricObjectText(BaseObject * in_owner) :
				GeometricObjectSurface(in_owner) {}
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			int halign = HALIGN_LEFT;
			/** object information */
			int valign = VALIGN_TOP;
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
			friend class ObjectLayer;

		public:

			virtual GeometricObjectTile * GetObjectTile() override { return this; }
			virtual GeometricObjectTile const * GetObjectTile() const override { return this; }

			virtual box2 GetBoundingBox(bool world_system) const override;

		protected:

			/** constructor */
			GeometricObjectTile(BaseObject * in_owner) :
				GeometricObjectSurface(in_owner) {}

			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

		public:

			/** object information */
			int gid = 0;
			/** object information */
			bool horizontal_flip = false;
			/** object information */
			bool vertical_flip = false;
		};

		// ==========================================
		// GroundData
		// ==========================================

		class GroundData : public PropertyOwner
		{
			friend class Manager;
			friend class BaseObject;
			friend class TileSet;

		protected:

			/** constructor */
			GroundData(BaseObject * in_owner) :
				PropertyOwner(in_owner) {}
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

		class TileData : public PropertyOwner
		{
			friend class Manager;
			friend class BaseObject;
			friend class TileSet;

		protected:

			/** constructor */
			TileData(BaseObject * in_owner) :
				PropertyOwner(in_owner) {}
			/** loading method from XML */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** initialize terrain indices from string */
			bool ComputeTerrainIndices(char const * str);

		public:

			/** object information */
			int id = 0;
			/** object information */
			std::string type;
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
			friend class Map;

		public:

			/** cast method into its subtype */
			virtual class ImageLayer * GetImageLayer() { return nullptr; }
			/** cast method into its subtype */
			virtual class ImageLayer const * GetImageLayer() const { return nullptr; }

			/** cast method into its subtype */
			virtual class ObjectLayer * GetObjectLayer() { return nullptr; }
			/** cast method into its subtype */
			virtual class ObjectLayer const * GetObjectLayer() const { return nullptr; }

			/** cast method into its subtype */
			virtual class TileLayer * GetTileLayer() { return nullptr; }
			/** cast method into its subtype */
			virtual class TileLayer const * GetTileLayer() const { return nullptr; }

			/** get the layer ID (used for Checkpoints) */
			int GetObjectID() const { return object_id; }

		protected:

			/** constructor */
			LayerBase(BaseObject * in_owner) :
				PropertyOwner(in_owner) {}
			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const * element);

		public:

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

			/** an ID deducted from the order in the source JSON */
			int object_id = -1;
		};

		// ==========================================
		// ImageLayer
		// ==========================================

		class ImageLayer : public LayerBase
		{
			friend class Map;
			friend class BaseObject;

		protected:

			/** constructor */
			ImageLayer(BaseObject * in_owner) :
				LayerBase(in_owner) {}
			/** the loading method */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

			/** override */
			virtual class ImageLayer * GetImageLayer() override { return this; }
			/** override */
			virtual class ImageLayer const * GetImageLayer() const override { return this; }

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

		class ObjectLayer : public LayerBase
		{
			friend class Map;
			friend class BaseObject;

			static int const DRAW_ORDER_MANUAL = 0;
			static int const DRAW_ORDER_TOPDOWN = 1;

		public:

			/** override */
			virtual class ObjectLayer * GetObjectLayer() override { return this; }
			/** override */
			virtual class ObjectLayer const * GetObjectLayer() const override { return this; }

		protected:

			/** constructor */
			ObjectLayer(BaseObject * in_owner) :
				LayerBase(in_owner) {}

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
			int       draw_order = DRAW_ORDER_MANUAL;

			/** the properties of the object */
			std::vector<shared_ptr<GeometricObject>> geometric_objects;
		};

		// ==========================================
		// TileLayer
		// ==========================================

		class TileLayer : public LayerBase
		{

			friend class Map;
			friend class BaseObject;

		public:

			/** get the bounding box of a tile */
			box2 GetTileBoundingBox(glm::ivec2 const tile_coord, glm::vec2 const & image_size, bool world_system) const;

			/** get the number of tiles with something inside */
			size_t GetNonEmptyTileCount() const;
			/** get the position of the tile */
			glm::ivec2 GetTileCoordinate(int index) const;

			/** override */
			virtual class TileLayer * GetTileLayer() override { return this; }
			/** override */
			virtual class TileLayer const * GetTileLayer() const override { return this; }

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
			friend class Manager;

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
		// TileSet
		// ==========================================

		class TileSet : public ManagerObject
		{
			static int const ORIENTATION_ORTHOGONAL = 0;
			static int const ORIENTATION_ISOMETRIC = 1;

			friend class Manager;
			friend class Map;

		protected:

			/** the constructor */
			TileSet(BaseObject * in_owner, boost::filesystem::path in_path);
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
			int         orientation = ORIENTATION_ORTHOGONAL;
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
		public:

			/** the first gid for the tileset */
			int first_gid = 1;
			/** the tileset */
			shared_ptr<TileSet> tileset;
		};

		class TileInfo
		{
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

		class Map : public ManagerObject
		{
			friend class Manager;
			friend class TileSet;

			static int const ORIENTATION_ORTHOGONAL = 0;
			static int const ORIENTATION_ISOMETRIC = 1;
			static int const ORIENTATION_STAGGERED = 2;
			static int const ORIENTATION_HEXAGONAL = 3;

			static int const STAGGERED_AXIS_X = 0;
			static int const STAGGERED_AXIS_Y = 1;

			static int const STAGGERED_INDEX_ODD = 0;
			static int const STAGGERED_INDEX_EVEN = 1;

			static int const RENDER_ORDER_RIGHT_UP = 0;
			static int const RENDER_ORDER_RIGHT_DOWN = 1;
			static int const RENDER_ORDER_LEFT_UP = 2;
			static int const RENDER_ORDER_LEFT_DOWN = 3;

		protected:

			/** the constructor */
			Map(BaseObject * in_owner, boost::filesystem::path in_path);
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
			LayerBase * FindLayerByZOrder(int zorder);
			/** find a layer by its zorder */
			LayerBase const * FindLayerByZOrder(int zorder) const;

			/** returns the number of layers */
			int GetLayerCount() const;

		public:

			/** object information */
			int orientation = ORIENTATION_ORTHOGONAL;
			/** object information */
			glm::ivec2 size = glm::ivec2(100, 100);
			/** object information */
			glm::ivec2 tile_size = glm::ivec2(32, 32);
			/** object information */
			bool infinite = false;
			/** object information */
			int hex_side_length = 0;
			/** object information */
			int stagger_axis = STAGGERED_AXIS_Y;
			/** object information */
			int stagger_index = STAGGERED_INDEX_ODD;
			/** object information */
			int render_order = RENDER_ORDER_RIGHT_DOWN;
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
			friend class Map;
			friend class TileSet;

		public:

			/** constructor */
			Manager() : BaseObject(nullptr) {}

			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, bool store_map = true);
			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, Buffer<char> buffer, bool store_map = true);
			/** load a tiled map set */
			Map * LoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_map = true);

			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path);
			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path, Buffer<char> buffer);
			/** load a tiled map */
			TileSet * LoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc);

			/** find tiled map */
			Map * FindMap(FilePathParam const & path);
			Map const * FindMap(FilePathParam const & path) const;
			/** find tiled map set */
			TileSet * FindTileSet(FilePathParam const & path);
			TileSet const * FindTileSet(FilePathParam const & path) const;

		protected:

			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, bool store_map);
			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, Buffer<char> buffer, bool store_map);
			/** internal method to load a tiled map set (with no search for exisiting items) */
			Map * DoLoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc, bool store_map);

			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path);
			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path, Buffer<char> buffer);
			/** internal method to load a tiled map (with no search for exisiting items) */
			TileSet * DoLoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc);

		public:

			/** the maps */
			std::vector<shared_ptr<Map>> maps;
			/** the assets */
			std::vector<shared_ptr<TileSet>> tile_sets;
		};

	}; // namespace TiledMap

}; // namespace chaos
