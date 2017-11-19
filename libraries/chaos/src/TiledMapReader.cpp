#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>

namespace chaos
{


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

  bool TiledMapObjectBase::DoLoad(tinyxml2::XMLDocument const * doc)
  {
    assert(doc != nullptr);
    return true;
  }

  //
  // TiledMap methods
  //

  TiledMap::TiledMap(class TiledMapManager * in_manager, char const * in_filename) :
    TiledMapObjectBase(in_manager, in_filename)
  {

  }

  bool TiledMap::DoLoad(tinyxml2::XMLDocument const * doc)
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

  bool TiledMapSet::DoLoad(tinyxml2::XMLDocument const * doc)
  {
    assert(doc != nullptr);

    tinyxml2::XMLElement const * root = doc->RootElement();
    if (root != nullptr)
    {
      tinyxml2::XMLAttribute const * attribute = root->FirstAttribute();
      while (attribute != nullptr)
      {


        attribute = attribute->Next();
      }
    }



    tinyxml2::XMLNode const * children = doc->FirstChild();
    while (children != nullptr)
    {

      children = children->NextSiblingElement();
    }






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

  TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * in_filename, Buffer<char> buffer)
  {
    TiledMapSet * result = FindTiledMapSet(in_filename);
    if (result != nullptr)
      return result;
    return DoLoadTiledMapSet(in_filename, buffer);
  }

  TiledMapSet * TiledMapManager::LoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument const * doc)
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

  TiledMap * TiledMapManager::LoadTiledMap(char const * in_filename, Buffer<char> buffer)
  {
    TiledMap * result = FindTiledMap(in_filename);
    if (result != nullptr)
      return result;
    return DoLoadTiledMap(in_filename, buffer);
  }

  TiledMap * TiledMapManager::LoadTiledMap(char const * in_filename, tinyxml2::XMLDocument const * doc)
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

    Buffer<char> buffer = FileTools::LoadFile(in_filename, true);
    if (buffer != nullptr)
      return DoLoadTiledMapSet(in_filename, buffer);
    return nullptr;
  }

  TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * in_filename, Buffer<char> buffer)
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

  TiledMapSet * TiledMapManager::DoLoadTiledMapSet(char const * in_filename, tinyxml2::XMLDocument const * doc)
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

    Buffer<char> buffer = FileTools::LoadFile(in_filename, true);
    if (buffer != nullptr)
      return DoLoadTiledMap(in_filename, buffer);
    return nullptr;
  }

  TiledMap * TiledMapManager::DoLoadTiledMap(char const * in_filename, Buffer<char> buffer)
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

  TiledMap * TiledMapManager::DoLoadTiledMap(char const * in_filename, tinyxml2::XMLDocument const * doc)
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


}; // namespace chaos
