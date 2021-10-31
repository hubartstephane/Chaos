#include <chaos/Chaos.h>

namespace chaos
{
	namespace TiledMap
	{
		// ==========================================
		// Utility methods
		// ==========================================

		int DecodeTileGID(int pseudo_gid, int* particle_flags)
		{
			// see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/
			const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
			const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
			const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

			std::int32_t tmp = (std::int32_t)pseudo_gid;

			std::int32_t gid = tmp & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

			if (particle_flags != nullptr)
			{
				*particle_flags = 0;
				if ((tmp & FLIPPED_HORIZONTALLY_FLAG) != 0)
					*particle_flags |= ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
				if ((tmp & FLIPPED_VERTICALLY_FLAG) != 0)
					*particle_flags |= ParticleFlags::TEXTURE_VERTICAL_FLIP;
				if ((tmp & FLIPPED_DIAGONALLY_FLAG) != 0)
					*particle_flags |= ParticleFlags::TEXTURE_DIAGONAL_FLIP;
			}

			return int(gid);
		}

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

		bool ReadXMLColor(std::string const& str, glm::vec4& result)
		{
			char const* c = str.c_str(); // #00000000 is a valid attribute too, as well as hexadecimal strings
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

		bool ReadXMLColor(tinyxml2::XMLElement const* element, char const* attribute_name, glm::vec4& result)
		{
			std::string result_string;
			if (!XMLTools::ReadAttribute(element, attribute_name, result_string))
				return false;

			return ReadXMLColor(result_string, result);
		}

		// ==========================================
		// BaseObject methods
		// ==========================================

		BaseObject::BaseObject(BaseObject * in_owner) :
			owner(in_owner)
		{
		}

		Manager * BaseObject::GetManager()
		{
			return GetOwner<Manager>(true);
		}

		Manager const * BaseObject::GetManager() const
		{
			return GetOwner<Manager>(true);
		}

		Map * BaseObject::GetMap()
		{
			return GetOwner<Map>(true);
		}

		Map const * BaseObject::GetMap() const
		{
			return GetOwner<Map>(true);
		}

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

		bool BaseObject::DoLoadLayersImpl(tinyxml2::XMLElement const* element, std::vector<shared_ptr<LayerBase>>& result)
		{
			for (tinyxml2::XMLElement const* e = element->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
			{
				char const* child_name = e->Name();
				if (StringTools::Stricmp(child_name, "imagelayer") == 0)
					DoLoadObjectAndInsertInList<ImageLayer>(e, result, this);
				else if (StringTools::Stricmp(child_name, "objectgroup") == 0)
					DoLoadObjectAndInsertInList<ObjectLayer>(e, result, this);
				else if (StringTools::Stricmp(child_name, "group") == 0)
					DoLoadObjectAndInsertInList<GroupLayer>(e, result, this);
				else if (StringTools::Stricmp(child_name, "layer") == 0)
				{
					Map const* map = GetMap();
					if (map != nullptr)
						DoLoadObjectAndInsertInList<TileLayer>(e, result, this, map->tile_size);
				}
			}
			return true;
		}

	};  // namespace TiledMap

}; // namespace chaos
