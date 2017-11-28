#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Buffer.h>

namespace chaos
{
  namespace TiledMap
  {
    //
    // Property : base class for some properties
    //

    class Property : public ReferencedObject
    {
      friend class PropertyOwner;

    public:

      /** returns whether the property is of type int */
      bool IsIntProperty() const { return GetIntProperty() != nullptr; }
      /** returns whether the property is of type float */
      bool IsFloatProperty() const { return GetFloatProperty() != nullptr; }
      /** returns whether the property is of type bool */
      bool IsBoolProperty() const { return GetBoolProperty() != nullptr; }
      /** returns whether the property is of type string */
      bool IsStringProperty() const { return GetStringProperty() != nullptr; }

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

      /** returns the name of the property */
      char const * GetName() const { return name.c_str(); }

    protected:

      /** the name of the property */
      std::string name;
    };

    //
    // PropertyTemplate : templated specialization for properties
    //

    template<typename T>
    class PropertyTemplate : public Property
    {
    public:

      /** the type of the property */
      typedef T property_type;

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
      T value;
    };

    //
    // Specialization of properties
    //

    using PropertyInt = PropertyTemplate<int>;
    using PropertyFloat = PropertyTemplate<float>;
    using PropertyBool = PropertyTemplate<bool>;
    using PropertyString = PropertyTemplate<std::string>;

    //
    // PropertyOwner : some objects that have dynamic properties
    //

    class PropertyOwner : public ReferencedObject
    {
      friend class Manager;

    public:

      /** find property by name */
      Property * FindProperty(char const * name);
      /** find property by name */
      Property * FindProperty(char const * name) const;

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
      std::vector<boost::intrusive_ptr<Property>> properties;
    };

    //
    // LayerBase
    //

    class LayerBase : public PropertyOwner
    {
      friend class Map;

    protected:

      /** constructor */
      LayerBase(class Map * in_map) : 
        map(in_map) { assert(map != nullptr); }

      /** the loading method */
      virtual bool DoLoad(tinyxml2::XMLElement const * element);

    protected:

      /** the owner of the object */
      class Map * map = nullptr;

      /** the name of the layer */
      std::string name;
      /** whether the layer is visible */
      bool visible = true;
      /** whether the layer is locked */
      bool locked = false;
      /** the opacity */
      float opacity = 1.0f;
      /** the offset of the layer */
      glm::ivec2 offset = glm::ivec2(0, 0);
    };

    //
    // ImageLayer
    //

    class ImageLayer : public LayerBase
    {
      friend class Map;

    protected:

      /** constructor */
      ImageLayer(class Map * in_map) :
        LayerBase(in_map) {  }

      /** the loading method */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;

    protected:
      
      /** layer information */
      boost::filesystem::path image_path;
      /** layer information */
      glm::ivec2 size = glm::ivec2(0, 0);
      /** layer information */
      glm::vec4 transparent_color;
    };

    //
    // ObjectLayer
    //

    class ObjectLayer : public LayerBase
    {
      friend class Map;

      static int const DRAW_ORDER_MANUAL  = 0;
      static int const DRAW_ORDER_TOPDOWN = 1;

    protected:

      /** constructor */
      ObjectLayer(class Map * in_map) :
        LayerBase(in_map) {  }

      /** the loading method */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** the loading method */
      bool DoLoadObjects(tinyxml2::XMLElement const * element);
      /** the loading method */
      bool DoLoadOneObject(tinyxml2::XMLElement const * object_element);

    protected:

      /** layer information */
      glm::vec4 color;
      /** layer information */
      int       draw_order;
    };

    class TileLayer : public LayerBase
    {

      friend class Map;

    protected:

      /** constructor */
      TileLayer(class Map * in_map) :
        LayerBase(in_map) {  }

      /** the loading method */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** the loading method */
      bool DoLoadTileBuffer(tinyxml2::XMLElement const * element);
      /** loading buffer method */
      void DoLoadTileBufferFromBase64(Buffer<char> const & buffer);

    protected:

      /** layer information */
      glm::ivec2 size = glm::ivec2(0, 0);
      /** the tiles */
      std::vector<int> tile_indices;
    };

    //
    // ManagerObject : objects control by the manager (Map & TileSet)
    //

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
      ManagerObject(class Manager * in_manager, boost::filesystem::path in_path);
      /** loading method from XML */
      virtual bool DoLoadDocument(tinyxml2::XMLDocument const * doc);
      /** the method to override */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** the method to override */
      virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) { return true; }
      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const { return nullptr; }

    protected:

      /** the manager */
      Manager * manager = nullptr;
      /** the filename */
      boost::filesystem::path path;
    };

    //
    // TileData
    //

    class TileData : public PropertyOwner
    {
      friend class Manager;

    public:

    protected:

      /** the id of the tile */
      int id = 0;
    };

    //
    // TileSet
    //

    class TileSet : public ManagerObject
    {
      static int const ORIENTATION_ORTHOGONAL = 0;
      static int const ORIENTATION_ISOMETRIC  = 1;

      friend class Manager;

    protected:

      /** the constructor */
      TileSet(class Manager * in_manager, boost::filesystem::path in_path);
      /** loading method from XML */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** loading method from XML */
      virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
      /** loading method from XML */
      bool DoLoadTiles(tinyxml2::XMLElement const * element);

      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const override { return "tileset"; }

    protected:

      /** tileset information */
      std::string name;
      /** tileset information */
      int         orientation;
      /** tileset information */
      glm::ivec2  size = glm::ivec2(32, 32);
      /** tileset information */
      glm::ivec2  tile_size = glm::ivec2(0, 0);
      /** tileset information */
      int         columns = 0;
      /** tileset information */
      int         tile_count = 0;
      /** tileset information */
      glm::vec4   background_color;
      /** tileset information */
      boost::filesystem::path image_path;
      /** tileset information */
      glm::ivec2        image_size = glm::vec2(0, 0);
      /** tileset information */
      int               image_margin = 0;
      /** tileset information */
      int               image_spacing = 0;
      /** tileset information */
      glm::vec4         transparent_color;

      /** the data for the tiles */
      std::vector<boost::intrusive_ptr<TileData>> tiles;
    };

    //
    // Map : some map class
    //

    class TileSetData
    {
    public:

      /** the first gid for the tileset */
      int first_gid = 1;
      /** the tileset */
      boost::intrusive_ptr<TileSet> tileset;
    };

    class Map : public ManagerObject
    {
      friend class Manager;

      static int const ORIENTATION_ORTHOGONAL = 0;
      static int const ORIENTATION_ISOMETRIC  = 1;
      static int const ORIENTATION_STAGGERED  = 2;
      static int const ORIENTATION_HEXAGONAL  = 3;

      static int const STAGGERED_AXIS_X = 0;
      static int const STAGGERED_AXIS_Y = 1;

      static int const STAGGERED_INDEX_ODD  = 0;
      static int const STAGGERED_INDEX_EVEN = 1;

      static int const RENDER_ORDER_RIGHT_UP   = 0;
      static int const RENDER_ORDER_RIGHT_DOWN = 1;
      static int const RENDER_ORDER_LEFT_UP    = 2;
      static int const RENDER_ORDER_LEFT_DOWN  = 3;

    protected:

      /** the constructor */
      Map(class Manager * in_manager, boost::filesystem::path in_path);
      /** loading method from XML */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const override { return "map"; }

      /** load internal data */
      virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
      /** load the tile sets */
      bool DoLoadTileSet(tinyxml2::XMLElement const * element);
      /** load the tile layers */
      bool DoLoadTileLayers(tinyxml2::XMLElement const * element);
      /** load the image layers */
      bool DoLoadImageLayers(tinyxml2::XMLElement const * element);
      /** load the object groups */
      bool DoLoadObjectGroups(tinyxml2::XMLElement const * element);

      /** utility function to load a layer */
      template<typename T>
      bool DoLoadLayerHelper(tinyxml2::XMLElement const * element, std::vector<boost::intrusive_ptr<T>> & result, char const * element_name)
      {
        tinyxml2::XMLElement const * e = element->FirstChildElement(element_name);
        for (; e != nullptr; e = e->NextSiblingElement(element_name))
        {
          T * layer = new T(this);
          if (layer == nullptr)
            break;
          if (!layer->DoLoad(e))
            delete(layer);
          else
            result.push_back(layer);
        }
        return true;
      }

    protected:

      /** map information */
      int orientation = ORIENTATION_ORTHOGONAL;
      /** map information */
      glm::ivec2 size = glm::ivec2(100, 100);
      /** map information */
      glm::ivec2 tile_size = glm::ivec2(32, 32);
      /** map information */
      bool infinite = false;
      /** map information */
      int hex_side_length = 0;
      /** map information */
      int stagger_axis = STAGGERED_AXIS_Y;
      /** map information */
      int stagger_index = STAGGERED_INDEX_ODD;
      /** map information */
      int render_order = RENDER_ORDER_RIGHT_DOWN;
      /** map information */
      glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      /** map information */
      std::string version;

      /** the tileset used */
      std::vector<TileSetData> tilesets;
      /** the layers composing the map */
      std::vector<boost::intrusive_ptr<ImageLayer>> image_layers;
      /** the layers composing the map */
      std::vector<boost::intrusive_ptr<TileLayer>> tile_layers;
      /** the layers composing the map */
      std::vector<boost::intrusive_ptr<ObjectLayer>> object_layers;
    };

    //
    // Manager : container for maps and tileset
    //

    class Manager
    {
    public:

      /** load a tiled map set */
      Map * LoadMap(boost::filesystem::path path);
      /** load a tiled map set */
      Map * LoadMap(boost::filesystem::path path, Buffer<char> buffer);
      /** load a tiled map set */
      Map * LoadMap(boost::filesystem::path path, tinyxml2::XMLDocument const * doc);

      /** load a tiled map */
      TileSet * LoadTileSet(boost::filesystem::path path);
      /** load a tiled map */
      TileSet * LoadTileSet(boost::filesystem::path path, Buffer<char> buffer);
      /** load a tiled map */
      TileSet * LoadTileSet(boost::filesystem::path path, tinyxml2::XMLDocument const * doc);

      /** find tiled map */
      Map * FindMap(boost::filesystem::path const & path);
      Map const * FindMap(boost::filesystem::path const & path) const;
      /** find tiled map set */
      TileSet * FindTileSet(boost::filesystem::path const & path);
      TileSet const * FindTileSet(boost::filesystem::path const & path) const;

    protected:

      /** internal method to load a tiled map set (with no search for exisiting items) */
      Map * DoLoadMap(boost::filesystem::path path);
      /** internal method to load a tiled map set (with no search for exisiting items) */
      Map * DoLoadMap(boost::filesystem::path path, Buffer<char> buffer);
      /** internal method to load a tiled map set (with no search for exisiting items) */
      Map * DoLoadMap(boost::filesystem::path path, tinyxml2::XMLDocument const * doc);

      /** internal method to load a tiled map (with no search for exisiting items) */
      TileSet * DoLoadTileSet(boost::filesystem::path path);
      /** internal method to load a tiled map (with no search for exisiting items) */
      TileSet * DoLoadTileSet(boost::filesystem::path path, Buffer<char> buffer);
      /** internal method to load a tiled map (with no search for exisiting items) */
      TileSet * DoLoadTileSet(boost::filesystem::path path, tinyxml2::XMLDocument const * doc);

    protected:

      /** the maps */
      std::vector<boost::intrusive_ptr<Map>> maps;
      /** the assets */
      std::vector<boost::intrusive_ptr<TileSet>> tile_sets;
    };


  }; // namespace TiledMap

}; // namespace chaos
