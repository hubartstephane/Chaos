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
			if (!ParseFromNameReadGridSkip(name, i, result, name_result))
				return false;
			// ensure the result is not empty
			return !result.IsEmpty();
		}

		// ========================================================================
		// BitmapAnimationInfo functions
		// ========================================================================

		bool BitmapAnimationInfo::IsFrameAnimation() const
		{
			return (child_frame_count > 0);
		}

		bool BitmapAnimationInfo::IsGridAnimation() const
		{
			if (IsFrameAnimation())
				return false;
			return !grid_data.IsEmpty();
		}

		// ========================================================================
		// BitmapInfo functions
		// ========================================================================

		bool BitmapInfo::HasAnimation() const
		{
			if (animation_info == nullptr)
				return false;
			return true;
		}

		bool BitmapInfo::HasFrameAnimation() const
		{
			if (animation_info == nullptr)
				return false;
			return animation_info->IsFrameAnimation();
		}

		bool BitmapInfo::HasGridAnimation() const
		{
			if (animation_info == nullptr)
				return false;
			return animation_info->IsGridAnimation();
		}

		BitmapLayout BitmapInfo::DoGetFrameAnimationLayout(int index, GetBitmapLayoutFlag flag) const
		{
			// check index range, returns failure or clamp
			if (index < 0 || index >= animation_info->child_frame_count)
			{
				if (flag == GetBitmapLayoutFlag::none)
					return BitmapLayout();
				if (flag == GetBitmapLayoutFlag::clamp)
					index = MathTools::Clamp(index, 0, animation_info->child_frame_count - 1);
				else if (flag == GetBitmapLayoutFlag::wrap)
					index = index % animation_info->child_frame_count;
			}
			// find the bitmap further in the bitmapinfo array
			return this[index];
		}

		BitmapLayout BitmapInfo::DoGetGridAnimationLayout(glm::ivec2 grid_index, GetBitmapLayoutFlag flag) const
		{
			BitmapGridAnimationInfo grid_data = animation_info->grid_data;

			// check grid_index range, returns failure or clamp
			if (grid_index.x < 0 || grid_index.x >= grid_data.grid_size.x)
			{
				if (flag == GetBitmapLayoutFlag::none)
					return BitmapLayout();
				if (flag == GetBitmapLayoutFlag::clamp)
					grid_index.x = MathTools::Clamp(grid_index.x, 0, grid_data.grid_size.x - 1);
				else if (flag == GetBitmapLayoutFlag::wrap)
					grid_index.x = grid_index.x % grid_data.grid_size.x;
			}

			if (grid_index.y < 0 || grid_index.y >= grid_data.grid_size.y)
			{
				if (flag == GetBitmapLayoutFlag::none)
					return BitmapLayout();
				if (flag == GetBitmapLayoutFlag::clamp)
					grid_index.y = MathTools::Clamp(grid_index.y, 0, grid_data.grid_size.y - 1);
				else if (flag == GetBitmapLayoutFlag::wrap)
					grid_index.y = grid_index.y % grid_data.grid_size.y;
			}

			// check linear index and skip_lasts 
			if (grid_data.skip_lasts > 0) // if 0 this test is useless
			{
				int index = grid_index.x + grid_index.y * grid_data.grid_size.x;

				int animation_count = (grid_data.grid_size.x * grid_data.grid_size.y) - grid_data.skip_lasts;
				if (index >= animation_count)
				{
					if (flag == GetBitmapLayoutFlag::none)
						return BitmapLayout();
					if (flag == GetBitmapLayoutFlag::clamp)
						index = animation_count - 1;
					else if (flag == GetBitmapLayoutFlag::wrap)
						index = index % animation_count;
					grid_index.x = (index % grid_data.grid_size.x);
					grid_index.y = (index / grid_data.grid_size.x);
				}			
			}

			// compute the result
			BitmapLayout result;
			result.bitmap_index = bitmap_index;
			result.width = width / animation_info->grid_data.grid_size.x;
			result.height = height / animation_info->grid_data.grid_size.y;
			result.x = x + (grid_index.x * result.width);
			result.y = y + (grid_index.y * result.height);
			return result;
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(size_t index, GetBitmapLayoutFlag flag) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();
			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				return DoGetFrameAnimationLayout((int)index, flag);
			}
			else // grid base animation
			{
				BitmapGridAnimationInfo grid_data = animation_info->grid_data;

				glm::ivec2 grid_index;
				grid_index.x = ((int)index) % grid_data.grid_size.x;
				grid_index.y = ((int)index) / grid_data.grid_size.x;
				return DoGetGridAnimationLayout(grid_index, flag);
			}
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(glm::ivec2 const & grid_index, GetBitmapLayoutFlag flag) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();
			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				if (grid_index.y != 0 && flag == GetBitmapLayoutFlag::none)
					return BitmapLayout();
				return DoGetFrameAnimationLayout(grid_index.x, flag);
			}
			else // grid base animation
			{
				return DoGetGridAnimationLayout(grid_index, flag);
			}
		}

		size_t BitmapInfo::GetAnimationImageCount() const
		{
			if (animation_info == nullptr)
				return 0;
			if (animation_info->IsFrameAnimation())
				return animation_info->child_frame_count;

			assert(animation_info->IsGridAnimation());
			return (size_t)MathTools::Maximum((animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts, 0);
		}

		double BitmapInfo::GetFrameTime() const
		{
			if (animation_info == nullptr)
				return 0;
			return animation_info->GetFrameTime();
		}

		// ========================================================================
		// FolderInfo functions
		// ========================================================================

		void FolderInfo::DoCollectEntries(std::vector<BitmapInfo> & result)
		{
			size_t count = bitmaps.size();

			result.reserve(result.size() + count);
			for (size_t i = 0; i < count; ++i)
				result.push_back(bitmaps[i]);		
		}
		
		void FolderInfo::DoCollectEntries(std::vector<CharacterInfo> & result)
		{
			size_t font_count = fonts.size();
			for (size_t i = 0; i < font_count; ++i)
			{
				font_type const & font_info = fonts[i];

				size_t count = font_info.elements.size();

				result.reserve(result.size() + count);
				for (size_t j = 0; j < count; ++j)
					result.push_back(font_info.elements[j]);
			}
		}
		
		void FolderInfo::DoCollectEntries(std::vector<BitmapLayout> & result, bool collect_bitmaps, bool collect_characters)
		{
			if (collect_bitmaps)
			{
				size_t count = bitmaps.size();

				result.reserve(result.size() + count);
				for (size_t i = 0; i < count; ++i)
					result.push_back(bitmaps[i]);	
			}

			if (collect_characters)
			{
				size_t font_count = fonts.size();
				for (size_t i = 0; i < font_count; ++i)
				{
					font_type const & font_info = fonts[i];

					size_t count = font_info.elements.size();

					result.reserve(result.size() + count);
					for (size_t j = 0; j < count; ++j)
						result.push_back(font_info.elements[j]);
				}
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
				SaveIntoJSON(json["root_folder"], root_folder);

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
					JSONTools::GetAttribute(json, "root_folder", root_folder);
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

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapAnimationInfo const & info)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::SetAttribute(json_entry, "grid_size_x", info.grid_data.grid_size.x);
			JSONTools::SetAttribute(json_entry, "grid_size_y", info.grid_data.grid_size.y);
			JSONTools::SetAttribute(json_entry, "skip_lasts", info.grid_data.skip_lasts);
			JSONTools::SetAttribute(json_entry, "child_frame_count", info.child_frame_count);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapAnimationInfo & info)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "grid_size_x", info.grid_data.grid_size.x);
			JSONTools::GetAttribute(json_entry, "grid_size_y", info.grid_data.grid_size.y);
			JSONTools::GetAttribute(json_entry, "skip_lasts", info.grid_data.skip_lasts);
			JSONTools::GetAttribute(json_entry, "child_frame_count", info.child_frame_count);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapLayout const & info)
		{
			JSONTools::SetAttribute(json_entry, "bitmap_index", info.bitmap_index);
			JSONTools::SetAttribute(json_entry, "x", info.x);
			JSONTools::SetAttribute(json_entry, "y", info.y);
			JSONTools::SetAttribute(json_entry, "width", info.width);
			JSONTools::SetAttribute(json_entry, "height", info.height);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapLayout & info)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "bitmap_index", info.bitmap_index);
			JSONTools::GetAttribute(json_entry, "x", info.x);
			JSONTools::GetAttribute(json_entry, "y", info.y);
			JSONTools::GetAttribute(json_entry, "width", info.width);
			JSONTools::GetAttribute(json_entry, "height", info.height);		
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, CharacterLayout const & info)
		{
			SaveIntoJSON(json_entry, (BitmapLayout const & )info); // call 'super' method

			JSONTools::SetAttribute(json_entry, "advance_x", info.advance.x);
			JSONTools::SetAttribute(json_entry, "advance_y", info.advance.y);
			JSONTools::SetAttribute(json_entry, "bitmap_left", info.bitmap_left);
			JSONTools::SetAttribute(json_entry, "bitmap_top", info.bitmap_top);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, CharacterLayout & info)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (BitmapLayout & )info); // call 'super' method

			JSONTools::GetAttribute(json_entry, "advance_x", info.advance.x);
			JSONTools::GetAttribute(json_entry, "advance_y", info.advance.y);
			JSONTools::GetAttribute(json_entry, "bitmap_left", info.bitmap_left);
			JSONTools::GetAttribute(json_entry, "bitmap_top", info.bitmap_top);		
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapInfo const & info)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)info); // call 'super' method
			SaveIntoJSON(json_entry, (BitmapLayout const &)info); // call 'super' method

			if (info.animation_info != nullptr)
			{
				json_entry["animation_info"] = nlohmann::json::object();
				SaveIntoJSON(json_entry["animation_info"], *info.animation_info);
			}
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapInfo & info)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (NamedObject & )info); // call 'super' method
			LoadFromJSON(json_entry, (BitmapLayout & )info); // call 'super' method

			nlohmann::json const * animation_json = JSONTools::GetStructure(json_entry, "animation_info");
			if (animation_json != nullptr)
			{
				info.animation_info = new BitmapAnimationInfo;
				if (info.animation_info != nullptr)
					LoadFromJSON(*animation_json, *info.animation_info);
			}
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, CharacterInfo const & info)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)info); // call 'super' method
			SaveIntoJSON(json_entry, (CharacterLayout const &)info); // call 'super' method

			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, CharacterInfo & info)
		{
			if (!json_entry.is_object())
				return false;
			LoadFromJSON(json_entry, (NamedObject &)info); // call 'super' method
			LoadFromJSON(json_entry, (CharacterLayout &)info); // call 'super' method
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, FontInfo const & info)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)info); // call 'super' method

			JSONTools::SetAttribute(json_entry, "max_character_width", info.max_character_width);
			JSONTools::SetAttribute(json_entry, "max_character_height", info.max_character_height);
			JSONTools::SetAttribute(json_entry, "ascender", info.ascender);
			JSONTools::SetAttribute(json_entry, "descender", info.descender);
			JSONTools::SetAttribute(json_entry, "face_height", info.face_height);
			JSONTools::SetAttribute(json_entry, "elements", nlohmann::json::array());
			JSONTools::SaveVectorIntoJSON(info.elements, json_entry["elements"]);

			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, FontInfo & info)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (NamedObject & )info); // call 'super' method

			JSONTools::GetAttribute(json_entry, "max_character_width", info.max_character_width);
			JSONTools::GetAttribute(json_entry, "max_character_height", info.max_character_height);
			JSONTools::GetAttribute(json_entry, "ascender", info.ascender);
			JSONTools::GetAttribute(json_entry, "descender", info.descender);
			JSONTools::GetAttribute(json_entry, "face_height", info.face_height);
			JSONTools::GetAttribute(json_entry, "elements", info.elements);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, FolderInfo const & info)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)info); // call 'super' method

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

			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, FolderInfo & info)
		{
			if (!json_entry.is_object())
				return false;
			LoadFromJSON(json_entry, (NamedObject & )info); // call 'super' method

			JSONTools::GetAttribute(json_entry, "bitmaps", info.bitmaps);
			JSONTools::GetAttribute(json_entry, "fonts", info.fonts);
			JSONTools::GetAttribute(json_entry, "folders", info.folders);
			return true;
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

