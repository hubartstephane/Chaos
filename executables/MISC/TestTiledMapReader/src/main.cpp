#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>
#include <chaos/HTMLTools.h>


class TiledMapPropertyBase : public chaos::ReferencedObject
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

using TiledMapPropertyInt = TiledMapPropertyTemplate<int>;
using TiledMapPropertyFloat = TiledMapPropertyTemplate<float>;
using TiledMapPropertyBool = TiledMapPropertyTemplate<bool>;
using TiledMapPropertyString = TiledMapPropertyTemplate<std::string>;


class TiledMapObjectBase : public chaos::ReferencedObject
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
  virtual bool DoLoad(tinyxml2::XMLDocument * doc);

protected:

  /** the manager */
  TiledMapManager * manager = nullptr;
  /** the filename */
  boost::filesystem::path filename;


};

class TiledMap : public TiledMapObjectBase
{
  friend class TiledMapManager;

protected:

  /** the constructor */
  TiledMap(class TiledMapManager * in_manager, char const * in_filename);
  /** loading method from XML */
  virtual bool DoLoad(tinyxml2::XMLDocument * doc) override;

protected:

  int  width      = 0;
  int  height     = 0;
  int  tilewidth  = 0;
  int  tileheight = 0;
  bool infinite   = false;
};

class TiledMapSet : public TiledMapObjectBase
{
  friend class TiledMapManager;

protected:

  /** the constructor */
  TiledMapSet(class TiledMapManager * in_manager, char const * in_filename);
  /** loading method from XML */
  virtual bool DoLoad(tinyxml2::XMLDocument * doc) override;
};

class TiledMapManager
{
public:

  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * in_filename);
  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * name, chaos::Buffer<char> buffer);
  /** load a tiled map set */
  TiledMap * LoadTiledMap(char const * name, tinyxml2::XMLDocument * doc);

  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * in_filename);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * in_filename, chaos::Buffer<char> buffer);
  /** load a tiled map */
  TiledMapSet * LoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument * doc);


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
  TiledMap * DoLoadTiledMap(char const * name, chaos::Buffer<char> buffer);
  /** internal method to load a tiled map set (with no search for exisiting items) */
  TiledMap * DoLoadTiledMap(char const * name, tinyxml2::XMLDocument * doc);

  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * in_filename);
  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * in_filename, chaos::Buffer<char> buffer);
  /** internal method to load a tiled map (with no search for exisiting items) */
  TiledMapSet * DoLoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument * doc);

protected:

	/** the maps */
	std::vector<boost::intrusive_ptr<TiledMap>> tiled_maps;
	/** the assets */
	std::vector<boost::intrusive_ptr<TiledMapSet>> tiled_sets;
};

//
// TiledMapObjectBase methods
//

TiledMapObjectBase::TiledMapObjectBase(class TiledMapManager * in_manager, char const * in_filename) :
  manager(in_manager)
{
  assert(in_manager != nullptr);

  if (in_filename != nullptr)
    filename = in_filename;
}

bool TiledMapObjectBase::IsMatchingName(char const * in_filename) const
{
  

  return false;
}

bool TiledMapObjectBase::DoLoad(tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);
  return true;
}

//
// TiledMap methods
//

TiledMap::TiledMap(class TiledMapManager * in_manager, char const * in_filename):
  TiledMapObjectBase(in_manager, in_filename)
{

}

bool TiledMap::DoLoad(tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);
/*
  doc->FirstChild;
  doc->FirstChildElement;
  doc->


    */

  return true;
}

//
// TiledMapSet methods
//

TiledMapSet::TiledMapSet(class TiledMapManager * in_manager, char const * in_filename) :
  TiledMapObjectBase(in_manager, in_filename)
{

}

bool TiledMapSet::DoLoad(tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);







  return true;
}

//
// TiledMapManager methods
//

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * in_filename)
{
  TiledMapSet * result = FindTiledMapSet(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(in_filename);
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * in_filename, chaos::Buffer<char> buffer)
{
  TiledMapSet * result = FindTiledMapSet(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(in_filename, buffer);
}

TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument * doc)
{
  TiledMapSet * result = FindTiledMapSet(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMapSet(in_filename, doc);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * in_filename)
{
  TiledMap * result = FindTiledMap(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(in_filename);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * in_filename, chaos::Buffer<char> buffer)
{
  TiledMap * result = FindTiledMap(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(in_filename, buffer);
}

TiledMap * TiledMapManager::LoadTiledMap(char const * in_filename, tinyxml2::XMLDocument * doc)
{
  TiledMap * result = FindTiledMap(in_filename);
  if (result != nullptr)
    return result;
  return DoLoadTiledMap(in_filename, doc);
}

TiledMap * TiledMapManager::FindTiledMap(char const * in_filename)
{
  size_t count = tiled_maps.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_maps[i]->IsMatchingName(in_filename))
      return tiled_maps[i].get();
  return nullptr;
}

TiledMap const * TiledMapManager::FindTiledMap(char const * in_filename) const
{
  size_t count = tiled_maps.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_maps[i]->IsMatchingName(in_filename))
      return tiled_maps[i].get();
  return nullptr;
}

TiledMapSet * TiledMapManager::FindTiledMapSet(char const * in_filename)
{
  size_t count = tiled_sets.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_sets[i]->IsMatchingName(in_filename))
      return tiled_sets[i].get();
  return nullptr;
}

TiledMapSet const * TiledMapManager::FindTiledMapSet(char const * in_filename) const
{
  size_t count = tiled_sets.size();
  for (size_t i = 0; i < count; ++i)
    if (tiled_sets[i]->IsMatchingName(in_filename))
      return tiled_sets[i].get();
  return nullptr;
}












TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * in_filename)
{
  assert(in_filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_filename, true);
  if (buffer != nullptr)
    return DoLoadTiledMapSet(in_filename, buffer);
  return nullptr;
}

TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * in_filename, chaos::Buffer<char> buffer)
{
  TiledMapSet * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = DoLoadTiledMapSet(in_filename, doc);
    delete(doc);
  }
  return result;

}

TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);
  TiledMapSet * result = new TiledMapSet(this, in_filename);
  if (result != nullptr)
  {
    if (result->DoLoad(doc))
      tiled_sets.push_back(result);
    else
    {
      delete(result);
      result = nullptr;
    }
  }
  return result;
}












TiledMap * TiledMapManager::DoLoadTiledMap(char const * in_filename)
{
  assert(in_filename != nullptr);

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_filename, true);
  if (buffer != nullptr)
    return DoLoadTiledMap(in_filename, buffer);
  return nullptr;
}

TiledMap * TiledMapManager::DoLoadTiledMap(char const * in_filename, chaos::Buffer<char> buffer)
{
  TiledMap * result = nullptr;

  tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
    if (error == tinyxml2::XML_SUCCESS)
      result = DoLoadTiledMap(in_filename, doc);
    delete(doc);
  }
  return result;

}

TiledMap * TiledMapManager::DoLoadTiledMap(char const * in_filename, tinyxml2::XMLDocument * doc)
{
  assert(doc != nullptr);

  TiledMap * result = new TiledMap(this, in_filename);
  if (result != nullptr)
  {
    if (result->DoLoad(doc))
      tiled_maps.push_back(result);
    else
    {
      delete(result);
      result = nullptr;
    }
  }
  return result;
}
















/*



*/









// tiled_map : it use tile set that are ordered
//             each tile as a number that correspond to 
//             - a tileset.
//             - a tile in the tileset
// in a tile_set
//    top-left = 1

void LoadTileMap(TiledMapManager & manager)
{
	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return;

	boost::filesystem::path const & resource_path = application->GetResourcesPath();

  chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(resource_path / "tiled_map.tmx", false);
  if (buffer1 != nullptr)
  {
    TiledMapSet * map_set = manager.LoadTiledMapSet(buffer1);

  }

	chaos::Buffer<char> buffer2 = chaos::FileTools::LoadFile(resource_path / "tiled_map.tmx", false);
	if (buffer2 != nullptr)
	{
    TiledMap * map = manager.LoadTiledMap(buffer2);

	}
}

int _tmain(int argc, char ** argv, char ** env)
{
	TiledMapManager manager;

  size_t t = sizeof(std::string);

  boost::filesystem::path p;

  TiledMapPropertyInt pi;
  TiledMapPropertyInt const pic;

  TiledMapPropertyString ps;
  TiledMapPropertyString const psc;

  auto p1 = pi.GetBoolProperty();
  auto p2 = pi.GetIntProperty();
  auto p3 = pi.GetFloatProperty();
  auto p4 = pi.GetStringProperty();

  auto p5 = pic.GetBoolProperty();
  auto p6 = pic.GetIntProperty();
  auto p7 = pic.GetFloatProperty();
  auto p8 = pic.GetStringProperty();

  auto s1 = ps.GetBoolProperty();
  auto s2 = ps.GetIntProperty();
  auto s3 = ps.GetFloatProperty();
  auto s4 = ps.GetStringProperty();

  auto s5 = psc.GetBoolProperty();
  auto s6 = psc.GetIntProperty();
  auto s7 = psc.GetFloatProperty();
  auto s8 = psc.GetStringProperty();

	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();



	LoadTileMap(manager);

	chaos::WinTools::PressToContinue();

	return 0;
}


