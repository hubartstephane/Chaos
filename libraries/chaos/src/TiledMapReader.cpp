#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>

namespace chaos
{
  namespace TiledMap
  {

    Property * TiledMapObject::FindProperty(char const * property_name)
    {
      assert(property_name != nullptr);

      for (auto & property : properties)
        if (property->GetPropertyName() == property_name)
          return property.get();
      return nullptr;
    }

    Property * TiledMapObject::FindProperty(char const * property_name) const
    {
      for (auto & property : properties)
        if (property->GetPropertyName() == property_name)
          return property.get();
      return nullptr;
    }











    //
    // TiledMapObjectBase methods
    //

    TiledMapObjectBase::TiledMapObjectBase(class Manager * in_manager, char const * in_filename) :
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
    // Map methods
    //

    Map::Map(class Manager * in_manager, char const * in_filename) :
      TiledMapObjectBase(in_manager, in_filename)
    {

    }

    bool Map::DoLoad(tinyxml2::XMLDocument const * doc)
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
    // TileSet methods
    //

    TileSet::TileSet(class Manager * in_manager, char const * in_filename) :
      TiledMapObjectBase(in_manager, in_filename)
    {

    }

    bool TileSet::DoLoad(tinyxml2::XMLDocument const * doc)
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
    // Manager methods
    //

    TileSet * Manager::LoadTileSet(char const * in_filename)
    {
      TileSet * result = FindTileSet(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(in_filename);
    }

    TileSet * Manager::LoadTileSet(char const * in_filename, Buffer<char> buffer)
    {
      TileSet * result = FindTileSet(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(in_filename, buffer);
    }

    TileSet * Manager::LoadTileSet(char const * in_filename, tinyxml2::XMLDocument const * doc)
    {
      TileSet * result = FindTileSet(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(in_filename, doc);
    }

    Map * Manager::LoadMap(char const * in_filename)
    {
      Map * result = FindMap(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadMap(in_filename);
    }

    Map * Manager::LoadMap(char const * in_filename, Buffer<char> buffer)
    {
      Map * result = FindMap(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadMap(in_filename, buffer);
    }

    Map * Manager::LoadMap(char const * in_filename, tinyxml2::XMLDocument const * doc)
    {
      Map * result = FindMap(in_filename);
      if (result != nullptr)
        return result;
      return DoLoadMap(in_filename, doc);
    }

    Map * Manager::FindMap(char const * in_filename)
    {
      size_t count = maps.size();
      for (size_t i = 0; i < count; ++i)
        if (maps[i]->IsMatchingName(in_filename))
          return maps[i].get();
      return nullptr;
    }

    Map const * Manager::FindMap(char const * in_filename) const
    {
      size_t count = maps.size();
      for (size_t i = 0; i < count; ++i)
        if (maps[i]->IsMatchingName(in_filename))
          return maps[i].get();
      return nullptr;
    }

    TileSet * Manager::FindTileSet(char const * in_filename)
    {
      size_t count = tile_sets.size();
      for (size_t i = 0; i < count; ++i)
        if (tile_sets[i]->IsMatchingName(in_filename))
          return tile_sets[i].get();
      return nullptr;
    }

    TileSet const * Manager::FindTileSet(char const * in_filename) const
    {
      size_t count = tile_sets.size();
      for (size_t i = 0; i < count; ++i)
        if (tile_sets[i]->IsMatchingName(in_filename))
          return tile_sets[i].get();
      return nullptr;
    }












    TileSet * Manager::DoLoadTileSet(char const * in_filename)
    {
      assert(in_filename != nullptr);

      Buffer<char> buffer = FileTools::LoadFile(in_filename, true);
      if (buffer != nullptr)
        return DoLoadTileSet(in_filename, buffer);
      return nullptr;
    }

    TileSet * Manager::DoLoadTileSet(char const * in_filename, Buffer<char> buffer)
    {
      TileSet * result = nullptr;

      tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
      if (doc != nullptr)
      {
        tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
        if (error == tinyxml2::XML_SUCCESS)
          result = DoLoadTileSet(in_filename, doc);
        delete(doc);
      }
      return result;

    }

    TileSet * Manager::DoLoadTileSet(char const * in_filename, tinyxml2::XMLDocument const * doc)
    {
      assert(doc != nullptr);
      TileSet * result = new TileSet(this, in_filename);
      if (result != nullptr)
      {
        if (result->DoLoad(doc))
          tile_sets.push_back(result);
        else
        {
          delete(result);
          result = nullptr;
        }
      }
      return result;
    }












    Map * Manager::DoLoadMap(char const * in_filename)
    {
      assert(in_filename != nullptr);

      Buffer<char> buffer = FileTools::LoadFile(in_filename, true);
      if (buffer != nullptr)
        return DoLoadMap(in_filename, buffer);
      return nullptr;
    }

    Map * Manager::DoLoadMap(char const * in_filename, Buffer<char> buffer)
    {
      Map * result = nullptr;

      tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
      if (doc != nullptr)
      {
        tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
        if (error == tinyxml2::XML_SUCCESS)
          result = DoLoadMap(in_filename, doc);
        delete(doc);
      }
      return result;

    }

    Map * Manager::DoLoadMap(char const * in_filename, tinyxml2::XMLDocument const * doc)
    {
      assert(doc != nullptr);

      Map * result = new Map(this, in_filename);
      if (result != nullptr)
      {
        if (result->DoLoad(doc))
          maps.push_back(result);
        else
        {
          delete(result);
          result = nullptr;
        }
      }
      return result;
    }


  };  // namespace TiledMap


}; // namespace chaos
