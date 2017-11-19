#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Buffer.h>

namespace chaos
{
  //
  // TiledMapPropertyBase : base class for some properties
  //

  class TiledMapPropertyBase : public ReferencedObject
  {
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

  public:

    /** the name of the property */
    std::string property_name;
  };

  //
  // TiledMapPropertyTemplate : templated specialization for properties
  //

  template<typename T>
  class TiledMapPropertyTemplate : public TiledMapPropertyBase
  {
  public:

    /** the type of the property */
    typedef T property_type;

    /** returns a pointer on the int property */
    virtual int * GetIntProperty() { return CastPropertyTo(&property_value, boost::mpl::identity<int>()); }
    virtual int const * GetIntProperty() const { return CastPropertyTo(&property_value, boost::mpl::identity<int>()); }
    /** returns a pointer on the float property */
    virtual float * GetFloatProperty() { return CastPropertyTo(&property_value, boost::mpl::identity<float>()); }
    virtual float const * GetFloatProperty() const { return CastPropertyTo(&property_value, boost::mpl::identity<float>()); }
    /** returns a pointer on the int property */
    virtual bool * GetBoolProperty() { return CastPropertyTo(&property_value, boost::mpl::identity<bool>()); }
    virtual bool const * GetBoolProperty() const { return CastPropertyTo(&property_value, boost::mpl::identity<bool>()); }
    /** returns a pointer on the int property */
    virtual std::string * GetStringProperty() { return CastPropertyTo(&property_value, boost::mpl::identity<std::string>()); }
    virtual std::string const * GetStringProperty() const { return CastPropertyTo(&property_value, boost::mpl::identity<std::string>()); }

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
    T property_value;
  };

  //
  // Specialization of properties
  //

  using TiledMapPropertyInt = TiledMapPropertyTemplate<int>;
  using TiledMapPropertyFloat = TiledMapPropertyTemplate<float>;
  using TiledMapPropertyBool = TiledMapPropertyTemplate<bool>;
  using TiledMapPropertyString = TiledMapPropertyTemplate<std::string>;


  //
  // TiledMapObjectBase : base object for both Map and both MapSet
  //

  class TiledMapObjectBase : public ReferencedObject
  {
    friend class TiledMapManager;

  public:

    /** returns true whether the name match the resource name */
    bool IsMatchingName(char const * in_filename) const;
    /** get the path */
    boost::filesystem::path const & GetPath() const { return filename; }

  protected:

    /** the constructor */
    TiledMapObjectBase(class TiledMapManager * in_manager, char const * in_filename);
    /** loading method from XML */
    virtual bool DoLoad(tinyxml2::XMLDocument const * doc);

  protected:

    /** the manager */
    TiledMapManager * manager = nullptr;
    /** the filename */
    boost::filesystem::path filename;


  };

  //
  // TiledMap : some map class
  //

  class TiledMap : public TiledMapObjectBase
  {
    friend class TiledMapManager;

  protected:

    /** the constructor */
    TiledMap(class TiledMapManager * in_manager, char const * in_filename);
    /** loading method from XML */
    virtual bool DoLoad(tinyxml2::XMLDocument const * doc) override;

  protected:

    int  width = 0;
    int  height = 0;
    int  tilewidth = 0;
    int  tileheight = 0;
    bool infinite = false;
  };

  //
  // TiledMapSet
  //

  class TiledMapSet : public TiledMapObjectBase
  {
    friend class TiledMapManager;

  protected:

    /** the constructor */
    TiledMapSet(class TiledMapManager * in_manager, char const * in_filename);
    /** loading method from XML */
    virtual bool DoLoad(tinyxml2::XMLDocument const * doc) override;
  };

  //
  // TiledMapManager : container for maps and tileset
  //

  class TiledMapManager
  {
  public:

    /** load a tiled map set */
    TiledMap * LoadTiledMap(char const * in_filename);
    /** load a tiled map set */
    TiledMap * LoadTiledMap(char const * name, Buffer<char> buffer);
    /** load a tiled map set */
    TiledMap * LoadTiledMap(char const * name, tinyxml2::XMLDocument const * doc);

    /** load a tiled map */
    TiledMapSet * LoadTiledMapSet(char const * in_filename);
    /** load a tiled map */
    TiledMapSet * LoadTiledMapSet(char const * in_filename, Buffer<char> buffer);
    /** load a tiled map */
    TiledMapSet * LoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument const * doc);


    /** find tiled map */
    TiledMap * FindTiledMap(char const * in_filename);
    TiledMap const * FindTiledMap(char const * in_filename) const;
    /** find tiled map set */
    TiledMapSet * FindTiledMapSet(char const * in_filename);
    TiledMapSet const * FindTiledMapSet(char const * in_filename) const;


  protected:

    /** internal method to load a tiled map set (with no search for exisiting items) */
    TiledMap * DoLoadTiledMap(char const * in_filename);
    /** internal method to load a tiled map set (with no search for exisiting items) */
    TiledMap * DoLoadTiledMap(char const * name, Buffer<char> buffer);
    /** internal method to load a tiled map set (with no search for exisiting items) */
    TiledMap * DoLoadTiledMap(char const * name, tinyxml2::XMLDocument const * doc);

    /** internal method to load a tiled map (with no search for exisiting items) */
    TiledMapSet * DoLoadTiledMapSet(char const * in_filename);
    /** internal method to load a tiled map (with no search for exisiting items) */
    TiledMapSet * DoLoadTiledMapSet(char const * in_filename, Buffer<char> buffer);
    /** internal method to load a tiled map (with no search for exisiting items) */
    TiledMapSet * DoLoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument const * doc);

  protected:

    /** the maps */
    std::vector<boost::intrusive_ptr<TiledMap>> tiled_maps;
    /** the assets */
    std::vector<boost::intrusive_ptr<TiledMapSet>> tiled_sets;
  };

}; // namespace chaos
