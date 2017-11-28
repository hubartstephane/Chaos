#include <chaos/TiledMap.h>
#include <chaos/FileTools.h>
#include <chaos/XMLTools.h>
#include <chaos/BoostTools.h>
#include <chaos/PixelTypes.h>
#include <chaos/StringTools.h>
#include <chaos/MyBase64.h>
#include <chaos/MyZLib.h>

namespace chaos
{
	namespace TiledMap
	{
		int GetHEXCharacterCount(char const * c)
		{
			int result = 0;
			while (StringTools::GetHexValueForChar(c[result]) < 16)
				++result;
			return result;
		}

		bool ReadAttributeHEX(tinyxml2::XMLElement const * element, char const * attribute_name, unsigned int & result)
		{
			std::string result_string;
			if (!XMLTools::ReadAttribute(element, attribute_name, result_string))
				return false;

			result = StringTools::AtoiH(result_string.c_str());
			return true;
		}

		static bool ReadXMLColor(tinyxml2::XMLElement const * element, char const * attribute_name, glm::vec4 & result)
		{
			std::string result_string;
			if (!XMLTools::ReadAttribute(element, attribute_name, result_string))
				return false;

			char const * c = result_string.c_str(); // #00000000 is a valid attribute too, as well as hexadecimal strings
			if (c[0] == '#')
				c = c + 1;
			else
				c = StringTools::SkipHEXPrefix(c);

			PixelRGBAFloat rgba_float;
			PixelConverter::Convert(rgba_float, PixelBGRA(StringTools::AtoiH(c)));

			int count = GetHEXCharacterCount(c);
			if (count == 6)
				rgba_float.A = 1.0f;

			result = rgba_float;
			return true;
		}

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
			if (!DoLoadMembers(element))
				return false;
			return true;
		}

		//
		// LayerBase methods
		//

		bool LayerBase::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;

			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "visible", visible);
			XMLTools::ReadAttribute(element, "locked", locked);
			XMLTools::ReadAttribute(element, "opacity", opacity);
			XMLTools::ReadAttribute(element, "offsetx", offset.x);
			XMLTools::ReadAttribute(element, "offsety", offset.y);

			return true;
		}

		//
		// ImageLayer methods
		//

		bool ImageLayer::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!LayerBase::DoLoad(element))
				return false;

			tinyxml2::XMLElement const * image_source = element->FirstChildElement("image");
			if (image_source != nullptr)
			{
				ReadXMLColor(image_source, "trans", transparent_color);
				XMLTools::ReadAttribute(image_source, "width", size.x);
				XMLTools::ReadAttribute(image_source, "height", size.y);

				std::string source;
				XMLTools::ReadAttribute(image_source, "source", source);
				image_path = BoostTools::FindAbsolutePath(map->GetPath(), boost::filesystem::path(source));
			}
			return true;
		}

		//
		// LayerBase methods
		//

		bool ObjectLayer::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!LayerBase::DoLoad(element))
				return false;

			ReadXMLColor(element, "color", color);

			std::pair<char const*, int> const draw_order_map[] = {
				{ "index", DRAW_ORDER_MANUAL },
				{ nullptr, DRAW_ORDER_TOPDOWN }
			};
			XMLTools::ReadEnumAttribute(element, "draworder", draw_order_map, draw_order);

			if (!DoLoadObjects(element))
				return false;

			return true;
		}

		bool ObjectLayer::DoLoadObjects(tinyxml2::XMLElement const * element)
		{
			tinyxml2::XMLElement const * object_element = element->FirstChildElement("object");
			for (; object_element != nullptr; object_element = object_element->NextSiblingElement("object"))
				if (!DoLoadOneObject(object_element))
					return false;
			return true;
		}

		bool ObjectLayer::DoLoadOneObject(tinyxml2::XMLElement const * object_element)
		{








			return true;
		}

		//
		// TileLayer methods
		//

		bool TileLayer::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!LayerBase::DoLoad(element))
				return false;

			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);

			if (!DoLoadTileBuffer(element))
				return false;

			return true;
		}

		void TileLayer::DoLoadTileBufferFromBase64(Buffer<char> const & buffer)
		{
			size_t count = size.x * size.y;

			if (buffer.bufsize != count * sizeof(uint32_t))  // array width * height * sizeof(uint32)
				return;

			// transform the char buffer into tiles
			for (size_t i = 0; i < count; ++i)
			{
				unsigned int a = (unsigned int)buffer[i * 4 + 0]; 
				unsigned int b = (unsigned int)buffer[i * 4 + 1];
				unsigned int c = (unsigned int)buffer[i * 4 + 2];
				unsigned int d = (unsigned int)buffer[i * 4 + 3];

				int tile = (a << 0) | (b << 8) | (c << 16) | (d << 24);
				tile_indices.push_back(tile);
			}
		}

		bool TileLayer::DoLoadTileBuffer(tinyxml2::XMLElement const * element)
		{
			tinyxml2::XMLElement const * data = element->FirstChildElement("data");
			if (data == nullptr)
				return false;

			std::string encoding;
			XMLTools::ReadAttribute(data, "encoding", encoding);

			// read the data
			size_t count = size.x * size.y;
			tile_indices.reserve(count);

			if (encoding == "base64")
			{
				std::string compression;
				XMLTools::ReadAttribute(data, "compression", compression);

				if (compression == "gzip")
				{









				}
				else if (compression == "zlib")
				{
					std::string content = StringTools::TrimBase64String(data->GetText());

					Buffer<char> base64  = MyBase64().Decode(content.c_str()); 
					Buffer<char> decoded = MyZLib().Decode(base64);

					DoLoadTileBufferFromBase64(decoded);
				}
				else // no encoding
				{
					std::string content = StringTools::TrimBase64String(data->GetText());

					Buffer<char> base64 = MyBase64().Decode(content.c_str());

					DoLoadTileBufferFromBase64(base64);
				}
			}
			else if (encoding == "csv")
			{
				char const * txt = data->GetText();

				int i = 0;
				while (txt[i] != 0 && tile_indices.size() != count)
				{
					while (txt[i] != 0 && !isdigit(txt[i])) // search first figure
						++i;
					if (txt[i] == 0)
						break;

					int value = atoi(&txt[i]);
					tile_indices.push_back(value); // get and push the tile

					while (txt[i] != 0 && isdigit(txt[i])) // skip all figures
						++i;
				}
			}
			else // else XML
			{
				tinyxml2::XMLElement const * child = data->FirstChildElement("tile");
				while (child != nullptr && tile_indices.size() != count)
				{
					int value = 0;
					XMLTools::ReadAttribute(child, "gid", value);
					tile_indices.push_back(value);

					child = child->NextSiblingElement("tile");
				}
			}
			return (tile_indices.size() == count);
		}

    //
    // GroundData methods
    //

    bool GroundData::DoLoad(tinyxml2::XMLElement const * element)
    {
      if (!PropertyOwner::DoLoad(element))
        return false;

      XMLTools::ReadAttribute(element, "tile", tile_index);
      XMLTools::ReadAttribute(element, "name", name);

      return true;
    }

		//
		// TileData methods
		//

		bool TileData::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;

			XMLTools::ReadAttribute(element, "id", id);
			XMLTools::ReadAttribute(element, "type", type);
			XMLTools::ReadAttribute(element, "probability", probability);
			
			return true;
		}

		//
		// TileSet methods
		//

		TileSet::TileSet(class Manager * in_manager, boost::filesystem::path in_path) :
			ManagerObject(in_manager, in_path)
		{

		}
    
    bool TileSet::DoLoadGrounds(tinyxml2::XMLElement const * element)
    {
      return DoLoadObjectListHelper(element, grounds, "terrain", "terraintypes");
    }

		bool TileSet::DoLoadTiles(tinyxml2::XMLElement const * element)
		{
      return DoLoadObjectListHelper(element, tiles, "tile", nullptr);		
		}

		bool TileSet::DoLoadMembers(tinyxml2::XMLElement const * element)
		{
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "tilewidth", tile_size.x);
			XMLTools::ReadAttribute(element, "tileheight", tile_size.y);
			XMLTools::ReadAttribute(element, "tilecount", tile_count);
			XMLTools::ReadAttribute(element, "columns", columns);
			XMLTools::ReadAttribute(element, "margin", image_margin);
			XMLTools::ReadAttribute(element, "spacing", image_spacing);

			ReadXMLColor(element, "backgroundcolor", background_color);

			tinyxml2::XMLElement const * grid_element = element->FirstChildElement("grid");
			if (grid_element != nullptr)
			{
				std::pair<char const*, int> const orientation_map[] = {
					{ "isometric", ORIENTATION_ISOMETRIC },
					{ nullptr, ORIENTATION_ORTHOGONAL }
				};
				XMLTools::ReadEnumAttribute(grid_element, "orientation", orientation_map, orientation);

				XMLTools::ReadAttribute(grid_element, "width", size.x);
				XMLTools::ReadAttribute(grid_element, "height", size.y);
			}

			tinyxml2::XMLElement const * image_element = element->FirstChildElement("image");
			if (image_element != nullptr)
			{
				std::string source;
				XMLTools::ReadAttribute(image_element, "source", source);
				image_path = BoostTools::FindAbsolutePath(GetPath(), boost::filesystem::path(source));

				ReadXMLColor(image_element, "trans", transparent_color);

				XMLTools::ReadAttribute(image_element, "width", image_size.x);
				XMLTools::ReadAttribute(image_element, "height", image_size.y);
			}

			return true;
		}

		bool TileSet::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!ManagerObject::DoLoad(element))
				return false;
			if (!DoLoadTiles(element))
				return false;
      if (!DoLoadGrounds(element))
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

		bool Map::DoLoadMembers(tinyxml2::XMLElement const * element)
		{
			std::pair<char const*, int> const orient_map[] = {
				{ "orthogonal", ORIENTATION_ORTHOGONAL },
				{ "isometric" , ORIENTATION_ISOMETRIC },
				{ "staggered" , ORIENTATION_STAGGERED },
				{ "hexagonal" , ORIENTATION_HEXAGONAL },
				{ nullptr, 0}
			};
			XMLTools::ReadEnumAttribute(element, "orientation", orient_map, orientation);

			std::pair<char const*, int> const stagger_axis_map[] = {
				{ "Y", STAGGERED_AXIS_Y },
				{ nullptr, STAGGERED_AXIS_X }
			};
			XMLTools::ReadEnumAttribute(element, "staggeraxis", stagger_axis_map, stagger_axis);

			std::pair<char const*, int> const stagger_index_map[] = {
				{ "even", STAGGERED_INDEX_EVEN },
				{ nullptr, STAGGERED_INDEX_ODD }
			};
			XMLTools::ReadEnumAttribute(element, "staggerindex", stagger_index_map, stagger_index);

			std::pair<char const*, int> const render_order_map[] = {
				{ "right-up", RENDER_ORDER_RIGHT_UP },
				{ "right-down", RENDER_ORDER_RIGHT_DOWN },
				{ "left-up", RENDER_ORDER_LEFT_UP },
				{ "left-down", RENDER_ORDER_LEFT_DOWN },
				{ nullptr, 0 }
			};
			XMLTools::ReadEnumAttribute(element, "renderorder", render_order_map, render_order);

			XMLTools::ReadAttribute(element, "version", version);
			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);
			XMLTools::ReadAttribute(element, "tilewidth", tile_size.x);
			XMLTools::ReadAttribute(element, "tileheight", tile_size.y);
			XMLTools::ReadAttribute(element, "infinite", infinite);
			XMLTools::ReadAttribute(element, "hexsidelength", hex_side_length);

			ReadXMLColor(element, "backgroundcolor", background_color);

			return true;
		}

		bool Map::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!ManagerObject::DoLoad(element))
				return false;
			if (!DoLoadTileSet(element))
				return false;
			if (!DoLoadTileLayers(element))
				return false;
			if (!DoLoadImageLayers(element))
				return false;
			if (!DoLoadObjectGroups(element))
				return false;

			return true;
		}

		bool Map::DoLoadTileSet(tinyxml2::XMLElement const * element)
		{
			assert(element != nullptr);

			tinyxml2::XMLElement const * tileset = element->FirstChildElement("tileset");
			for (; tileset != nullptr; tileset = tileset->NextSiblingElement("tileset"))
			{
				int first_gid = 0;
				std::string source;

				if (!XMLTools::ReadAttribute(tileset, "firstgid", first_gid)) // firstgid is mandatory (map would be incomplete)
					return false;
				if (!XMLTools::ReadAttribute(tileset, "source", source)) // source is mandatory (map would be incomplete)
					return false;

				boost::filesystem::path tileset_path = BoostTools::FindAbsolutePath(path, source); // compute the path of the tileset relative to this

				TileSet * tileset = manager->LoadTileSet(tileset_path);
				if (tileset == nullptr)
					return false;

				TileSetData data;
				data.first_gid = first_gid;
				data.tileset = tileset;
				tilesets.push_back(data);
			}
			return true;
		}

		bool Map::DoLoadImageLayers(tinyxml2::XMLElement const * element)
		{
			return DoLoadObjectListHelper(element, image_layers, "imagelayer", nullptr, this);
		}

		bool Map::DoLoadObjectGroups(tinyxml2::XMLElement const * element)
		{ 
			return DoLoadObjectListHelper(element, object_layers, "objectgroup", nullptr, this);
		}

		bool Map::DoLoadTileLayers(tinyxml2::XMLElement const * element)
		{
			return DoLoadObjectListHelper(element, tile_layers, "layer", nullptr, this);
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
