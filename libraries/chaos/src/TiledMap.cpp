#include <chaos/TiledMap.h>
#include <chaos/FileTools.h>
#include <chaos/XMLTools.h>

namespace chaos
{
  namespace TiledMap
  {

    //
    // PropertyOwner methods
    //

    Property * PropertyOwner::FindProperty(char const * name)
    {
      assert(name != nullptr);
      for (auto & property : properties)
        if (property->GetName() == name)
          return property.get();
      return nullptr;
    }

    Property * PropertyOwner::FindProperty(char const * name) const
    {
      assert(name != nullptr);
      for (auto & property : properties)
        if (property->GetName() == name)
          return property.get();
      return nullptr;
    }

    bool PropertyOwner::DoLoad(tinyxml2::XMLElement const * element)
    {
      assert(element != nullptr);

      tinyxml2::XMLElement const * properties_element = element->FirstChildElement("properties");
      if (properties_element == nullptr)
        return true; // no properties is not an error

      tinyxml2::XMLElement const * node = properties_element->FirstChildElement("property");
      for (; node != nullptr ; node = node->NextSiblingElement("property"))
      {
        tinyxml2::XMLAttribute const * name_attribute = node->FindAttribute("name"); // name is mandatory
        if (name_attribute == nullptr)
          continue;
        char const * property_name = name_attribute->Value();
        if (FindProperty(property_name) != nullptr) // and must be unique
          continue;

        tinyxml2::XMLAttribute const * value_attribute = node->FindAttribute("value"); // value is mandatory
        if (value_attribute == nullptr)
          continue;

        tinyxml2::XMLAttribute const * type_attribute = node->FindAttribute("type"); // type is NOT mandatory (default is string)

        char const * property_type = (type_attribute != nullptr) ? 
          type_attribute->Value() : nullptr;

        if (property_type == nullptr || strcmp(property_type, "string") == 0)
        {
          DoInsertProperty(property_name, value_attribute->Value());
        }
        else if (strcmp(property_type, "int") == 0)
        {
          DoInsertProperty(property_name, value_attribute->IntValue());
        }
        else if (strcmp(property_type, "float") == 0)
        {
          DoInsertProperty(property_name, value_attribute->FloatValue());
        }
        else if (strcmp(property_type, "bool") == 0)
        {
          DoInsertProperty(property_name, value_attribute->BoolValue());
        }
      }
      return true;
    }

    PropertyInt * PropertyOwner::DoInsertProperty(char const * name, int value)
    {
      PropertyInt * result = new PropertyInt;
      if (result != nullptr)
      {
        result->name = name;
        result->value = value;
        properties.push_back(result);
      }
      return result;
    }

    PropertyFloat * PropertyOwner::DoInsertProperty(char const * name, float value)
    {
      PropertyFloat * result = new PropertyFloat;
      if (result != nullptr)
      {
        result->name = name;
        result->value = value;
        properties.push_back(result);
      }
      return result;
    }

    PropertyBool * PropertyOwner::DoInsertProperty(char const * name, bool value)
    {
      PropertyBool * result = new PropertyBool;
      if (result != nullptr)
      {
        result->name = name;
        result->value = value;
        properties.push_back(result);
      }
      return result;
    }

    PropertyString * PropertyOwner::DoInsertProperty(char const * name, char const * value)
    {
      PropertyString * result = new PropertyString;
      if (result != nullptr)
      {
        result->name = name;
        result->value = value;
        properties.push_back(result);
      }
      return result;
    }

    //
    // ManagerObject methods
    //

    ManagerObject::ManagerObject(class Manager * in_manager, boost::filesystem::path in_path) :
      manager(in_manager),
      path(std::move(in_path))
    {
      assert(in_manager != nullptr);
    }

    bool ManagerObject::IsMatchingName(boost::filesystem::path const & in_path) const
    {
      return (path.compare(in_path) == 0);
    }

    bool ManagerObject::DoLoadDocument(tinyxml2::XMLDocument const * doc)
    {
      assert(doc != nullptr);

      tinyxml2::XMLElement const * element = doc->RootElement();
      if (element == nullptr)
        return false;

      if (strcmp(element->Name(), GetXMLMarkupName()) != 0)
        return false;

      return DoLoad(element);
    }

    bool ManagerObject::DoLoad(tinyxml2::XMLElement const * element)
    {
      assert(element != nullptr);
      if (!PropertyOwner::DoLoad(element))
        return false;
      return true;
    }

    //
    // Layer methods
    //
      
    bool Layer::DoLoad(tinyxml2::XMLElement const * element)
    {
      if (!PropertyOwner::DoLoad(element))
        return false;

      XMLTools::ReadAttribute(element, "name", name);
      XMLTools::ReadAttribute(element, "width", width);
      XMLTools::ReadAttribute(element, "height", height);

      return true;
    }

    //
    // TileSet methods
    //

    TileSet::TileSet(class Manager * in_manager, boost::filesystem::path in_path) :
      ManagerObject(in_manager, in_path)
    {

    }

    bool TileSet::DoLoad(tinyxml2::XMLElement const * element)
    {
      if (!ManagerObject::DoLoad(element))
        return false;





      return true;
    }

    //
    // Map methods
    //

    Map::Map(class Manager * in_manager, boost::filesystem::path in_path) :
      ManagerObject(in_manager, in_path)
    {

    }



    bool Map::DoLoad(tinyxml2::XMLElement const * element)
    {
      if (!ManagerObject::DoLoad(element))
        return false;

      XMLTools::ReadAttribute(element, "width", width);
      XMLTools::ReadAttribute(element, "height", height);
      XMLTools::ReadAttribute(element, "tilewidth", tilewidth);
      XMLTools::ReadAttribute(element, "tileheight", tileheight);
      XMLTools::ReadAttribute(element, "infinite", infinite);

      DoLoadTileSet(element);
      DoLoadLayers(element);

        //orientation = "orthogonal" renderorder = "right-down" width = "32" height = "48" tilewidth = "32" tileheight = "32" infinite = "0"
 


      return true;
    }

    bool Map::DoLoadTileSet(tinyxml2::XMLElement const * element)
    {
      assert(element != nullptr);

      tinyxml2::XMLElement const * tileset = element->FirstChildElement("tileset");
      for (; tileset != nullptr; tileset = tileset->NextSiblingElement("tileset"))
      {
        int firstgid = 0;
        std::string source;

        if (!XMLTools::ReadAttribute(tileset, "firstgid", firstgid))
          continue;
        if (!XMLTools::ReadAttribute(tileset, "source", source))
          continue;

        //boost::filesystem::path p = GetRelativePath()


        firstgid = firstgid;

      }
      return true;
    }

    bool Map::DoLoadLayers(tinyxml2::XMLElement const * element)
    {
      tinyxml2::XMLElement const * tileset = element->FirstChildElement("layer");
      for (; tileset != nullptr; tileset = tileset->NextSiblingElement("layer"))
      {



        tileset = tileset;
      }


      return true;
    }



    //
    // Manager methods
    //

    TileSet * Manager::LoadTileSet(boost::filesystem::path path)
    {
      TileSet * result = FindTileSet(path);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(std::move(path));
    }

    TileSet * Manager::LoadTileSet(boost::filesystem::path path, Buffer<char> buffer)
    {
      TileSet * result = FindTileSet(path);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(std::move(path), buffer);
    }

    TileSet * Manager::LoadTileSet(boost::filesystem::path path, tinyxml2::XMLDocument const * doc)
    {
      TileSet * result = FindTileSet(path);
      if (result != nullptr)
        return result;
      return DoLoadTileSet(std::move(path), doc);
    }

    Map * Manager::LoadMap(boost::filesystem::path path)
    {
      Map * result = FindMap(path);
      if (result != nullptr)
        return result;
      return DoLoadMap(std::move(path));
    }

    Map * Manager::LoadMap(boost::filesystem::path path, Buffer<char> buffer)
    {
      Map * result = FindMap(path);
      if (result != nullptr)
        return result;
      return DoLoadMap(std::move(path), buffer);
    }

    Map * Manager::LoadMap(boost::filesystem::path path, tinyxml2::XMLDocument const * doc)
    {
      Map * result = FindMap(path);
      if (result != nullptr)
        return result;
      return DoLoadMap(std::move(path), doc);
    }

    Map * Manager::FindMap(boost::filesystem::path const & path)
    {
      size_t count = maps.size();
      for (size_t i = 0; i < count; ++i)
        if (maps[i]->IsMatchingName(path))
          return maps[i].get();
      return nullptr;
    }

    Map const * Manager::FindMap(boost::filesystem::path const & path) const
    {
      size_t count = maps.size();
      for (size_t i = 0; i < count; ++i)
        if (maps[i]->IsMatchingName(path))
          return maps[i].get();
      return nullptr;
    }

    TileSet * Manager::FindTileSet(boost::filesystem::path const & path)
    {
      size_t count = tile_sets.size();
      for (size_t i = 0; i < count; ++i)
        if (tile_sets[i]->IsMatchingName(path))
          return tile_sets[i].get();
      return nullptr;
    }

    TileSet const * Manager::FindTileSet(boost::filesystem::path const & path) const
    {
      size_t count = tile_sets.size();
      for (size_t i = 0; i < count; ++i)
        if (tile_sets[i]->IsMatchingName(path))
          return tile_sets[i].get();
      return nullptr;
    }

    TileSet * Manager::DoLoadTileSet(boost::filesystem::path path)
    {
      Buffer<char> buffer = FileTools::LoadFile(path, true);
      if (buffer != nullptr)
        return DoLoadTileSet(std::move(path), buffer);
      return nullptr;
    }

    TileSet * Manager::DoLoadTileSet(boost::filesystem::path path, Buffer<char> buffer)
    {
      TileSet * result = nullptr;

      tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
      if (doc != nullptr)
      {
        tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
        if (error == tinyxml2::XML_SUCCESS)
          result = DoLoadTileSet(std::move(path), doc);
        delete(doc);
      }
      return result;

    }

    TileSet * Manager::DoLoadTileSet(boost::filesystem::path path, tinyxml2::XMLDocument const * doc)
    {
      assert(doc != nullptr);
      TileSet * result = new TileSet(this, std::move(path));
      if (result != nullptr)
      {
        if (result->DoLoadDocument(doc))
          tile_sets.push_back(result);
        else
        {
          delete(result);
          result = nullptr;
        }
      }
      return result;
    }

    Map * Manager::DoLoadMap(boost::filesystem::path path)
    {
      Buffer<char> buffer = FileTools::LoadFile(path, true);
      if (buffer != nullptr)
        return DoLoadMap(std::move(path), buffer);
      return nullptr;
    }

    Map * Manager::DoLoadMap(boost::filesystem::path path, Buffer<char> buffer)
    {
      Map * result = nullptr;

      tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
      if (doc != nullptr)
      {
        tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
        if (error == tinyxml2::XML_SUCCESS)
          result = DoLoadMap(std::move(path), doc);
        delete(doc);
      }
      return result;
    }

    Map * Manager::DoLoadMap(boost::filesystem::path path, tinyxml2::XMLDocument const * doc)
    {
      Map * result = new Map(this, std::move(path));
      if (result != nullptr)
      {
        if (result->DoLoadDocument(doc))
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
