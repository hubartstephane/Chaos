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
    // Layer
    //

    class Layer : public PropertyOwner
    {
      friend class Map;

    protected:

      /** the loading method */
      virtual bool DoLoad(tinyxml2::XMLElement const * element);

    protected:

      /** the size of the layer */
      int width = 0;
      /** the size of the layer */
      int height = 0;
      /** the name of the layer */
      std::string name;
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
      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const { return nullptr; }

    protected:

      /** the manager */
      Manager * manager = nullptr;
      /** the filename */
      boost::filesystem::path path;
    };

    //
    // TileSet
    //

    class TileSet : public ManagerObject
    {
      friend class Manager;

    protected:

      /** the constructor */
      TileSet(class Manager * in_manager, boost::filesystem::path in_path);
      /** loading method from XML */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const override { return "tileset"; }
    };

    //
    // Map : some map class
    //

    class TileSetData
    {
    public:

      /** the first gid for the tileset */
      int firstgid = 1;
      /** the tileset */
      boost::intrusive_ptr<TileSet> tileset;
    };

    class Map : public ManagerObject
    {
      friend class Manager;

    protected:

      /** the constructor */
      Map(class Manager * in_manager, boost::filesystem::path in_path);
      /** loading method from XML */
      virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
      /** get the name of the expected markup */
      virtual char const * GetXMLMarkupName() const override { return "map"; }

      /** load the tile sets */
      bool DoLoadTileSet(tinyxml2::XMLElement const * element);
      /** load the layers */
      bool DoLoadLayers(tinyxml2::XMLElement const * element);

    protected:

      /** the dimension of the map */
      int  width = 0;
      /** the dimension of the map */
      int  height = 0;
      /** the dimension of the map */
      int  tilewidth = 0;
      /** the dimension of the map */
      int  tileheight = 0;
      /** the dimension of the map */
      bool infinite = false;


      /** the tileset used */
      std::vector<TileSetData> tilesets;
      /** the layers composing the map */
      std::vector<boost::intrusive_ptr<Layer>> layers;
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
