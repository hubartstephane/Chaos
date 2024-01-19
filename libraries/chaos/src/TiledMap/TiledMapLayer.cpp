#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace TiledMap
	{
		static chaos::EnumTools::EnumMetaData<DrawOrder> const DrawOrder_metadata = {
			{ DrawOrder::MANUAL, "index"  },
			{ DrawOrder::TOPDOWN, "topdown" } // default
		};

		CHAOS_IMPLEMENT_ENUM_METHOD(DrawOrder, DrawOrder_metadata, CHAOS_API);

		// ==========================================
		// LayerBase methods
		// ==========================================

		bool LayerBase::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!PropertyOwner::DoLoad(element))
				return false;
			XMLTools::ReadAttribute(element, "id", id);
			XMLTools::ReadAttribute(element, "name", name);
			XMLTools::ReadAttribute(element, "visible", visible);
			XMLTools::ReadAttribute(element, "locked", locked);
			XMLTools::ReadAttribute(element, "opacity", opacity);
			XMLTools::ReadAttribute(element, "offsetx", offset.x);
			XMLTools::ReadAttribute(element, "offsety", offset.y);
			XMLTools::ReadAttribute(element, "parallaxx", parallax_factor.x);
			XMLTools::ReadAttribute(element, "parallaxy", parallax_factor.y);
			// reverse the Y axis
			offset = offset * REVERSE_Y_AXIS;
			return true;
		}

		Property const* LayerBase::FindProperty(char const* name, PropertyType type_id) const
		{
			// super method
			Property const  * result = PropertyOwner::FindProperty(name, type_id);
			if (result != nullptr)
				return result;
			// search recursively in containing layers
			if (owner != nullptr)
			{
				LayerBase const* parent_layer = owner->GetOwner<LayerBase>(true);
				if (parent_layer != nullptr)
					return parent_layer->FindProperty(name, type_id);
			}
			return nullptr;
		}

		// ==========================================
		// ImageLayer methods
		// ==========================================

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
				image_path = PathTools::FindAbsolutePath(GetOwnerPath(), image_path);
			}
			return true;
		}

		// ==========================================
		// LayerBase methods
		// ==========================================

		bool ObjectLayer::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!LayerBase::DoLoad(element))
				return false;

			ReadXMLColor(element, "color", color);

			draw_order = DrawOrder::TOPDOWN;
			XMLTools::ReadAttribute(element, "draworder", draw_order);
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
				{
					delete(object);
					continue;
				}
				geometric_objects.push_back(object);
			}
			return true;
		}

		// ==========================================
		// TileLayerChunk methods
		// ==========================================

		Tile TileLayerChunk::GetTile(glm::ivec2 pos) const // pos is expressed in local coordinate
		{
			assert(ContainTile(pos));
			pos = pos - offset;
			return tile_indices[(size_t)(pos.x + pos.y * size.x)];
		}

		bool TileLayerChunk::ContainTile(glm::ivec2 const& pos) const
		{
			if ((pos.x >= offset.x) && (pos.x < offset.x + size.x) && (pos.y >= offset.y) && (pos.y < offset.y + size.y))
				return true;
			return false;
		}

		// ==========================================
		// TileLayer methods
		// ==========================================

		// +------+------+------+------+------+------+
		// |      |      |      |      |      |      |
		// | 0, 0 |      |      |      |      |      |
		// |      |      |      |      |      |      |
		// x------+------+------+------+------+------+
		// ^
		// |
		// +-- for tile (0, 0), by DEFAULT, the BOTTOM LEFT corner of the particle should be there (see object_alignment for some tweak)


		box2 TileLayer::GetTileBoundingBox(glm::ivec2 const tile_coord, glm::vec2 const & image_size, int particle_flags, bool world_system) const
		{
			glm::vec2 tc = auto_cast_vector(tile_coord);
			glm::vec2 ts = auto_cast_vector(tile_size);

			glm::vec2 p1 = (tc * ts * REVERSE_Y_AXIS) + (ts * glm::vec2(0.0f, 1.0f) * REVERSE_Y_AXIS); // BOTTOM-LEFT ... x (0, 1) because we just want to keep the Y component for this offset

			glm::vec2 p2;
			if ((particle_flags & ParticleFlags::TEXTURE_DIAGONAL_FLIP) == 0)
				p2 = p1 + glm::vec2(image_size.x, -image_size.y) * REVERSE_Y_AXIS;
			else
				p2 = p1 + glm::vec2(image_size.y, -image_size.x) * REVERSE_Y_AXIS;

			box2 result = box2(std::make_pair(p1, p2));
			if (world_system)
				result.position += offset;
			return result;
		}

		bool TileLayer::DoLoad(tinyxml2::XMLElement const * element)
		{
			if (!LayerBase::DoLoad(element))
				return false;
			// some attributes
			XMLTools::ReadAttribute(element, "width", size.x);
			XMLTools::ReadAttribute(element, "height", size.y);
			// load tiles
			if (!DoLoadTileBuffer(element))
				return false;
			// compute additionnal tile flags
			ComputeTileFlags();
			return true;
		}

		void TileLayer::ComputeTileFlags()
		{
			// get all TileProcessor
			std::string processors = GetPropertyValueString("TILE_FLAG_PROCESSORS", "");
			if (processors.length() > 0)
			{
				// split into array, remove space characters
				std::vector<std::string> processor_names;
				NameFilter::AddNames(processors.c_str(), processor_names);
				// run all processors
				for (std::string const& name : processor_names)
				{
					// get the processor class
					SubClassOf<TileFlagProcessor> processor_class = ClassManager::GetDefaultInstance()->FindClass(name.c_str());
					if (processor_class)
					{
						// create the processor and run it
						shared_ptr<TileFlagProcessor> processor = processor_class.CreateInstance();
						if (processor != nullptr)
						{
							processor->Process(this);
						}
					}
				}
			}
		}

		std::vector<Tile> TileLayer::DoLoadTileChunkFromBuffer(Buffer<char> const & buffer, glm::ivec2 const & chunk_size)
		{
			size_t count = (size_t)(chunk_size.x * chunk_size.y);

			if (buffer.bufsize != count * sizeof(uint32_t))  // array width * height * sizeof(uint32)
				return {};

			std::vector<Tile> result;
			result.reserve(count);

			// transform the char buffer into tiles
			for (size_t i = 0; i < count; ++i)
			{
				unsigned int a = (unsigned int)buffer[i * 4 + 0];
				unsigned int b = (unsigned int)buffer[i * 4 + 1];
				unsigned int c = (unsigned int)buffer[i * 4 + 2];
				unsigned int d = (unsigned int)buffer[i * 4 + 3];

				// do not decode yet the ID and the flags
				unsigned int pseudo_id = (a << 0) | (b << 8) | (c << 16) | (d << 24);
				result.push_back({ *(int *)&pseudo_id, 0 });
			}
			return result;
		}

		bool TileLayer::DoLoadTileChunk(tinyxml2::XMLElement const* element, char const* encoding, char const* compression)
		{
			if (element == nullptr)
				return true;

			// read the data
			std::vector<Tile> tiles;

			glm::ivec2 chunk_size = size;
			XMLTools::ReadAttribute(element, "width", chunk_size.x); // for non infinite layer, the size of the chunk is the size of the map
			XMLTools::ReadAttribute(element, "height", chunk_size.y);

			glm::ivec2 chunk_offset = glm::ivec2(0, 0);
			XMLTools::ReadAttribute(element, "x", chunk_offset.x);
			XMLTools::ReadAttribute(element, "y", chunk_offset.y);

			if (StringTools::Stricmp(encoding, "base64") == 0)
			{
				char const* txt = element->GetText();
				if (txt == nullptr)
					return true;

				if (StringTools::Stricmp(compression, "gzip") == 0)
				{
					assert(0);

					// TODO

				}
				else if (StringTools::Stricmp(compression, "zstd") == 0)
				{
					assert(0);

					// TODO

				}
				else if (StringTools::Stricmp(compression, "zlib") == 0)
				{
					std::string content = StringTools::TrimBase64(txt);

					Buffer<char> base64 = MyBase64().Decode(content.c_str());
					Buffer<char> decoded = MyZLib().Decode(base64);

					tiles = DoLoadTileChunkFromBuffer(decoded, chunk_size);
				}
				else // no encoding
				{
					std::string content = StringTools::TrimBase64(txt);

					Buffer<char> base64 = MyBase64().Decode(content.c_str());

					tiles = DoLoadTileChunkFromBuffer(base64, chunk_size);
				}
			}
			else if (StringTools::Stricmp(encoding, "csv") == 0)
			{

				tiles.reserve(chunk_size.x * chunk_size.y);

				// XXX : for tiles that have any flags set, we have to work with UNSIGNED int

				char const* txt = element->GetText();
				if (txt == nullptr)
					return true;

				size_t count = (size_t)(chunk_size.x * chunk_size.y);

				int i = 0;
				while (txt[i] != 0 && tiles.size() != count)
				{
					while (txt[i] != 0 && !isdigit(txt[i])) // search first figure
						++i;
					if (txt[i] == 0)
						break;

					unsigned int pseudo_id = strtoul(&txt[i], nullptr, 10);
					tiles.push_back({ *(int*)&pseudo_id, 0 }); // do not decode the ID and the flags

					while (txt[i] != 0 && isdigit(txt[i])) // skip all figures
						++i;
				}
			}
			else // else XML
			{
				tiles.reserve(chunk_size.x * chunk_size.y);

				size_t count = (size_t)(chunk_size.x * chunk_size.y);

				tinyxml2::XMLElement const* child = element->FirstChildElement("tile");
				while (child != nullptr && tiles.size() != count)
				{
					unsigned int pseudo_id = 0;
					XMLTools::ReadAttribute(child, "gid", pseudo_id);

					tiles.push_back({ *((int *)&pseudo_id), 0 }); // do not decode yet the ID and the flags

					child = child->NextSiblingElement("tile");
				}
			}


			if (tiles.size() != 0)
			{
				// decode the ID's and the flags
				for (Tile& tile : tiles)
					tile.gid = DecodeTileGID(tile.gid, &tile.flags);
				// insert a chunk for theses tiles
				TileLayerChunk chunk;
				chunk.size = chunk_size;
				chunk.offset = chunk_offset;
				chunk.tile_indices = std::move(tiles);
				tile_chunks.push_back(std::move(chunk));
			}
			return true;
		}

		bool TileLayer::DoLoadTileBuffer(tinyxml2::XMLElement const * element)
		{
			tinyxml2::XMLElement const * data = element->FirstChildElement("data");
			if (data == nullptr)
				return false;

			std::string encoding;
			XMLTools::ReadAttribute(data, "encoding", encoding);

			std::string compression;
			XMLTools::ReadAttribute(data, "compression", compression);

			// for non infinite layer
			DoLoadTileChunk(data, encoding.c_str(), compression.c_str());
			// for infinite layer
			tinyxml2::XMLElement const* chunk = data->FirstChildElement("chunk");
			while (chunk != nullptr)
			{
				DoLoadTileChunk(chunk, encoding.c_str(), compression.c_str());
				chunk = chunk->NextSiblingElement("chunk");
			}

			return true;
		}

		glm::ivec2 TileLayer::GetTileCoordinate(TileLayerChunk const & chunk, size_t index) const
		{
			int tmp = int(index);
			return chunk.offset + glm::ivec2(tmp % chunk.size.x, tmp / chunk.size.x);
		}

		TileLayerChunk const* TileLayer::GetTileChunk(glm::ivec2 const& pos) const
		{
			for (TileLayerChunk const& chunk : tile_chunks)
				if (chunk.ContainTile(pos))
					return &chunk;
			return nullptr;
		}


		Tile TileLayer::GetTile(glm::ivec2 const& pos) const
		{
			// get the chunk containing the position
			TileLayerChunk const * chunk = GetTileChunk(pos);
			if (chunk == nullptr)
				return {};
			// get the ID at the given position
			return chunk->GetTile(pos);
		}

		// ==========================================
		// GroupLayer methods
		// ==========================================

		bool GroupLayer::DoLoad(tinyxml2::XMLElement const* element)
		{
			if (!LayerBase::DoLoad(element))
				return false;
			if (!DoLoadLayersImpl(element, layers))
				return false;
			return true;
		}

	};  // namespace TiledMap

}; // namespace chaos
