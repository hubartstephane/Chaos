#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	namespace TiledMap
	{
		static std::vector<std::pair<HorizontalTextAlignment, char const*>> const halign_map = {
			{ HorizontalTextAlignment::LEFT, "left" }, // default
			{ HorizontalTextAlignment::CENTER, "center" },
			{ HorizontalTextAlignment::RIGHT, "right" },
			{ HorizontalTextAlignment::JUSTIFY, "justify" }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(HorizontalTextAlignment, halign_map);

		static std::vector<std::pair<VerticalTextAlignment, char const*>> const valign_map = {
			{ VerticalTextAlignment::TOP, "top" }, // default
			{ VerticalTextAlignment::CENTER, "center" },
			{ VerticalTextAlignment::BOTTOM, "bottom" }
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(VerticalTextAlignment, valign_map);

		// ==========================================
		// GeometricObject methods
		// ==========================================

		bool GeometricObject::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!TypedObject::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "id", id);
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "visible", visible);
			XMLTools::ReadAttribute(element, "x", position.x);
			XMLTools::ReadAttribute(element, "y", position.y);
			XMLTools::ReadAttribute(element, "rotation", rotation); // clockwise rotation in degree

			// convert to trigonometric rotation in rads
			rotation = -MathTools::DegreeToRadian(rotation);

			// remove useless space in type
			if (type.length() > 0)
				type = StringTools::TrimSpaces(type.c_str(), true, true);

			// reverse the Y axis
			position = position * REVERSE_Y_AXIS;

			return true;
		}

		box2 GeometricObject::GetBoundingBox(bool world_system) const
		{
			box2 result = DoGetBoundingBox();
			if (world_system)
			{
				LayerBase const* parent_layer = auto_cast(owner);
				while (parent_layer != nullptr)
				{
					result.position += parent_layer->offset;
					parent_layer = auto_cast(parent_layer->owner);
				}
			}
			return result;
		}

		box2 GeometricObject::DoGetBoundingBox() const
		{
			box2 result;
			result.position = position;
			result.half_size = glm::vec2(0.0f, 0.0f);
			return result;
		}

		// ==========================================
		// GeometricObjectSurface methods
		// ==========================================

		bool GeometricObjectSurface::DoLoad(tinyxml2::XMLElement const* element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);
			return true;
		}

		box2 GeometricObjectSurface::DoGetBoundingBox() const
		{
			// TOP-LEFT
			glm::vec2 p1 = position;
			glm::vec2 p2 = position + size * REVERSE_Y_AXIS;
			return box2(std::make_pair(p1, p2));
		}

		// ==========================================
		// GeometricObjectPoint methods
		// ==========================================

		bool GeometricObjectPoint::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			return true;
		}

		// ==========================================
		// GeometricObjectRectangle methods
		// ==========================================

		bool GeometricObjectRectangle::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;
			return true;
		}

		// ==========================================
		// GeometricObjectEllipse methods
		// ==========================================

		bool GeometricObjectEllipse::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;
			return true;
		}

		// ==========================================
		// Utility methods
		// ==========================================

		static std::vector<glm::vec2> GetPointArray(tinyxml2::XMLElement const * element, char const * attribute_name, glm::vec2 & position, glm::vec2 & size)
		{
			std::vector<glm::vec2> result;

			// read the points
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
							result.push_back(p * REVERSE_Y_AXIS);
							p = glm::vec2(0.0f, 0.0f); // flush the point if both axis are found
							coord = 0;
						}

						while (values[i] != 0 && values[i] != ',' && values[i] != ' ') // skip until separator
							++i;
						if (values[i] == ',' || values[i] == ' ')
							++i;
					}
					// flush the point even if there is a missing axis
					if (coord == 1)
						result.push_back(p * REVERSE_Y_AXIS);
				}
			}
			// extract the bounding box, correct the points
			if (result.size() > 0)
			{

				// shu49. Ca vaut le coup d externaliser ce bout de code non ?


				glm::vec2 min_position = result[0];
				glm::vec2 max_position = result[0];
				for (glm::vec2 const& p : result)
				{
					min_position = glm::min(min_position, p);
					max_position = glm::max(max_position, p);
				}			




				glm::vec2 center = (max_position + min_position) / 2.0f;
				for (glm::vec2 & p : result)
					p -= center;

				// correct the position & compute bounding box
				position += center;
				size = max_position - min_position;
			}
			return result;
		}

		// ==========================================
		// GeometricObjectPolygon methods
		// ==========================================

		bool GeometricObjectPolygon::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			tinyxml2::XMLElement const * polygon_element = element->FirstChildElement("polygon");
			points = GetPointArray(polygon_element, "points", position, size);

			// shu49 update bounding box et points ici


			return true;
		}

		box2 GeometricObjectPolygon::DoGetBoundingBox() const
		{
			box2 result;
			result.position = position;
			result.half_size = size * 0.5f;
			return result;
		}

		// ==========================================
		// GeometricObjectPolyline methods
		// ==========================================

		bool GeometricObjectPolyline::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObject::DoLoad(element))
				return false;
			tinyxml2::XMLElement const * polygon_element = element->FirstChildElement("polyline");
			points = GetPointArray(polygon_element, "points", position, size);

			// shu49 update bounding box et points ici


			return true;
		}

		box2 GeometricObjectPolyline::DoGetBoundingBox() const
		{
			box2 result;
			result.position = position;
			result.half_size = size * 0.5f;
			return result;
		}

		// ==========================================
		// GeometricObjectText methods
		// ==========================================

		bool GeometricObjectText::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;

			tinyxml2::XMLElement const * text_element = element->FirstChildElement("text");

			halign = HorizontalTextAlignment::LEFT;
			XMLTools::ReadAttribute(text_element, "halign", halign);
			valign = VerticalTextAlignment::TOP;
			XMLTools::ReadAttribute(text_element, "valign", valign);
			XMLTools::ReadAttribute(text_element, "pixelsize", pixelsize);
			XMLTools::ReadAttribute(text_element, "wrap", wrap);
			XMLTools::ReadAttribute(text_element, "fontfamily", fontfamily);
			ReadXMLColor(text_element, "color", color);

			char const * txt = text_element->GetText();
			if (txt != nullptr)
				text = txt;

			return true;
		}

		// ==========================================
		// GeometricObjectTile methods
		// ==========================================

		bool GeometricObjectTile::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!GeometricObjectSurface::DoLoad(element))
				return false;

			// this is a pseudo_gid, because the Vertical & Horizontal flipping is encoded inside this value
			int pseudo_gid = 0;
			XMLTools::ReadAttribute(element, "gid", pseudo_gid);
			// decode the ID, extract the flags
			gid = DecodeTileGID(pseudo_gid, &particle_flags);

			return true;
		}

		box2 GeometricObjectTile::DoGetBoundingBox() const
		{
			// search the alignment for this tile
			Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT; // default value for tileset

			TileInfo tile_info = FindTileInfo();
			if (tile_info.tileset != nullptr)
				hotpoint = tile_info.tileset->object_alignment;

			// search 2 points
			glm::vec2 p1 = ConvertHotpointToBottomLeft(position, size, hotpoint);
			glm::vec2 p2 = p1 + glm::vec2(size.x, -size.y) * REVERSE_Y_AXIS;

			return box2(std::make_pair(p1, p2));
		}

		TileInfo GeometricObjectTile::FindTileInfo() const
		{
			Map const * tiled_map = GetMap();
			if (tiled_map != nullptr)
				return tiled_map->FindTileInfo(gid);
			return TileInfo();
		}

		bool GeometricObjectTile::IsObjectOfType(char const* in_type) const
		{
			if (GeometricObjectSurface::IsObjectOfType(in_type))
				return true;

			TileInfo tile_info = FindTileInfo();
			if (tile_info.tiledata != nullptr)
				if (tile_info.tiledata->IsObjectOfType(in_type))
					return true;
			return false;
		}

		// XXX : for a ObjectTile, a property may be localized in 4 places
		//
		//       -the PROPERTY_MAP of the object
		//       -the TYPE1 of the object
		//       -the TILE_DATA 
		//           -PROPERTY_MAP 
		//           -TYPE2 (that may differ from TYPE1)
		//
		//       the PROPERTY_MAP of the object is obviously the very first place where to look at for a property (while it is the finer customization we can do)
		//
		//       we can say that TILE_DATA.PROPERTY_MAP is to be searched before TILE_DATA.TYPE2 (obvious)
		//

		Property const* GeometricObjectTile::FindProperty(char const* name, PropertyType type_id) const
		{
			// 1 - Own properties
			Property const* result = PropertyOwner::FindProperty(name, type_id);
			if (result != nullptr)
				return result;

			// 2 - See Tile properties
			TileInfo tile_info = FindTileInfo();
			if (tile_info.tiledata != nullptr)
			{
				result = tile_info.tiledata->FindProperty(name, type_id);
				if (result != nullptr)
					return result;
			}

			// 3 - See our own type
			if (!StringTools::IsEmpty(type))
			{
				if (tile_info.tiledata == nullptr || StringTools::Stricmp(type, tile_info.tiledata->type) != 0) // if TYPE1 == TYPE2, do not search twice
				{
					Manager const * manager = GetManager();
					if (manager != nullptr)
					{
						result = manager->FindObjectProperty(type.c_str(), name, type_id);
						if (result != nullptr)
							return result;
					}
				}
			}
			return result;
		}

	};  // namespace TiledMap

}; // namespace chaos
