#include <chaos/BitmapAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	namespace BitmapAtlas
	{
		// ========================================================================
		// BitmapSet functions
		// ========================================================================

		BitmapEntry const * BitmapSet::GetEntry(char const * name) const
		{
			for (auto const & element : elements)
				if (element.name == name)
					return &element;
			return nullptr;
		}

		BitmapEntry const * BitmapSet::GetEntry(TagType tag) const
		{
			for (auto const & element : elements)
				if (element.tag == tag)
					return &element;
			return nullptr;
		}

		// ========================================================================
		// CharacterSet functions
		// ========================================================================

		CharacterEntry const * CharacterSet::GetEntry(char const * name) const
		{
			for (auto const & element : elements)
				if (element.name == name)
					return &element;
			return nullptr;
		}

		CharacterEntry const * CharacterSet::GetEntry(TagType tag) const
		{
			for (auto const & element : elements)
				if (element.tag == tag)
					return &element;
			return nullptr;
		}

		// ========================================================================
		// JSON functions
		// ========================================================================

		template<typename T>
		void SaveIntoJSON(std::vector<T> const & elements, nlohmann::json & json_entries)
		{
			for (auto const & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(element, json_entry);
				json_entries.push_back(json_entry);
			}
		}

		template<typename T>
		void SaveIntoJSON(std::vector<std::unique_ptr<T>> const & elements, nlohmann::json & json_entries)
		{
			for (auto & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(*element.get(), json_entry);
				json_entries.push_back(json_entry);
			}
		}

		template<typename T>
		void SaveIntoJSON(std::vector<T*> const & elements, nlohmann::json & json_entries)
		{
			for (auto const * element : elements)
			{
				if (element == nullptr)
					continue;
				auto json_entry = nlohmann::json();
				SaveIntoJSON(*element, json_entry);
				json_entries.push_back(json_entry);
			}
		}

		template<typename T>
		void LoadFromJSON(std::vector<T> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T element;
				LoadFromJSON(element, json_entry);
				elements.push_back(element);
			}
		}

		template<typename T>
		void LoadFromJSON(std::vector<std::unique_ptr<T>> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				std::unique_ptr<T> element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(*element, json_entry);
				elements.push_back(std::move(element));
			}
		}

		template<typename T>
		void LoadFromJSON(std::vector<T*> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				std::unique_ptr<T> element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(*element, json_entry);
				elements.push_back(element);
			}
		}

		void SaveIntoJSON(NamedObject const & entry, nlohmann::json & json_entry)
		{
			json_entry["name"] = entry.name;
			json_entry["tag"] = entry.tag;
		}

		void LoadFromJSON(NamedObject & entry, nlohmann::json const & json_entry)
		{
			JSONTools::GetAttribute(json_entry, "name", entry.name, "");
			JSONTools::GetAttribute(json_entry, "tag", entry.tag, 0);
		}

		void SaveIntoJSON(BitmapEntry const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["bitmap_index"] = entry.bitmap_index;
			json_entry["x"] = entry.x;
			json_entry["y"] = entry.y;
			json_entry["width"] = entry.width;
			json_entry["height"] = entry.height;
		}

		void LoadFromJSON(BitmapEntry & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method


			JSONTools::GetAttribute(json_entry, "bitmap_index", entry.bitmap_index, 0);
			JSONTools::GetAttribute(json_entry, "x", entry.x, 0);
			JSONTools::GetAttribute(json_entry, "y", entry.y, 0);
			JSONTools::GetAttribute(json_entry, "width", entry.width, 0);
			JSONTools::GetAttribute(json_entry, "height", entry.height, 0);
		}

		void SaveIntoJSON(CharacterEntry const & entry, nlohmann::json & json_entry)
		{
			BitmapEntry const & bitmap_entry = entry;
			SaveIntoJSON(bitmap_entry, json_entry); // call 'super' method

			json_entry["advance_x"] = entry.advance.x;
			json_entry["advance_y"] = entry.advance.y;
			json_entry["bitmap_left"] = entry.bitmap_left;
			json_entry["bitmap_top"] = entry.bitmap_top;
		}

		void LoadFromJSON(CharacterEntry & entry, nlohmann::json const & json_entry)
		{
			BitmapEntry & bitmap_entry = entry;
			LoadFromJSON(bitmap_entry, json_entry); // call 'super' method

			JSONTools::GetAttribute(json_entry, "advance_x", entry.advance.x, 0);
			JSONTools::GetAttribute(json_entry, "advance_y", entry.advance.y, 0);
			JSONTools::GetAttribute(json_entry, "bitmap_left", entry.bitmap_left, 0);
			JSONTools::GetAttribute(json_entry, "bitmap_top", entry.bitmap_top, 0);
		}

		void SaveIntoJSON(BitmapSet const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["elements"] = nlohmann::json::array();
			SaveIntoJSON(entry.elements, json_entry["elements"]);
		}

		void LoadFromJSON(BitmapSet & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method

			LoadFromJSON(entry.elements, json_entry["elements"]);
		}

		void SaveIntoJSON(CharacterSet const & entry, nlohmann::json & json_entry)
		{
			NamedObject const & named_entry = entry;
			SaveIntoJSON(named_entry, json_entry); // call 'super' method

			json_entry["max_character_width"] = entry.max_character_width;
			json_entry["max_character_height"] = entry.max_character_height;
			json_entry["ascender"] = entry.ascender;
			json_entry["descender"] = entry.descender;
			json_entry["face_height"] = entry.face_height;
			json_entry["elements"] = nlohmann::json::array();
			SaveIntoJSON(entry.elements, json_entry["elements"]);
		}

		void LoadFromJSON(CharacterSet & entry, nlohmann::json const & json_entry)
		{
			NamedObject & named_entry = entry;
			LoadFromJSON(named_entry, json_entry); // call 'super' method

			JSONTools::GetAttribute(json_entry, "max_character_width", entry.max_character_width, 0);
			JSONTools::GetAttribute(json_entry, "max_character_height", entry.max_character_height, 0);
			JSONTools::GetAttribute(json_entry, "ascender", entry.ascender, 0);
			JSONTools::GetAttribute(json_entry, "descender", entry.descender, 0);
			JSONTools::GetAttribute(json_entry, "face_height", entry.face_height, 0);

			LoadFromJSON(entry.elements, json_entry["elements"]);
		}

		// ========================================================================
		// Atlas functions
		// ========================================================================

		void AtlasBase::Clear()
		{
			// reset members
			atlas_count = 0;
			dimension = glm::ivec2(0, 0);
			// destroy the bitmap sets
			bitmap_sets.clear();
			// destroy the character sets
			character_sets.clear();
		}

		BitmapSet const * AtlasBase::GetBitmapSet(char const * name) const
		{
			for (auto & bitmap_set_ptr : bitmap_sets)
			{
				BitmapSet const * bitmap_set = bitmap_set_ptr.get();
				if (bitmap_set->name == name)
					return bitmap_set;
			}
			return nullptr;
		}

		BitmapSet const * AtlasBase::GetBitmapSet(TagType tag) const
		{
			for (auto & bitmap_set_ptr : bitmap_sets)
			{
				BitmapSet const * bitmap_set = bitmap_set_ptr.get();
				if (bitmap_set->tag == tag)
					return bitmap_set;
			}
			return nullptr;
		}

		CharacterSet const * AtlasBase::GetCharacterSet(char const * name) const
		{
			for (auto & character_set_ptr : character_sets)
			{
				CharacterSet const * character_set = character_set_ptr.get();
				if (character_set->name == name)
					return character_set;
			}
			return nullptr;
		}

		CharacterSet const * AtlasBase::GetCharacterSet(TagType tag) const
		{
			for (auto & character_set_ptr : character_sets)
			{
				CharacterSet const * character_set = character_set_ptr.get();
				if (character_set->tag == tag)
					return character_set;
			}
			return nullptr;
		}

		float AtlasBase::ComputeSurface(int bitmap_index) const
		{
			float result = 0.0f;
			// surface for the bitmap sets
			for (auto & bitmap_set_ptr : bitmap_sets)
			{
				BitmapSet const * bitmap_set = bitmap_set_ptr.get();
				if (bitmap_set != nullptr)
					for (BitmapEntry const & entry : bitmap_set->elements)
						if (entry.bitmap_index == bitmap_index || bitmap_index < 0)
							result += (float)(entry.width * entry.height);
			}
			// surface for character sets
			for (auto & character_set_ptr : character_sets)
			{
				CharacterSet const * character_set = character_set_ptr.get();
				if (character_set != nullptr)
					for (CharacterEntry const & entry : character_set->elements)
						if (entry.bitmap_index == bitmap_index || bitmap_index < 0)
							result += (float)(entry.width * entry.height);
			}

			return result;
		}

		void AtlasBase::OutputInfo(std::ostream & stream) const
		{
			// info for bitmap sets
			for (auto & bitmap_set_ptr : bitmap_sets)
			{
				BitmapSet const * bitmap_set = bitmap_set_ptr.get();
				if (bitmap_set != nullptr)
					for (BitmapEntry const & entry : bitmap_set->elements)
						OutputInfo(entry, stream);
			}
			// info for character sets
			for (auto & character_set_ptr : character_sets)
			{
				CharacterSet const * character_set = character_set_ptr.get();
				if (character_set != nullptr)
					for (CharacterEntry const & entry : character_set->elements)
						OutputInfo(entry, stream);
			}
		}

		void AtlasBase::OutputInfo(NamedObject const & entry, std::ostream & stream)
		{
			stream << "  name         : " << entry.name << std::endl;
			stream << "  tag          : " << entry.tag << std::endl;
		}

		void AtlasBase::OutputInfo(BitmapEntry const & entry, std::ostream & stream)
		{
			NamedObject const & named_entry = entry;
			OutputInfo(named_entry, stream);

			stream << "  bitmap_index : " << entry.bitmap_index << std::endl;
			stream << "  width        : " << entry.width << std::endl;
			stream << "  height       : " << entry.height << std::endl;
			stream << "  x            : " << entry.x << std::endl;
			stream << "  y            : " << entry.y << std::endl;
		}

		void AtlasBase::OutputInfo(CharacterEntry const & entry, std::ostream & stream)
		{
			BitmapEntry const & bitmap_entry = entry;
			OutputInfo(bitmap_entry, stream);

			stream << "  advance.x    : " << entry.advance.x << std::endl;
			stream << "  advance.y    : " << entry.advance.y << std::endl;
			stream << "  bitmap_left  : " << entry.bitmap_left << std::endl;
			stream << "  bitmap_top   : " << entry.bitmap_top << std::endl;
		}

		std::string AtlasBase::GetInfoString() const
		{
			std::ostringstream out;
			OutputInfo(out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(NamedObject const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(BitmapEntry const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(CharacterEntry const & entry)
		{
			std::ostringstream out;
			OutputInfo(entry, out);
			return out.str();
		}

		std::string AtlasBase::GetAtlasSpaceOccupationString(int bitmap_index) const
		{
			std::ostringstream stream;
			OutputAtlasSpaceOccupation(bitmap_index, stream);
			return stream.str();
		}

		std::string AtlasBase::GetAtlasSpaceOccupationString() const
		{
			std::ostringstream stream;
			OutputAtlasSpaceOccupation(stream);
			return stream.str();
		}

		void AtlasBase::OutputAtlasSpaceOccupation(std::ostream & stream) const
		{
			size_t count = atlas_count;
			for (size_t i = 0; i < count; ++i)
				OutputAtlasSpaceOccupation(i, stream);
		}

		void AtlasBase::OutputAtlasSpaceOccupation(int bitmap_index, std::ostream & stream) const
		{
			float atlas_surface = (float)(dimension.x * dimension.y);
			float atlas_used_surface = ComputeSurface(bitmap_index);
			float percent = 100.0f * atlas_used_surface / atlas_surface;

			stream << "Atlas " << bitmap_index << std::endl;
			stream << "  occupation : " << percent << "%" << std::endl;
		}

		void AtlasBase::OutputGeneralInformation(std::ostream & stream) const
		{
			float atlas_surface = (float)(dimension.x * dimension.y);
			float full_surface = ComputeSurface(-1);
			int   min_atlas_count = (int)std::ceil(full_surface / atlas_surface);

			stream << "Full used surface  : " << full_surface << std::endl;
			stream << "Atlas surface      : " << atlas_surface << std::endl;
			stream << "Best atlas count   : " << min_atlas_count << std::endl;
			stream << "Actual atlas count : " << atlas_count << std::endl;
		}

		std::string AtlasBase::GetGeneralInformationString() const
		{
			std::ostringstream stream;
			OutputGeneralInformation(stream);
			return stream.str();
		}

		// ========================================================================
		// Atlas functions
		// ========================================================================

		void Atlas::Clear()
		{
			AtlasBase::Clear();
			// destroy the bitmaps      
			bitmaps.clear();
		}

		bool Atlas::SaveAtlas(boost::filesystem::path const & filename) const
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(filename, target_dir, index_filename, bitmap_filename);

			// create a target directory if necessary   
			if (!boost::filesystem::is_directory(target_dir))
				if (!boost::filesystem::create_directories(target_dir))
					return false;

			// save the atlas
			return SaveAtlasBitmaps(target_dir, index_filename, bitmap_filename) && SaveAtlasIndex(target_dir, index_filename, bitmap_filename);
		}

		bool Atlas::SaveAtlasBitmaps(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const
		{
			bool result = true;
			// save them
			size_t count = bitmaps.size();
			for (size_t i = 0; (i < count) && result; ++i)
			{
				FIBITMAP * image = bitmaps[i].get();
				if (image == nullptr)
					continue;

				ImageDescription image_desc = ImageTools::GetImageDescription(image);
				if (!image_desc.IsValid())
					continue;

				FREE_IMAGE_FORMAT image_format = ImageTools::GetFreeImageFormat(image_desc.pixel_format);

				boost::filesystem::path dst_filename = target_dir / GetBitmapFilename(image_format, bitmap_filename, i);

				result = (FreeImage_Save(image_format, image, dst_filename.string().c_str(), 0) != 0);

			}
			return result;
		}

		bool Atlas::SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & bitmap_filename) const
		{
			// generate a file for the index (JSON format)
			std::ofstream stream(index_filename.string().c_str());
			if (stream)
			{
				nlohmann::json j;
				// insert the files
				j["bitmaps"] = nlohmann::json::array();

				size_t count = bitmaps.size();
				for (size_t i = 0; i < count ; ++i)
				{
					FIBITMAP * image = bitmaps[i].get();
					if (image == nullptr)
						continue;

					ImageDescription image_desc = ImageTools::GetImageDescription(image);
					if (!image_desc.IsValid())
						continue;

					FREE_IMAGE_FORMAT image_format = ImageTools::GetFreeImageFormat(image_desc.pixel_format);

					j["bitmaps"].push_back(GetBitmapFilename(image_format, bitmap_filename, i).string());
				}
				// insert the entries
				j["bitmap_sets"] = nlohmann::json::array();
				SaveIntoJSON(bitmap_sets, j["bitmap_sets"]);
				j["character_sets"] = nlohmann::json::array();
				SaveIntoJSON(character_sets, j["character_sets"]);
				// format the JSON into string and insert it into stream
				stream << j.dump(4);
				return true;
			}
			return false;
		}

		bool Atlas::LoadAtlas(boost::filesystem::path const & filename)
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(filename, target_dir, index_filename, bitmap_filename); // will be ignored during loading, real name is read from .JSON index
																				  // load the file into memory
			Buffer<char> buf = FileTools::LoadFile(index_filename, true);
			if (buf == nullptr)
				return false;

			// parse JSON file
			nlohmann::json j = JSONTools::Parse(buf.data);
			return LoadAtlas(j, target_dir);
		}

		bool Atlas::LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & target_dir)
		{
			bool result = true;

			// clean the object
			Clear();

			// load the files
			nlohmann::json const & json_files = JSONTools::GetStructure(j, "bitmaps");
			for (auto const json_filename : json_files)
			{
				std::string const & filename = json_filename;

				FIBITMAP * bitmap = ImageTools::LoadImageFromFile((target_dir / filename).string().c_str());
				if (bitmap == nullptr)
				{
					result = false;
					break;
				}

				int width = (int)FreeImage_GetWidth(bitmap);
				int height = (int)FreeImage_GetHeight(bitmap);
				if (bitmaps.size() == 0) // when reading the very first bitmap store the dimension
				{
					dimension.x = width;
					dimension.y = height;
				}
				else if (bitmaps.size() >= 1)  // for additional bitmaps ensure dimensions match the previous
				{
					if (dimension.x != width || dimension.y != height)
					{
						result = false;
						break;
					}
				}
				bitmaps.push_back(std::move(unique_bitmap_ptr(bitmap)));
			}
			// load the entries
			if (result)
			{
				LoadFromJSON(bitmap_sets, JSONTools::GetStructure(j, "bitmap_sets"));
				LoadFromJSON(character_sets, JSONTools::GetStructure(j, "character_sets"));
				atlas_count = bitmaps.size();
			}

			// in case of failure, reset the whole atlas once more
			if (!result)
				Clear();

			return result;
		}

		boost::filesystem::path Atlas::GetBitmapFilename(FREE_IMAGE_FORMAT image_format, boost::filesystem::path bitmap_filename, int index) const
		{
			char const * extension = "png"; // by default
			if (image_format == FIF_PNG)
				extension = "png";
			else if (image_format == FIF_EXR)
				extension = "exr";

			char buffer[30]; // far big enough
			sprintf_s(buffer, "_%d.%s", index, extension);
			return bitmap_filename.concat(buffer);
		}

		void Atlas::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const
		{
			// decompose INDEX and BITMAPS filename
			target_dir = filename.parent_path();
			index_filename = filename;
			bitmap_filename = filename.filename();

			if (!index_filename.has_extension())
				index_filename.replace_extension(".json");    // by default, INDEX file has extension JSON
			else
				bitmap_filename.replace_extension(); // for moment, BITMAP files should not have any extension
		}
	};
};

