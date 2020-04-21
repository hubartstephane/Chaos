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

		BitmapLayout BitmapInfo::DoGetFrameAnimationLayout(int index, WrapMode mode) const
		{
			// check index range, returns failure or clamp
			if (index < 0 || index >= animation_info->child_frame_count)
			{
				if (mode == WrapMode::check_ranges)
					return BitmapLayout();
				if (mode == WrapMode::clamp)
					index = std::clamp(index, 0, animation_info->child_frame_count - 1);
				else if (mode == WrapMode::wrap)
					index = index % animation_info->child_frame_count;
			}
			// find the bitmap further in the bitmapinfo array
			return this[index];
		}

		BitmapLayout BitmapInfo::DoGetGridAnimationLayout(glm::ivec2 grid_index, WrapMode mode) const
		{
			BitmapGridAnimationInfo grid_data = animation_info->grid_data;

			// check grid_index range, returns failure or clamp
			if (grid_index.x < 0 || grid_index.x >= grid_data.grid_size.x)
			{
				if (mode == WrapMode::check_ranges)
					return BitmapLayout();
				if (mode == WrapMode::clamp)
					grid_index.x = std::clamp(grid_index.x, 0, grid_data.grid_size.x - 1);
				else if (mode == WrapMode::wrap)
					grid_index.x = grid_index.x % grid_data.grid_size.x;
			}

			if (grid_index.y < 0 || grid_index.y >= grid_data.grid_size.y)
			{
				if (mode == WrapMode::check_ranges)
					return BitmapLayout();
				if (mode == WrapMode::clamp)
					grid_index.y = std::clamp(grid_index.y, 0, grid_data.grid_size.y - 1);
				else if (mode == WrapMode::wrap)
					grid_index.y = grid_index.y % grid_data.grid_size.y;
			}

			// check linear index and skip_lasts 
			if (grid_data.skip_lasts > 0) // if 0 this test is useless
			{
				int index = grid_index.x + grid_index.y * grid_data.grid_size.x;

				int animation_count = (grid_data.grid_size.x * grid_data.grid_size.y) - grid_data.skip_lasts;
				if (index >= animation_count)
				{
					if (mode == WrapMode::check_ranges)
						return BitmapLayout();
					if (mode == WrapMode::clamp)
						index = animation_count - 1;
					else if (mode == WrapMode::wrap)
						index = index % animation_count;
					grid_index.x = (index % grid_data.grid_size.x);
					grid_index.y = (index / grid_data.grid_size.x);
				}			
			}

			// XXX : for OpenGL, texture Y = 0 is bottom
			//       for bitmap,         Y = 0 is top
			//       => explains why (1. - Y)
			//
			// XXX : grid_index is top = 0 to bottom

			BitmapLayout result;
			result.bitmap_index = bitmap_index;
			result.width = width / animation_info->grid_data.grid_size.x;
			result.height = height / animation_info->grid_data.grid_size.y;
			result.x = x + (grid_index.x * result.width);
			result.y = y + (grid_index.y * result.height);

			float w = (topright_texcoord.x - bottomleft_texcoord.x);
			float h = (topright_texcoord.y - bottomleft_texcoord.y);

			float dx = w / (float)animation_info->grid_data.grid_size.x;
			float dy = h / (float)animation_info->grid_data.grid_size.y;

			result.bottomleft_texcoord.x = bottomleft_texcoord.x + dx * (float)grid_index.x;
			result.bottomleft_texcoord.y = (bottomleft_texcoord.y + h - dy * (float)grid_index.y) - dy;
			result.topright_texcoord.x = result.bottomleft_texcoord.x + dx;
			result.topright_texcoord.y = (result.bottomleft_texcoord.y + dy);

			return result;
		}

		WrapMode BitmapInfo::GetRequestWrapMode(WrapMode src) const
		{
			return src;
		}

		BitmapLayout BitmapInfo::GetAnimationLayoutFromTime(float time, WrapMode mode) const
		{


			return BitmapLayout();
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(size_t index, WrapMode mode) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();

			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				return DoGetFrameAnimationLayout((int)index, mode);
			}
			else // grid base animation
			{
				BitmapGridAnimationInfo grid_data = animation_info->grid_data;

				// transform the index into a 2D coord on the grid
				size_t frame_count = animation_info->grid_data.GetFrameCount();

				if (mode == WrapMode::clamp)
					index = std::min(index, frame_count);
				else if (mode == WrapMode::wrap)
					index = index % frame_count;
				else if (mode == WrapMode::check_ranges && index >= frame_count)
					return BitmapLayout();

				glm::ivec2 grid_index;
 				grid_index.x = ((int)index) % grid_data.grid_size.x;
				grid_index.y = ((int)index) / grid_data.grid_size.x;
				return DoGetGridAnimationLayout(grid_index, mode);
			}
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(glm::ivec2 const & grid_index, WrapMode mode) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();
			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				if (mode == WrapMode::check_ranges && grid_index.y != 0)
					return BitmapLayout();
				return DoGetFrameAnimationLayout(grid_index.x, mode);
			}
			else // grid base animation
			{
				return DoGetGridAnimationLayout(grid_index, mode);
			}
		}

		size_t BitmapInfo::GetAnimationImageCount() const
		{
			if (animation_info == nullptr)
				return 0;
			if (animation_info->IsFrameAnimation())
				return animation_info->child_frame_count;

			assert(animation_info->IsGridAnimation());
			return (size_t)std::max((animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts, 0);
		}

		float BitmapInfo::GetAnimationDuration() const
		{
			if (animation_info == nullptr)
				return 0.0f;
			return animation_info->GetAnimationDuration();
		}

		float BitmapInfo::GetFrameDuration() const
		{
			if (animation_info == nullptr)
				return 0.0f;
			return animation_info->GetFrameDuration();
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
			stream << stream_indent << "  bitmap_index          : " << info.bitmap_index << std::endl;
			stream << stream_indent << "  width                 : " << info.width << std::endl;
			stream << stream_indent << "  height                : " << info.height << std::endl;
			stream << stream_indent << "  x                     : " << info.x << std::endl;
			stream << stream_indent << "  y                     : " << info.y << std::endl;
			stream << stream_indent << "  bottomleft_texcoord.x : " << info.bottomleft_texcoord.x << std::endl;
			stream << stream_indent << "  bottomleft_texcoord.y : " << info.bottomleft_texcoord.y << std::endl;
			stream << stream_indent << "  topright_texcoord.x   : " << info.topright_texcoord.x << std::endl;
			stream << stream_indent << "  topright_texcoord.y   : " << info.topright_texcoord.y << std::endl;
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

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapAnimationInfo const & src)
		{
			return SaveIntoJSON(json_entry, (ImageAnimationDescription const &)src);
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapAnimationInfo & dst)
		{
			return LoadFromJSON(json_entry, (ImageAnimationDescription &)dst);
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapLayout const & src)
		{
			if (!json_entry.is_object())
				json_entry = nlohmann::json::object();

			JSONTools::SetAttribute(json_entry, "bitmap_index", src.bitmap_index);
			JSONTools::SetAttribute(json_entry, "x", src.x);
			JSONTools::SetAttribute(json_entry, "y", src.y);
			JSONTools::SetAttribute(json_entry, "width", src.width);
			JSONTools::SetAttribute(json_entry, "height", src.height);
			JSONTools::SetAttribute(json_entry, "bottomleft_texcoord", src.bottomleft_texcoord);
			JSONTools::SetAttribute(json_entry, "topright_texcoord", src.topright_texcoord);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapLayout & dst)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "bitmap_index", dst.bitmap_index);
			JSONTools::GetAttribute(json_entry, "x", dst.x);
			JSONTools::GetAttribute(json_entry, "y", dst.y);
			JSONTools::GetAttribute(json_entry, "width", dst.width);
			JSONTools::GetAttribute(json_entry, "height", dst.height);
			JSONTools::GetAttribute(json_entry, "bottomleft_texcoord", dst.bottomleft_texcoord);
			JSONTools::GetAttribute(json_entry, "topright_texcoord", dst.topright_texcoord);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, CharacterLayout const & src)
		{
			SaveIntoJSON(json_entry, (BitmapLayout const &)src); // call 'super' method (implies json_entry = nlohmann::json::object();)

			JSONTools::SetAttribute(json_entry, "advance_x", src.advance.x);
			JSONTools::SetAttribute(json_entry, "advance_y", src.advance.y);
			JSONTools::SetAttribute(json_entry, "bitmap_left", src.bitmap_left);
			JSONTools::SetAttribute(json_entry, "bitmap_top", src.bitmap_top);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, CharacterLayout & dst)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (BitmapLayout & )dst); // call 'super' method

			JSONTools::GetAttribute(json_entry, "advance_x", dst.advance.x);
			JSONTools::GetAttribute(json_entry, "advance_y", dst.advance.y);
			JSONTools::GetAttribute(json_entry, "bitmap_left", dst.bitmap_left);
			JSONTools::GetAttribute(json_entry, "bitmap_top", dst.bitmap_top);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapInfo const & src)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)src);  // call 'super' method (implies json_entry = nlohmann::json::object();)
			SaveIntoJSON(json_entry, (BitmapLayout const &)src); // call 'super' method (implies json_entry = nlohmann::json::object();)

			if (src.animation_info != nullptr)
			{
				json_entry["animation_info"] = nlohmann::json::object();
				SaveIntoJSON(json_entry["animation_info"], *src.animation_info);
			}
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapInfo & dst)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (NamedObject & )dst); // call 'super' method
			LoadFromJSON(json_entry, (BitmapLayout & )dst); // call 'super' method

			nlohmann::json const * animation_json = JSONTools::GetStructure(json_entry, "animation_info");
			if (animation_json != nullptr)
			{
				dst.animation_info = new BitmapAnimationInfo;
				if (dst.animation_info != nullptr)
					LoadFromJSON(*animation_json, *dst.animation_info);
			}
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, CharacterInfo const & src)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)src);     // call 'super' method (implies json_entry = nlohmann::json::object();)
			SaveIntoJSON(json_entry, (CharacterLayout const &)src); // call 'super' method (implies json_entry = nlohmann::json::object();)
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, CharacterInfo & dst)
		{
			if (!json_entry.is_object())
				return false;
			LoadFromJSON(json_entry, (NamedObject &)dst); // call 'super' method
			LoadFromJSON(json_entry, (CharacterLayout &)dst); // call 'super' method
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, FontInfo const & src)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)src); // call 'super' method (implies json_entry = nlohmann::json::object();)

			JSONTools::SetAttribute(json_entry, "max_character_width", src.max_character_width);
			JSONTools::SetAttribute(json_entry, "max_character_height", src.max_character_height);
			JSONTools::SetAttribute(json_entry, "ascender", src.ascender);
			JSONTools::SetAttribute(json_entry, "descender", src.descender);
			JSONTools::SetAttribute(json_entry, "face_height", src.face_height);
			JSONTools::SetAttribute(json_entry, "elements", src.elements);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, FontInfo & dst)
		{
			if (!json_entry.is_object())
				return false;

			LoadFromJSON(json_entry, (NamedObject & )dst); // call 'super' method

			JSONTools::GetAttribute(json_entry, "max_character_width", dst.max_character_width);
			JSONTools::GetAttribute(json_entry, "max_character_height", dst.max_character_height);
			JSONTools::GetAttribute(json_entry, "ascender", dst.ascender);
			JSONTools::GetAttribute(json_entry, "descender", dst.descender);
			JSONTools::GetAttribute(json_entry, "face_height", dst.face_height);
			JSONTools::GetAttribute(json_entry, "elements", dst.elements);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, FolderInfo const & src)
		{
			SaveIntoJSON(json_entry, (NamedObject const &)src); // call 'super' method (implies json_entry = nlohmann::json::object();)
			if (src.bitmaps.size())
				JSONTools::SetAttribute(json_entry, "bitmaps", src.bitmaps);
			if (src.fonts.size())
				JSONTools::SetAttribute(json_entry, "fonts", src.fonts);
			if (src.folders.size())
				JSONTools::SetAttribute(json_entry, "folders", src.folders);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, FolderInfo & dst)
		{
			if (!json_entry.is_object())
				return false;
			LoadFromJSON(json_entry, (NamedObject &)dst); // call 'super' method

			JSONTools::GetAttribute(json_entry, "bitmaps", dst.bitmaps);
			JSONTools::GetAttribute(json_entry, "fonts", dst.fonts);
			JSONTools::GetAttribute(json_entry, "folders", dst.folders);
			return true;
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

