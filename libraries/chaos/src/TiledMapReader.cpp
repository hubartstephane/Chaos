#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>

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

      tinyxml2::XMLElement const * node = element->FirstChildElement();
      for (; node != nullptr ; node = node->NextSiblingElement())
      {
        if (strcmp(node->Name(), "property") != 0)
          continue;

        char const * property_name  = node->Attribute("name"); // want name + type + value
        if (property_name == nullptr)
          continue;
        if (FindProperty(property_name) != nullptr) // cannot handle property already existing
          continue;
        char const * property_value = node->Attribute("type");
        if (property_value == nullptr)
          continue;
        char const * property_type  = node->Attribute("value");
        if (property_type == nullptr)
          continue;

        if (strcmp(property_type, "int") == 0)
        {
          int value = atoi(property_value);
          DoInsertProperty(property_name, value);
        }
        else if (strcmp(property_type, "float") == 0)
        {
          float value = (float)atof(property_value);
          DoInsertProperty(property_name, value);
        }
        else if (strcmp(property_type, "bool") == 0)
        {
          bool value = (strcmp(property_value, "true") == 0);
          DoInsertProperty(property_name, value);
        }
        else if (strcmp(property_type, "string") == 0)
        {
          DoInsertProperty(property_name, property_value);
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

    ManagerObject::ManagerObject(class Manager * in_manager, char const * in_filename) :
      manager(in_manager)
    {
      assert(in_manager != nullptr);

      if (in_filename != nullptr)
        filename = in_filename;
    }

    bool ManagerObject::IsMatchingName(char const * in_filename) const
    {


      return false;
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

      tinyxml2::XMLElement const * properties_element = element->FirstChildElement("properties");
      if (properties_element != nullptr)
        PropertyOwner::DoLoad(properties_element);
      return true;
    }

    //
    // Map methods
    //

    Map::Map(class Manager * in_manager, char const * in_filename) :
      ManagerObject(in_manager, in_filename)
    {

    }



    bool Map::DoLoad(tinyxml2::XMLElement const * element)
    {
    


      return true;
    }

    //
    // TileSet methods
    //

    TileSet::TileSet(class Manager * in_manager, char const * in_filename) :
      ManagerObject(in_manager, in_filename)
    {

    }

    bool TileSet::DoLoad(tinyxml2::XMLElement const * element)
    {






      return true;
    }


#if 0

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


#endif

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
