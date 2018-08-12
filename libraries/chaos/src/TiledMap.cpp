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
		// BaseObject methods
		//

		boost::filesystem::path BaseObject::GetOwnerPath() const
		{
			// XXX : order is important, because some tilesets are embedded inside a Map directly
			//       and so their real path is given by MAP->GetPath()

			// try to get the map
			Map const * map = GetOwner<Map>();
			if (map != nullptr)
				return map->GetPath();
			// try to get the tileset
			TileSet const * tileset = GetOwner<TileSet>();
			if (tileset != nullptr)
				return tileset->GetPath();
			// default path
			return boost::filesystem::path();
		}

		//
		// PropertyOwner methods
		//

		Property * PropertyOwner::FindProperty(char const * name, int type_id)
		{
			assert(name != nullptr);
			for (auto & property : properties)
				if (type_id == Property::PROPERTY_TYPEID_ANY || type_id == property->GetPropertyTypeID())
					if (strcmp(property->GetName(), name) == 0)
						return property.get();
			return nullptr;
		}

		Property const * PropertyOwner::FindProperty(char const * name, int type_id) const
		{
			assert(name != nullptr);
			for (auto & property : properties)
				if (type_id == Property::PROPERTY_TYPEID_ANY || type_id == property->GetPropertyTypeID())
					if (strcmp(property->GetName(), name) == 0)
						return property.get();
			return nullptr;
		}

		int * PropertyOwner::FindPropertyInt(char const * name)
		{
			Property * property = FindProperty(name, Property::PROPERTY_TYPEID_INT);
			if (property == nullptr)
				return nullptr;
			return property->GetIntProperty();
		}

		int const * PropertyOwner::FindPropertyInt(char const * name) const
		{
			Property const * property = FindProperty(name, Property::PROPERTY_TYPEID_INT);
			if (property == nullptr)
				return nullptr;
			return property->GetIntProperty();
		}

		float * PropertyOwner::FindPropertyFloat(char const * name)
		{
			Property * property = FindProperty(name, Property::PROPERTY_TYPEID_FLOAT);
			if (property == nullptr)
				return nullptr;
			return property->GetFloatProperty();
		}

		float const * PropertyOwner::FindPropertyFloat(char const * name) const
		{
			Property const * property = FindProperty(name, Property::PROPERTY_TYPEID_FLOAT);
			if (property == nullptr)
				return nullptr;
			return property->GetFloatProperty();
		}

		bool * PropertyOwner::FindPropertyBool(char const * name)
		{
			Property * property = FindProperty(name, Property::PROPERTY_TYPEID_BOOL);
			if (property == nullptr)
				return nullptr;
			return property->GetBoolProperty();
		}

		bool const * PropertyOwner::FindPropertyBool(char const * name) const
		{
			Property const * property = FindProperty(name, Property::PROPERTY_TYPEID_BOOL);
			if (property == nullptr)
				return nullptr;
			return property->GetBoolProperty();
		}

		std::string * PropertyOwner::FindPropertyString(char const * name)
		{
			Property * property = FindProperty(name, Property::PROPERTY_TYPEID_STRING);
			if (property == nullptr)
				return nullptr;
			return property->GetStringProperty();
		}

		std::string const * PropertyOwner::FindPropertyString(char const * name) const
		{
			Property const * property = FindProperty(name, Property::PROPERTY_TYPEID_STRING);
			if (property == nullptr)
				return nullptr;
			return property->GetStringProperty();
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
			PropertyInt * result = new PropertyInt(this);
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
			PropertyFloat * result = new PropertyFloat(this);
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
			PropertyBool * result = new PropertyBool(this);
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
			PropertyString * result = new PropertyString(this);
			if (result != nullptr)
			{
				result->name = name;
				result->value = value;
				properties.push_back(result);
			}
			return result;
		}

		//
		// GeometricObject methods
		//

		std::vector<glm::vec2> GeometricObject::GetPointArray(tinyxml2::XMLElement const * element, char const * attribute_name)
		{
			std::vector<glm::vec2> result;

			tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
			if (attribute != nullptr)
			{
				char const * values = attribute->Value();
				if (values != nullptr)
				{
					glm::vec2 p = glm::vec2(0.0f, 0.0f);

					int coord = 0;         
					int i = 0;
					while (values[i] != 0)
					{
						p[coord++] = (float)atof(&values[i]);
						if (coord == 2)
						{
							result.push_back(p);
							p = glm::vec2(0.0f, 0.0f); // flush the point if both axis are found
							coord = 0;
						}

						while (values[i] != 0 && values[i] != ',' && values[i] != ' ') // skip until separator
							++i;
						if (values[i] == ',' || values[i] == ' ')
							++i;
					}
					if (coord == 1)
						result.push_back(p); // flush the point even if there is a missing axis
				}
			}
			return result;
		}
		// XXX : the position is BOTTOMLEFT ... (in the viewer point of view) 
		//       but in TiledMap, the Y axis is directed DOWN
		//       means BOTTOMLEFT.y is the greatest Y for the object
		box2 GeometricObjectSurface::GetBoundingBox() const
		{
			// shuxxx
			return box2(std::make_pair(position, position + size)); 
#if 0
			glm::vec2 p1 = position;
			glm::vec2 p2 = position;
			p2.x += size.x;
			p2.y -= size.y; // axis Y is DOWN !!!
			return box2(std::make_pair(p1, p2)); 
#endif
		}

		bool GeometricObjectSurface::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);
			return true;
		}

		bool GeometricObject::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "id", id);
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "type", type);
			XMLTools::ReadAttribute(element, "visible", visible);
			XMLTools::ReadAttribute(element, "x", position.x);
			XMLTools::ReadAttribute(element, "y", position.y);
			XMLTools::ReadAttribute(element, "rotation", rotation);
			return true;
		}

		bool GeometricObjectPoint::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			return true;
		}

		bool GeometricObjectRectangle::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;
			return true;
		}

		bool GeometricObjectEllipse::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;
			return true;
		}

		bool GeometricObjectPolygon::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			tinyxml2::XMLElement const * polygon_element = element->FirstChildElement("polygon");
			points = GetPointArray(polygon_element, "points");
			return true;
		}

		bool GeometricObjectPolyline::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			tinyxml2::XMLElement const * polygon_element = element->FirstChildElement("polyline");
			points = GetPointArray(polygon_element, "points");
			return true;
		}

		bool GeometricObjectText::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;

			tinyxml2::XMLElement const * text_element = element->FirstChildElement("text");

			std::pair<char const*, int> const halign_map[] = {
				{ "left", HALIGN_LEFT },
				{ "center", HALIGN_CENTER },
				{ "right", HALIGN_RIGHT },
				{ "justify", HALIGN_JUSTIFY },
				{ nullptr, HALIGN_LEFT }
			};
			XMLTools::ReadEnumAttribute(text_element, "halign", halign_map, halign);

			std::pair<char const*, int> const valign_map[] = {
				{ "top", VALIGN_TOP },
				{ "center", VALIGN_CENTER },
				{ "bottom", VALIGN_BOTTOM },
				{ nullptr, VALIGN_TOP }
			};
			XMLTools::ReadEnumAttribute(text_element, "valign", valign_map, valign);

			XMLTools::ReadAttribute(text_element, "pixelsize", pixelsize);
			XMLTools::ReadAttribute(text_element, "wrap", wrap);
			XMLTools::ReadAttribute(text_element, "fontfamily", fontfamily);
			ReadXMLColor(text_element, "color", color);

			char const * txt = text_element->GetText();
			if (txt != nullptr)
				text = txt;

			return true;
		}

		bool GeometricObjectTile::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;

			int pseudo_gid = 0;// this is a pseudo_gid, because the Vertical & Horizontal flipping is encoded inside this value
			XMLTools::ReadAttribute(element, "gid", pseudo_gid);

			gid = (pseudo_gid & ~((1 << 31) | (1 << 30)));

			horizontal_flip = ((pseudo_gid & (1 << 31)) != 0);
			vertical_flip = ((pseudo_gid & (1 << 30)) != 0);

			return true;
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

			std::string terrain;
			if (XMLTools::ReadAttribute(element, "terrain", terrain))
				if (!ComputeTerrainIndices(terrain.c_str()))
					return false;

			// some tilesets have a single image (representing an atlas with elements in a grid cells)
			// some tilesets have individual images per tiles
			tinyxml2::XMLElement const * image_element = element->FirstChildElement("image");
			if (image_element != nullptr)
			{
				if (!XMLTools::ReadAttribute(image_element, "source", image_path))
					return false;
				if (!XMLTools::ReadAttribute(image_element, "width", image_size.x))
					return false;
				if (!XMLTools::ReadAttribute(image_element, "height", image_size.y))
					return false;

				image_path = BoostTools::FindAbsolutePath(GetOwnerPath(), image_path);
				atlas_key  = chaos::BoostTools::PathToName(image_path);
			}

			DoLoadObjectListHelper(element, object_layers, "objectgroup", nullptr, this);

			return true;
		}

		// "0,,," => top left
		// ",0,," => top right
		// ",,0," => bottom left
		// ",,,0" => bottom right
		bool TileData::ComputeTerrainIndices(char const * str)
		{
			assert(str != nullptr);

			// by default initialization
			for (size_t index = 0; index < 4; ++index)
				terrain_indices[index] = -1;

			// parse the string
			size_t index = 0;

			size_t i = 0;
			while (str[i] != 0)
			{
				if (str[i] >= '0' && str[i] <= '9')
				{
					terrain_indices[index] = atoi(&str[i]);
					i = StringTools::SkipNumber(&str[i]) - str;
					if (index == 4)
						break;
				}
				else
				{
					if (str[i] == ',')
						++index;
					++i;
				}
			}
			return true;
		}

		//
		// ManagerObject methods
		//

		ManagerObject::ManagerObject(BaseObject * in_owner, boost::filesystem::path in_path) :
			PropertyOwner(in_owner),
			path(std::move(in_path))
		{

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

				XMLTools::ReadAttribute(image_source, "source", image_path);
				image_path = BoostTools::FindAbsolutePath(GetOwnerPath(), image_path);
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

		GeometricObject * ObjectLayer::DoLoadOneObject(tinyxml2::XMLElement const * element)
		{
			// tile ?
			int pseudo_gid = 0;
			if (XMLTools::ReadAttribute(element, "gid", pseudo_gid)) // this is a pseudo_gid, because the Vertical & Horizontal flipping is encoded inside this value
				return new GeometricObjectTile(this);

			// ellipse ?
			tinyxml2::XMLElement const * ellipse_element = element->FirstChildElement("ellipse");
			if (ellipse_element != nullptr)
				return new GeometricObjectEllipse(this);

			// text ?
			tinyxml2::XMLElement const * text_element = element->FirstChildElement("text");
			if (text_element != nullptr)
				return new GeometricObjectText(this);

			// polygon ?
			tinyxml2::XMLElement const * polygon_element = element->FirstChildElement("polygon");
			if (polygon_element != nullptr)
				return new GeometricObjectPolygon(this);

			// polyline ?
			tinyxml2::XMLElement const * polyline_element = element->FirstChildElement("polyline");
			if (polyline_element != nullptr)
				return new GeometricObjectPolyline(this);

			// point ?
			tinyxml2::XMLElement const * point_element = element->FirstChildElement("point");
			if (point_element != nullptr)      
				return new GeometricObjectPoint(this);

			// rectangle ?
			return new GeometricObjectRectangle(this);
		}

		bool ObjectLayer::DoLoadObjects(tinyxml2::XMLElement const * element)
		{
			tinyxml2::XMLElement const * e = element->FirstChildElement("object");
			for (; e != nullptr; e = e->NextSiblingElement("object"))
			{
				GeometricObject * object = DoLoadOneObject(e);
				if (object == nullptr)
					continue;
				if (!object->DoLoad(e))
					delete(object);
				else
					geometric_objects.push_back(object);
			}
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

					// TODO

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

		size_t TileLayer::GetNonEmptyTileCount() const
		{
			size_t result = 0;
			for (size_t i = 0 ; i < tile_indices.size() ; ++i)
				if (tile_indices[i] > 0)
					++result;
			return result;
		}

		glm::ivec2 TileLayer::GetTileCoordinate(int index) const
		{
			return glm::ivec2(index % size.x, index / size.x);		
		}

		//
		// TileSet methods
		//

		TileSet::TileSet(BaseObject * in_owner, boost::filesystem::path in_path) :
			ManagerObject(in_owner, in_path)
		{

		}

		TileData * TileSet::FindTileData(int gid)
		{
			size_t count = tiles.size();
			for (size_t i = 0; i < count; ++i)
			{
				TileData * tile_data = tiles[i].get();
				if (tile_data == nullptr)
					continue;
				if (tile_data->id == gid)
					return tile_data;
			}
			return nullptr;
		}

		TileData const * TileSet::FindTileData(int gid) const
		{
			size_t count = tiles.size();
			for (size_t i = 0; i < count; ++i)
			{
				TileData const * tile_data = tiles[i].get();
				if (tile_data == nullptr)
					continue;
				if (tile_data->id == gid)
					return tile_data;
			}
			return nullptr;
		}

		bool TileSet::DoLoadGrounds(tinyxml2::XMLElement const * element)
		{
			return DoLoadObjectListHelper(element, grounds, "terrain", "terraintypes", this);
		}

		bool TileSet::DoLoadTiles(tinyxml2::XMLElement const * element)
		{
			// load the tiles
			if (!DoLoadObjectListHelper(element, tiles, "tile", nullptr, this))
				return false;
			return true;
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
				XMLTools::ReadAttribute(image_element, "source", image_path);
				image_path = BoostTools::FindAbsolutePath(GetOwnerPath(), image_path);

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

		Map::Map(BaseObject * in_owner, boost::filesystem::path in_path) :
			ManagerObject(in_owner, in_path)
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

			// get the manager
			Manager * manager = GetOwner<Manager>();
			if (manager == nullptr)
				return false;

			tinyxml2::XMLElement const * tileset_element = element->FirstChildElement("tileset");
			for (; tileset_element != nullptr; tileset_element = tileset_element->NextSiblingElement("tileset"))
			{
				int first_gid = 0;
				std::string source;

				if (!XMLTools::ReadAttribute(tileset_element, "firstgid", first_gid)) // firstgid is mandatory (map would be incomplete)
					return false;

				// external tileset
				TileSet * tileset = nullptr;

				if (XMLTools::ReadAttribute(tileset_element, "source", source)) // source is mandatory (map would be incomplete)
				{
					boost::filesystem::path tileset_path = BoostTools::FindAbsolutePath(path, source); // compute the path of the tileset relative to this

					tileset = manager->LoadTileSet(tileset_path);
					if (tileset == nullptr)
						return false;
				}
				// embedded titleset
				else
				{
					tileset = new TileSet(this, boost::filesystem::path());
					if (tileset == nullptr)
						return false;

					if (!tileset->DoLoad(tileset_element))
					{
						delete(tileset);
						return false;
					}
					manager->tile_sets.push_back(tileset);
				}

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

		TileInfo Map::FindTileInfo(int gid)
		{
			TileInfo result;
			if (gid >= 0)
			{
				size_t count = tilesets.size();
				for (size_t i = count ; i > 0; --i)
				{
					size_t index = i - 1;

					TileSetData & data = tilesets[index];
					if (gid >= data.first_gid)
					{
						TileData * tiledata = data.tileset->FindTileData(gid - data.first_gid);
						if (tiledata != nullptr)
						{
							result.gid     = (gid - data.first_gid);
							result.tileset = data.tileset.get();
							result.tiledata = tiledata;
							return result;
						}
					}
				}
			}
			return result;
		}

		TileInfo const Map::FindTileInfo(int gid) const
		{
			TileInfo result;
			if (gid >= 0)
			{
				size_t count = tilesets.size();
				for (size_t i = count ; i > 0; --i)
				{
					size_t index = i - 1;

					TileSetData const & data = tilesets[index];
					if (gid >= data.first_gid)
					{
						TileData * tiledata = data.tileset->FindTileData(gid - data.first_gid);
						if (tiledata != nullptr)
						{
							result.gid = (gid - data.first_gid);
							result.tileset = data.tileset.get();
							result.tiledata = tiledata;
							return result;
						}
					}
				}
			}
			return result;
		}

		//
		// Manager methods
		//

		TileSet * Manager::LoadTileSet(FilePathParam const & path)
		{
			TileSet * result = FindTileSet(path);
			if (result != nullptr)
				return result;
			return DoLoadTileSet(path);
		}

		TileSet * Manager::LoadTileSet(FilePathParam const & path, Buffer<char> buffer)
		{
			TileSet * result = FindTileSet(path);
			if (result != nullptr)
				return result;
			return DoLoadTileSet(path, buffer);
		}

		TileSet * Manager::LoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc)
		{
			TileSet * result = FindTileSet(path);
			if (result != nullptr)
				return result;
			return DoLoadTileSet(path, doc);
		}

		Map * Manager::LoadMap(FilePathParam const & path)
		{
			Map * result = FindMap(path);
			if (result != nullptr)
				return result;
			return DoLoadMap(path);
		}

		Map * Manager::LoadMap(FilePathParam const & path, Buffer<char> buffer)
		{
			Map * result = FindMap(path);
			if (result != nullptr)
				return result;
			return DoLoadMap(path, buffer);
		}

		Map * Manager::LoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc)
		{
			Map * result = FindMap(path);
			if (result != nullptr)
				return result;
			return DoLoadMap(path, doc);
		}

		Map * Manager::FindMap(FilePathParam const & path)
		{
			size_t count = maps.size();
			for (size_t i = 0; i < count; ++i)
				if (maps[i]->IsMatchingName(path.GetResolvedPath()))
					return maps[i].get();
			return nullptr;
		}

		Map const * Manager::FindMap(FilePathParam const & path) const
		{
			size_t count = maps.size();
			for (size_t i = 0; i < count; ++i)
				if (maps[i]->IsMatchingName(path.GetResolvedPath()))
					return maps[i].get();
			return nullptr;
		}

		TileSet * Manager::FindTileSet(FilePathParam const & path)
		{
			size_t count = tile_sets.size();
			for (size_t i = 0; i < count; ++i)
				if (tile_sets[i]->IsMatchingName(path.GetResolvedPath()))
					return tile_sets[i].get();
			return nullptr;
		}

		TileSet const * Manager::FindTileSet(FilePathParam const & path) const
		{
			size_t count = tile_sets.size();
			for (size_t i = 0; i < count; ++i)
				if (tile_sets[i]->IsMatchingName(path.GetResolvedPath()))
					return tile_sets[i].get();
			return nullptr;
		}

		TileSet * Manager::DoLoadTileSet(FilePathParam const & path)
		{
			Buffer<char> buffer = FileTools::LoadFile(path, true);
			if (buffer != nullptr)
				return DoLoadTileSet(path, buffer);
			return nullptr;
		}

		TileSet * Manager::DoLoadTileSet(FilePathParam const & path, Buffer<char> buffer)
		{
			TileSet * result = nullptr;

			tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
			if (doc != nullptr)
			{
				tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
				if (error == tinyxml2::XML_SUCCESS)
					result = DoLoadTileSet(path, doc);
				delete(doc);
			}
			return result;

		}

		TileSet * Manager::DoLoadTileSet(FilePathParam const & path, tinyxml2::XMLDocument const * doc)
		{
			assert(doc != nullptr);
			TileSet * result = new TileSet(this, path.GetResolvedPath());
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

		Map * Manager::DoLoadMap(FilePathParam const & path)
		{
			Buffer<char> buffer = FileTools::LoadFile(path, true);
			if (buffer != nullptr)
				return DoLoadMap(path, buffer);
			return nullptr;
		}

		Map * Manager::DoLoadMap(FilePathParam const & path, Buffer<char> buffer)
		{
			Map * result = nullptr;

			tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();
			if (doc != nullptr)
			{
				tinyxml2::XMLError error = doc->Parse(buffer.data, buffer.bufsize);
				if (error == tinyxml2::XML_SUCCESS)
					result = DoLoadMap(path, doc);
				delete(doc);
			}
			return result;
		}

		Map * Manager::DoLoadMap(FilePathParam const & path, tinyxml2::XMLDocument const * doc)
		{
			Map * result = new Map(this, path.GetResolvedPath());
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
