#include <chaos/BitmapAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/JSONTools.h>
#include <chaos/NamedObject.h>
#include <chaos/StreamTools.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		// ========================================================================
		// BitmapGridAnimationInfo functions
		// ========================================================================

		bool BitmapGridAnimationInfo::ParseDigitReverse(char const * str, int & start)
		{
			// buffer already consumned
			if (start < 0)
				return false;
			// reverse parsing
			int i = start;
			while (i >= 0 && std::isdigit(str[i]))
				--i;
			// no figure found ?
			if (start == i)
				return false;
			start = i;
			return true;
		}

		bool BitmapGridAnimationInfo::ParseFromNameReadGridX(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
		{
			// get a number
			if (!ParseDigitReverse(name, i)) // k should be just before a number
				return false;
			if (i < 1 || name[i] != '_')
				return false;

			if (name_result != nullptr)
				*name_result = std::string(name, i);

			result.grid_size.x = i + 1;

			// compute the result (parsing numbers)
			assert(result.grid_size.x >= 0);
			assert(result.grid_size.y >= 0);

			result.grid_size.x = atoi(name + result.grid_size.x);
			result.grid_size.y = atoi(name + result.grid_size.y);
			if (result.skip_lasts >= 0)
				result.skip_lasts = atoi(name + result.skip_lasts);
			else
				result.skip_lasts = 0;

			return true;
		}

		bool BitmapGridAnimationInfo::ParseFromNameReadGridY(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
		{
			// get a number
			if (!ParseDigitReverse(name, i)) // k should be just before a number
				return false;
			if (name[i] != 'x')
				return false;
			// store the index of the number
			result.grid_size.y = i + 1;
			// try to read x
			return ParseFromNameReadGridX(name, i - 1, result, name_result);
		}

		bool BitmapGridAnimationInfo::ParseFromNameReadGridSkip(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
		{
			// get a number
			if (!ParseDigitReverse(name, i))
				return false;
			// we were reading Y in fact
			if (name[i] == 'x')
			{
				result.grid_size.y = i + 1;
				return ParseFromNameReadGridX(name, i - 1, result, name_result);
			}
			// is it really a skip ?
			if (name[i] != 's')
				return false;

			result.skip_lasts = i + 1;
			return ParseFromNameReadGridY(name, i - 1, result, name_result);
		}

		bool BitmapGridAnimationInfo::ParseFromName(char const * name, BitmapGridAnimationInfo & result, std::string * name_result)
		{
			// hack the structure to hold 'pointer' on the string that contains the values
			result.grid_size.x = -1;
			result.grid_size.y = -1;
			result.skip_lasts = -1;

			// the format of an animated grid image can be
			//   filename_1x4.png
			//   filename_1x4
			//   filename_1x4_s4.png
			//   filename_1x4_s4
			//
			//   the minimum suffis size for an animated image is 4 characters (_1x4)

			static int const MIN_PREFIX_SIZE = 4;

			// get the length
			int len = (int)strlen(name);
			if (len < 1 + MIN_PREFIX_SIZE) // +1 for at least one character
				return false;

			// remove the extension if any
			int i = len;
			while (i >= 0 && name[i] != '.')
				--i;
			if (name[i] == '.') // prefix found => remove it
			{
				if (i < 1 + MIN_PREFIX_SIZE) // i is the size of the cut name
					return false;
				--i;
			}
			else
			{
				i = len - 1; // take the whole input
			}
			// parsing chain
			return ParseFromNameReadGridSkip(name, i, result, name_result);
		}

		// ========================================================================
		// BitmapInfo functions
		// ========================================================================

		BitmapLayout BitmapInfo::GetAnimationFrameLayout(size_t index) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();
			// test whether the frame index is valid
			if (index >= GetAnimationFrameCount())
				return BitmapLayout();
			// whether this is a grid animation
			if (!animation_info->grid_data.IsEmpty())
			{
				size_t frame_count = (size_t)(animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts;
				if (index >= frame_count)
					return BitmapLayout();

			}
			// whether this is contigus Bitmap style animation




			return BitmapLayout();
		}

		BitmapLayout BitmapInfo::GetAnimationFrameLayout(glm::ivec2 const & index) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();
			// this function is only valid for grid
			if (animation_info->grid_data.IsEmpty())
				return BitmapLayout();
			// test whether the index is valid
			if (index.x < 0 || index.x >= animation_info->grid_data.grid_size.x)
				return BitmapLayout();
			if (index.y < 0 || index.y >= animation_info->grid_data.grid_size.y)
				return BitmapLayout();
			


			//child_info->x = parent_info->x + (parent_info->width / parent_input_entry->grid_size.x) * input_entry->grid_position.x;
			//child_info->y = parent_info->y + (parent_info->height / parent_input_entry->grid_size.y) * input_entry->grid_position.y;

			return BitmapLayout();
		}

		size_t BitmapInfo::GetAnimationFrameCount() const
		{
			if (animation_info == nullptr)
				return 0;
			if (!animation_info->grid_data.IsEmpty())
				return (size_t)(animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts;
			return (size_t)animation_info->child_frame_count;
		}





		// ========================================================================
		// FolderInfo functions
		// ========================================================================

		void FolderInfo::CollectEntries(std::vector<BitmapLayout> * layout_result, std::vector<BitmapInfo> * bitmap_result, std::vector<CharacterInfo> * character_result, bool recursive) const
		{																													 
			// early exit
			if (layout_result == nullptr && bitmap_result == nullptr && character_result == nullptr)
				return;
			// collect layouts
			if (layout_result != nullptr)
			{
				size_t count = bitmaps.size();

				layout_result->reserve(layout_result->size() + count);
				for (size_t i = 0; i < count; ++i)
					layout_result->push_back(bitmaps[i]);
			}
			// collect bitmaps
			if (bitmap_result != nullptr)
			{
				size_t count = bitmaps.size();

				bitmap_result->reserve(bitmap_result->size() + count);
				for (size_t i = 0; i < count; ++i)
					bitmap_result->push_back(bitmaps[i]);
			}
			// collect fonts
			if (layout_result != nullptr || character_result != nullptr)
			{
				size_t font_count = fonts.size();
				for (size_t i = 0; i < font_count; ++i)
				{
					font_type const & font_info = fonts[i];

					size_t count = font_info.elements.size();

					if (layout_result != nullptr)
					{
						layout_result->reserve(layout_result->size() + count);
						for (size_t j = 0; j < count; ++j)
							layout_result->push_back(font_info.elements[j]);
					}

					if (character_result != nullptr)
					{
						character_result->reserve(character_result->size() + count);
						for (size_t j = 0; j < count; ++j)
							character_result->push_back(font_info.elements[j]);
					}
				}

			}
			// recursion
			if (recursive)
			{
				size_t folder_count = folders.size();
				for (size_t i = 0; i < folder_count; ++i)
					folders[i]->CollectEntries(layout_result, bitmap_result, character_result, recursive);
			}
		}

		// ========================================================================
		// Atlas functions
		// ========================================================================

		void AtlasBase::Clear()
		{
			// reset members
			atlas_count = 0;
			dimension = glm::ivec2(0, 0);
			// destroy the root folder
			AtlasBaseTemplate<BitmapInfo, FontInfo, FolderInfo, ReferencedObject>::Clear();
		}

		float AtlasBase::ComputeSurface(int bitmap_index) const
		{
			return DoComputeSurface(bitmap_index, &root_folder);
		}

		float AtlasBase::DoComputeSurface(int bitmap_index, FolderInfo const * folder_info) const
		{
			// XXX : ignore child entries. Their surface is already considered by their parent entry

			if (folder_info == nullptr)
				return 0.0f;

			float result = 0.0f;
			// surface for the bitmaps in the folder
			for (BitmapInfo const & bitmap_info : folder_info->bitmaps)
				if (bitmap_info.bitmap_index == bitmap_index || bitmap_index < 0)
					result += (float)(bitmap_info.width * bitmap_info.height);
			// surface for the fonts in the folder
			for (FontInfo const & font_info : folder_info->fonts)
				for (CharacterInfo const & character_info : font_info.elements)
					if (character_info.bitmap_index == bitmap_index || bitmap_index < 0)
						result += (float)(character_info.width * character_info.height);
			// recursive calls
			size_t count = folder_info->folders.size();
			for (size_t i = 0 ; i < count ; ++i)
				result += DoComputeSurface(bitmap_index, folder_info->folders[i].get());
			return result;
		}

		void AtlasBase::OutputInfo(std::ostream & stream) const
		{
			DoOutputInfo(root_folder, stream);
		}

		void AtlasBase::DoOutputInfo(FolderInfo const & folder_info, std::ostream & stream, int indent) const
		{
			StreamTools::OStreamIndent stream_indent(indent);

			stream << stream_indent << "Folder:" << std::endl;
			NamedObject const & named_info = folder_info;
			DoOutputInfo(named_info, stream, indent);

			// output the bitmaps in the folder
			if (folder_info.bitmaps.size() > 0)
			{
				StreamTools::OStreamIndent bitmap_stream_indent(indent + 1);
				stream << bitmap_stream_indent << "Bitmaps:" << std::endl;
				for (BitmapInfo const & bitmap_info : folder_info.bitmaps)
					DoOutputInfo(bitmap_info, stream, indent + 1);
			}
			// output the fonts in the folder
			if (folder_info.fonts.size() > 0)
			{
				StreamTools::OStreamIndent font_stream_indent(indent + 1);
				stream << font_stream_indent << "Fonts:" << std::endl;
				for (FontInfo const & font_info : folder_info.fonts)
					DoOutputInfo(font_info, stream, indent + 1);
			}
			// recursive calls
			if (folder_info.folders.size() > 0)
			{
				StreamTools::OStreamIndent folder_stream_indent(indent + 1);
				stream << folder_stream_indent << "Child Folders:" << std::endl;

				size_t count = folder_info.folders.size();
				for (size_t i = 0; i < count; ++i)
					DoOutputInfo(*folder_info.folders[i], stream, indent + 1);
			}
		}

		void AtlasBase::DoOutputInfo(NamedObject const & info, std::ostream & stream, int indent)
		{
			StreamTools::OStreamIndent stream_indent(indent);
			stream << stream_indent << "  name         : " << info.name << std::endl;
			stream << stream_indent << "  tag          : " << info.tag << std::endl;
		}


		void AtlasBase::DoOutputInfo(BitmapLayout const & info, std::ostream & stream, int indent)
		{
			StreamTools::OStreamIndent stream_indent(indent);
			stream << stream_indent << "  bitmap_index : " << info.bitmap_index << std::endl;
			stream << stream_indent << "  width        : " << info.width << std::endl;
			stream << stream_indent << "  height       : " << info.height << std::endl;
			stream << stream_indent << "  x            : " << info.x << std::endl;
			stream << stream_indent << "  y            : " << info.y << std::endl;
		}

		void AtlasBase::DoOutputInfo(CharacterLayout const & info, std::ostream & stream, int indent)
		{
			BitmapLayout const & bitmap_layout = info;
			DoOutputInfo(bitmap_layout, stream, indent);

			StreamTools::OStreamIndent stream_indent(indent);
			stream << stream_indent << "  advance.x    : " << info.advance.x << std::endl;
			stream << stream_indent << "  advance.y    : " << info.advance.y << std::endl;
			stream << stream_indent << "  bitmap_left  : " << info.bitmap_left << std::endl;
			stream << stream_indent << "  bitmap_top   : " << info.bitmap_top << std::endl;
		}

		void AtlasBase::DoOutputInfo(BitmapInfo const & info, std::ostream & stream, int indent)
		{
			NamedObject const & named_info = info;
			DoOutputInfo(named_info, stream, indent);

			BitmapLayout const & bitmap_layout = info;
			DoOutputInfo(bitmap_layout, stream, indent);
		}

		void AtlasBase::DoOutputInfo(CharacterInfo const & info, std::ostream & stream, int indent)
		{
			NamedObject const & named_info = info;
			DoOutputInfo(named_info, stream, indent);

			CharacterLayout const & character_layout = info;
			DoOutputInfo(character_layout, stream, indent);
		}

		void AtlasBase::DoOutputInfo(FontInfo const & info, std::ostream & stream, int indent)
		{
			NamedObject const & named_info = info;
			DoOutputInfo(info, stream, indent);

			StreamTools::OStreamIndent stream_indent(indent);
			stream << stream_indent << "  max_character_width  : " << info.max_character_width << std::endl;
			stream << stream_indent << "  max_character_height : " << info.max_character_height << std::endl;
			stream << stream_indent << "  ascender             : " << info.ascender << std::endl;
			stream << stream_indent << "  descender            : " << info.descender << std::endl;
			stream << stream_indent << "  face_height          : " << info.face_height << std::endl;

			// output the charactars in the fonts
			if (info.elements.size() > 0)
			{
				stream << stream_indent << "Characters:" << std::endl;
				for (CharacterInfo const & character_info : info.elements)
					DoOutputInfo(character_info, stream, indent + 1);
			}
		}

		std::string AtlasBase::GetInfoString() const
		{
			std::ostringstream out;
			OutputInfo(out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(NamedObject const & info)
		{
			std::ostringstream out;
			DoOutputInfo(info, out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(BitmapInfo const & info)
		{
			std::ostringstream out;
			DoOutputInfo(info, out);
			return out.str();
		}

		std::string AtlasBase::GetInfoString(CharacterInfo const & info)
		{
			std::ostringstream out;
			DoOutputInfo(info, out);
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
				OutputAtlasSpaceOccupation((int)i, stream);
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

		void AtlasBase::CollectEntries(std::vector<BitmapLayout> * layout_result, std::vector<BitmapInfo> * bitmap_result, std::vector<CharacterInfo> * character_result, bool recursive) const
		{
			root_folder.CollectEntries(layout_result, bitmap_result, character_result, recursive);
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

		bool Atlas::SaveAtlas(FilePathParam const & path) const
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(path, target_dir, index_filename, bitmap_filename);

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
				if (!image_desc.IsValid(false))
					continue;

				FREE_IMAGE_FORMAT image_format = ImageTools::GetFreeImageFormat(image_desc.pixel_format);

				boost::filesystem::path dst_filename = target_dir / GetBitmapFilename(image_format, bitmap_filename, (int)i);

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
				nlohmann::json json;
				// insert the files
				json["bitmaps"] = nlohmann::json::array();

				size_t count = bitmaps.size();
				for (size_t i = 0; i < count ; ++i)
				{
					FIBITMAP * image = bitmaps[i].get();
					if (image == nullptr)
						continue;

					ImageDescription image_desc = ImageTools::GetImageDescription(image);
					if (!image_desc.IsValid(false))
						continue;

					FREE_IMAGE_FORMAT image_format = ImageTools::GetFreeImageFormat(image_desc.pixel_format);

					json["bitmaps"].push_back(GetBitmapFilename(image_format, bitmap_filename, (int)i).string());
				}
				// insert the entries
				json["root_folder"] = nlohmann::json::object();				
				SaveIntoJSON(root_folder, json["root_folder"]);

				// format the JSON into string and insert it into stream
				stream << json.dump(4);
				return true;
			}
			return false;
		}

		bool Atlas::LoadAtlas(FilePathParam const & path)
		{
			// decompose the filename
			boost::filesystem::path target_dir;
			boost::filesystem::path index_filename;
			boost::filesystem::path bitmap_filename;
			SplitFilename(path, target_dir, index_filename, bitmap_filename); // will be ignored during loading, real name is read from .JSON index
			// load the file into memory
			Buffer<char> buf = FileTools::LoadFile(index_filename, true);
			if (buf == nullptr)
				return false;

			// parse JSON file
			nlohmann::json json;
			if (JSONTools::Parse(buf.data, json))
				return LoadAtlas(json, target_dir);
			return false;
		}

		bool Atlas::LoadAtlas(nlohmann::json const & json, boost::filesystem::path const & target_dir)
		{
			bool result = true;

			// clean the object
			Clear();

			// load the files
			nlohmann::json const * json_files = JSONTools::GetStructure(json, "bitmaps");
			if (json_files != nullptr)
			{
				for (auto const json_filename : *json_files)
				{
					std::string const & filename = json_filename;

					FIBITMAP * bitmap = ImageTools::LoadImageFromFile(target_dir / filename);
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
					bitmaps.push_back(std::move(bitmap_ptr(bitmap)));
				}
				// load the entries
				if (result)
				{
					nlohmann::json const * root_folder_json = JSONTools::GetStructure(json, "root_folder");
					if (root_folder_json != nullptr)
						LoadFromJSON(root_folder, *root_folder_json);

					atlas_count = (int)bitmaps.size();
				}			
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

		void Atlas::SplitFilename(FilePathParam const & path, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & bitmap_filename) const
		{
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();

			// decompose INDEX and BITMAPS filename
			target_dir = resolved_path.parent_path();
			index_filename = resolved_path;
			bitmap_filename = resolved_path.filename();

			if (!index_filename.has_extension())
				index_filename.replace_extension(".json");    // by default, INDEX file has extension JSON
			else
				bitmap_filename.replace_extension(); // for moment, BITMAP files should not have any extension
		}

		// ========================================================================
		// JSON functions
		// ========================================================================

		void SaveIntoJSON(BitmapAnimationInfo const & info, nlohmann::json & json_entry)
		{
			json_entry["grid_size_x"] = info.grid_data.grid_size.x;
			json_entry["grid_size_y"] = info.grid_data.grid_size.y;
			json_entry["skip_lasts"]  = info.grid_data.skip_lasts;
			json_entry["child_frame_count"] = info.child_frame_count;
		}

		void LoadFromJSON(BitmapAnimationInfo & info, nlohmann::json const & json_entry)
		{
			info.grid_data.grid_size.x = json_entry["grid_size_x"];
			info.grid_data.grid_size.y = json_entry["grid_size_y"];
			info.grid_data.skip_lasts  = json_entry["skip_lasts"];
			info.child_frame_count     = json_entry["child_frame_count"];	
		}

		void SaveIntoJSON(BitmapLayout const & info, nlohmann::json & json_entry)
		{
			json_entry["bitmap_index"] = info.bitmap_index;
			json_entry["x"] = info.x;
			json_entry["y"] = info.y;
			json_entry["width"] = info.width;
			json_entry["height"] = info.height;
		}

		void LoadFromJSON(BitmapLayout & info, nlohmann::json const & json_entry)
		{
			JSONTools::GetAttribute(json_entry, "bitmap_index", info.bitmap_index, 0);
			JSONTools::GetAttribute(json_entry, "x", info.x, 0);
			JSONTools::GetAttribute(json_entry, "y", info.y, 0);
			JSONTools::GetAttribute(json_entry, "width", info.width, 0);
			JSONTools::GetAttribute(json_entry, "height", info.height, 0);		
		}

		void SaveIntoJSON(CharacterLayout const & info, nlohmann::json & json_entry)
		{
			BitmapLayout const & bitmap_layout = info;
			SaveIntoJSON(bitmap_layout, json_entry); // call 'super' method

			json_entry["advance_x"] = info.advance.x;
			json_entry["advance_y"] = info.advance.y;
			json_entry["bitmap_left"] = info.bitmap_left;
			json_entry["bitmap_top"] = info.bitmap_top;
		}

		void LoadFromJSON(CharacterLayout & info, nlohmann::json const & json_entry)
		{
			BitmapLayout & bitmap_layout = info;
			LoadFromJSON(bitmap_layout, json_entry); // call 'super' method

			JSONTools::GetAttribute(json_entry, "advance_x", info.advance.x, 0);
			JSONTools::GetAttribute(json_entry, "advance_y", info.advance.y, 0);
			JSONTools::GetAttribute(json_entry, "bitmap_left", info.bitmap_left, 0);
			JSONTools::GetAttribute(json_entry, "bitmap_top", info.bitmap_top, 0);		
		}

		void SaveIntoJSON(BitmapInfo const & info, nlohmann::json & json_entry)
		{
			NamedObject const & named_info = info;
			SaveIntoJSON(named_info, json_entry); // call 'super' method

			BitmapLayout const & bitmap_layout = info;
			SaveIntoJSON(bitmap_layout, json_entry); // call 'super' method

			if (info.animation_info != nullptr)
			{
				json_entry["animation_info"] = nlohmann::json::object();
				SaveIntoJSON(*info.animation_info, json_entry["animation_info"]);
			}
		}

		void LoadFromJSON(BitmapInfo & info, nlohmann::json const & json_entry)
		{
			NamedObject & named_info = info;
			LoadFromJSON(named_info, json_entry); // call 'super' method

			BitmapLayout & bitmap_layout = info;
			LoadFromJSON(bitmap_layout, json_entry); // call 'super' method

			nlohmann::json const * animation_json = JSONTools::GetStructure(json_entry, "animation_info");
			if (animation_json != nullptr)
			{
				info.animation_info = new BitmapAnimationInfo;
				if (info.animation_info != nullptr)
					LoadFromJSON(*info.animation_info, *animation_json);
			}
		}

		void SaveIntoJSON(CharacterInfo const & info, nlohmann::json & json_entry)
		{
			NamedObject const & named_info = info;
			SaveIntoJSON(named_info, json_entry); // call 'super' method

			CharacterLayout const & character_layout = info;
			SaveIntoJSON(character_layout, json_entry); // call 'super' method
		}

		void LoadFromJSON(CharacterInfo & info, nlohmann::json const & json_entry)
		{
			NamedObject & named_info = info;
			LoadFromJSON(named_info, json_entry); // call 'super' method

			CharacterLayout & character_layout = info;
			LoadFromJSON(character_layout, json_entry); // call 'super' method
		}

		void SaveIntoJSON(FontInfo const & info, nlohmann::json & json_entry)
		{
			NamedObject const & named_info = info;
			SaveIntoJSON(named_info, json_entry); // call 'super' method

			json_entry["max_character_width"] = info.max_character_width;
			json_entry["max_character_height"] = info.max_character_height;
			json_entry["ascender"] = info.ascender;
			json_entry["descender"] = info.descender;
			json_entry["face_height"] = info.face_height;
			json_entry["elements"] = nlohmann::json::array();
			JSONTools::SaveVectorIntoJSON(info.elements, json_entry["elements"]);
		}

		void LoadFromJSON(FontInfo & info, nlohmann::json const & json_entry)
		{
			NamedObject & named_info = info;
			LoadFromJSON(named_info, json_entry); // call 'super' method

			JSONTools::GetAttribute(json_entry, "max_character_width", info.max_character_width, 0);
			JSONTools::GetAttribute(json_entry, "max_character_height", info.max_character_height, 0);
			JSONTools::GetAttribute(json_entry, "ascender", info.ascender, 0);
			JSONTools::GetAttribute(json_entry, "descender", info.descender, 0);
			JSONTools::GetAttribute(json_entry, "face_height", info.face_height, 0);

			nlohmann::json const * json_elements = JSONTools::GetStructure(json_entry, "elements");
			if (json_elements != nullptr)
				JSONTools::LoadVectorFromJSON(info.elements, *json_elements);
		}

		void SaveIntoJSON(FolderInfo const & info, nlohmann::json & json_entry)
		{
			NamedObject const & named_info = info;
			SaveIntoJSON(named_info, json_entry); // call 'super' method

			if (info.bitmaps.size())
			{
				json_entry["bitmaps"] = nlohmann::json::array();
				JSONTools::SaveVectorIntoJSON(info.bitmaps, json_entry["bitmaps"]);
			}

			if (info.fonts.size())
			{
				json_entry["fonts"] = nlohmann::json::array();
				JSONTools::SaveVectorIntoJSON(info.fonts, json_entry["fonts"]);
			}

			if (info.folders.size())
			{
				json_entry["folders"] = nlohmann::json::array();
				JSONTools::SaveVectorIntoJSON(info.folders, json_entry["folders"]);
			}
		}

		void LoadFromJSON(FolderInfo & info, nlohmann::json const & json_entry)
		{
			NamedObject & named_info = info;
			LoadFromJSON(named_info, json_entry); // call 'super' method

			nlohmann::json const * bitmaps_elements = JSONTools::GetStructure(json_entry, "bitmaps");
			if (bitmaps_elements != nullptr)
				JSONTools::LoadVectorFromJSON(info.bitmaps, *bitmaps_elements);

			nlohmann::json const * fonts_elements = JSONTools::GetStructure(json_entry, "fonts");
			if (fonts_elements != nullptr)
				JSONTools::LoadVectorFromJSON(info.fonts, *fonts_elements);

			nlohmann::json const * folders_elements = JSONTools::GetStructure(json_entry, "folders");
			if (folders_elements != nullptr)
				JSONTools::LoadVectorFromJSON(info.folders, *folders_elements);
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

