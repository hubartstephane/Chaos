#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace BitmapAtlas
	{
		static constexpr size_t INDENT_VALUE = 2;

		// ========================================================================
		// BitmapLayout functions
		// ========================================================================

		ParticleTexcoords BitmapLayout::GetTexcoords() const
		{
			ParticleTexcoords result;
			result.bottomleft = bottomleft_texcoord;
			result.topright = topright_texcoord;
			result.bitmap_index = bitmap_index;
			return result;
		}

		// if src.x AND src.y are 0     => use the size in the atlas
		// if src.x AND src.y are not 0 => returns the src unmodified
		// if src.x OR  src.y is  0     => use bitmap ratio to replace the 0 axis
		glm::vec2 BitmapLayout::ApplyRatioToSize(glm::vec2 src) const
		{
			float bw = float(width);
			float bh = float(height);

			if (src.x <= 0.0f || src.y <= 0.0f)
			{
				if (src.x <= 0.0f && src.y <= 0.0f) // both are invalid
					src = { bw, bh };
				else if (src.x <= 0.0f)
					src.x = src.y * bw / bh;
				else
					src.y = src.x * bh / bw;
			}
			return src;
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

		BitmapLayout BitmapInfo::DoGetFrameAnimationLayout(int index, WrapMode mode) const
		{
			// wrap the index
			if (!ApplyWrapMode(index, 0, animation_info->child_frame_count + 1 - 1, mode, index)) // the real number of frames is (child_frame_count + 1). So for clamping we use (child_frame_count + 1 - 1)
				return BitmapLayout();
			// find the bitmap further in the bitmapinfo array
			return this[static_cast<size_t>(index)];
		}

		BitmapLayout BitmapInfo::DoGetGridAnimationLayout(glm::ivec2 grid_index, WrapMode mode) const
		{
			BitmapGridAnimationInfo grid_data = animation_info->grid_data;

			// wrap the index
			if (!ApplyWrapMode(grid_index, glm::ivec2(0, 0), grid_data.grid_size - glm::ivec2(1, 1), mode, grid_index)) // -1 because the max_range belongs to the wanted values
				return BitmapLayout();

			// check linear index and skip_lasts
			//
			// Indexing a grid with skip images may cause indexation of invalid cell
			//
			// +---------------+
			// |               |
			// |               |
			// |           A   |
			// |       +-------+  You access x, which cell is best to read instead ?
			// |     B |   x   |
			// +-------+       |
			// |               |
			// +---------------+
			//
			// there is no real best choice between A & B for clamp methods
			// BUT, a grid with skip image probably means you want to access cell in a 1D way (just an index)
			// for that reason, we transform the 2D grid coord into a 1D index coord and then apply the wrapping on that new index

			if (grid_data.skip_lasts > 0)
			{
				int index = grid_index.x + grid_index.y * grid_data.grid_size.x;

				int animation_count = (grid_data.grid_size.x * grid_data.grid_size.y) - grid_data.skip_lasts;

				if (index >= animation_count)
				{
					if (!ApplyWrapMode(index, 0, animation_count - 1, mode, index))
						return BitmapLayout();
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

		WrapMode BitmapInfo::GetEffectiveRequestWrapMode(WrapMode src) const
		{
			// mode is none, see in the animation itself if there is a default settings
			if (src == WrapMode::NONE)
			{
				if (animation_info != nullptr)
				{
					src = animation_info->default_wrap_mode;
					// still no mode by default, take clamp
					if (src == WrapMode::NONE)
						src = WrapMode::CLAMP; // do not use 'check_ranges' by default because it is more to detect error for an invisible sprite than for bad animation
				}
			}
			return src;
		}

		BitmapLayout BitmapInfo::GetAnimationLayoutFromTime(float time, WrapMode mode) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();

			float frame_duration = animation_info->GetFrameDuration();
			if (frame_duration <= 0.0f)
				return BitmapLayout();

			// transform time into an index
			int index = int(time / frame_duration);
			return GetAnimationLayout(index, mode);
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(int index, WrapMode mode) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();

			// get the mode to use
			mode = GetEffectiveRequestWrapMode(mode);

			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				return DoGetFrameAnimationLayout(index, mode);
			}
			else // grid base animation
			{
				BitmapGridAnimationInfo grid_data = animation_info->grid_data;

				// transform the index into a 2D coord on the grid
				int frame_count = animation_info->grid_data.GetFrameCount();

				if (!ApplyWrapMode(index, 0, frame_count - 1, mode, index))
					return BitmapLayout();

				glm::ivec2 grid_index;
 				grid_index.x = index % grid_data.grid_size.x;
				grid_index.y = index / grid_data.grid_size.x;
				return DoGetGridAnimationLayout(grid_index, mode);
			}
		}

		BitmapLayout BitmapInfo::GetAnimationLayout(glm::ivec2 const & grid_index, WrapMode mode) const
		{
			// non animated bitmap
			if (animation_info == nullptr)
				return BitmapLayout();

			// get the mode to use
			mode = GetEffectiveRequestWrapMode(mode);

			// frame base animation
			if (animation_info->IsFrameAnimation())
			{
				if (mode == WrapMode::CHECK_RANGES && grid_index.y != 0)
					return BitmapLayout();
				return DoGetFrameAnimationLayout(grid_index.x, mode);
			}
			else
			{
				return DoGetGridAnimationLayout(grid_index, mode);
			}
			return BitmapLayout();
		}

		int BitmapInfo::GetAnimationImageCount() const
		{
			if (animation_info == nullptr)
				return 0;
			if (animation_info->IsFrameAnimation())
				return animation_info->child_frame_count + 1; // +1 for the very first image

			assert(animation_info->IsGridAnimation());
			return std::max((animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts, 0);
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
			AtlasBaseTemplate<BitmapInfo, FontInfo, FolderInfo, Object>::Clear();
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
			StreamTools::Whitespaces whitespaces(indent);

			stream << whitespaces << "Folder:" << '\n';
			NamedObject const & named_info = folder_info;
			DoOutputInfo(named_info, stream, indent);

			// output the bitmaps in the folder
			if (folder_info.bitmaps.size() > 0)
			{
				StreamTools::Whitespaces bitmap_stream_indent(indent + INDENT_VALUE);
				stream << bitmap_stream_indent << "Bitmaps:" << '\n';
				for (BitmapInfo const & bitmap_info : folder_info.bitmaps)
					DoOutputInfo(bitmap_info, stream, indent + INDENT_VALUE);
			}
			// output the fonts in the folder
			if (folder_info.fonts.size() > 0)
			{
				StreamTools::Whitespaces font_stream_indent(indent + INDENT_VALUE);
				stream << font_stream_indent << "Fonts:" << '\n';
				for (FontInfo const & font_info : folder_info.fonts)
					DoOutputInfo(font_info, stream, indent + INDENT_VALUE);
			}
			// recursive calls
			if (folder_info.folders.size() > 0)
			{
				StreamTools::Whitespaces folder_stream_indent(indent + INDENT_VALUE);
				stream << folder_stream_indent << "Child Folders:" << '\n';

				size_t count = folder_info.folders.size();
				for (size_t i = 0; i < count; ++i)
					DoOutputInfo(*folder_info.folders[i], stream, indent + INDENT_VALUE);
			}
		}

		void AtlasBase::DoOutputInfo(NamedObject const & info, std::ostream & stream, int indent)
		{
			StreamTools::Whitespaces whitespaces(indent);
			stream << whitespaces << "  name         : " << info.GetName() << '\n';
			stream << whitespaces << "  tag          : " << info.GetTag() << '\n';
		}


		void AtlasBase::DoOutputInfo(BitmapLayout const & info, std::ostream & stream, int indent)
		{
			StreamTools::Whitespaces whitespaces(indent);
			stream << whitespaces << "  bitmap_index          : " << info.bitmap_index << '\n';
			stream << whitespaces << "  width                 : " << info.width << '\n';
			stream << whitespaces << "  height                : " << info.height << '\n';
			stream << whitespaces << "  x                     : " << info.x << '\n';
			stream << whitespaces << "  y                     : " << info.y << '\n';
			stream << whitespaces << "  bottomleft_texcoord.x : " << info.bottomleft_texcoord.x << '\n';
			stream << whitespaces << "  bottomleft_texcoord.y : " << info.bottomleft_texcoord.y << '\n';
			stream << whitespaces << "  topright_texcoord.x   : " << info.topright_texcoord.x << '\n';
			stream << whitespaces << "  topright_texcoord.y   : " << info.topright_texcoord.y << '\n';
		}

		void AtlasBase::DoOutputInfo(CharacterLayout const & info, std::ostream & stream, int indent)
		{
			BitmapLayout const & bitmap_layout = info;
			DoOutputInfo(bitmap_layout, stream, indent);

			StreamTools::Whitespaces whitespaces(indent);
			stream << whitespaces << "  advance.x    : " << info.advance.x << '\n';
			stream << whitespaces << "  advance.y    : " << info.advance.y << '\n';
			stream << whitespaces << "  bitmap_left  : " << info.bitmap_left << '\n';
			stream << whitespaces << "  bitmap_top   : " << info.bitmap_top << '\n';
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
			DoOutputInfo(named_info, stream, indent);

			StreamTools::Whitespaces whitespaces(indent);
			stream << whitespaces << "  glyph_width  : " << info.glyph_width << '\n';
			stream << whitespaces << "  glyph_height : " << info.glyph_height << '\n';
			stream << whitespaces << "  ascender             : " << info.ascender << '\n';
			stream << whitespaces << "  descender            : " << info.descender << '\n';
			stream << whitespaces << "  face_height          : " << info.face_height << '\n';

			// output the charactars in the fonts
			if (info.elements.size() > 0)
			{
				stream << whitespaces << "Characters:" << '\n';
				for (CharacterInfo const & character_info : info.elements)
					DoOutputInfo(character_info, stream, indent + INDENT_VALUE);
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
				OutputAtlasSpaceOccupation(int(i), stream);
		}

		void AtlasBase::OutputAtlasSpaceOccupation(int bitmap_index, std::ostream & stream) const
		{
			float atlas_surface = (float)(dimension.x * dimension.y);
			float atlas_used_surface = ComputeSurface(bitmap_index);
			float percent = 100.0f * atlas_used_surface / atlas_surface;

			stream << "Atlas " << bitmap_index << '\n';
			stream << "  occupation : " << percent << "%" << '\n';
		}

		void AtlasBase::OutputGeneralInformation(std::ostream & stream) const
		{
			float atlas_surface = (float)(dimension.x * dimension.y);
			float full_surface = ComputeSurface(-1);
			int   min_atlas_count = int(std::ceil(full_surface / atlas_surface));

			stream << "Full used surface  : " << full_surface << '\n';
			stream << "Atlas surface      : " << atlas_surface << '\n';
			stream << "Best atlas count   : " << min_atlas_count << '\n';
			stream << "Actual atlas count : " << atlas_count << '\n';
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

				boost::filesystem::path dst_filename = target_dir / GetBitmapFilename(image_format, bitmap_filename, int(i));

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

					json["bitmaps"].push_back(GetBitmapFilename(image_format, bitmap_filename, int(i)).string());
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
			Buffer<char> buffer = FileTools::LoadFile(index_filename, LoadFileFlag::ASCII | LoadFileFlag::NO_ERROR_TRACE);
			if (buffer == nullptr)
			{
				Log::Error("Atlas::LoadAtlas: fail to load [%s]", index_filename.string().c_str());
				return false;
			}

			// parse JSON file
			nlohmann::json json;
			if (JSONTools::Parse(buffer.data, json))
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
				for (nlohmann::json const & json_filename : *json_files)
				{
					std::string filename;
					chaos::LoadFromJSON(json_filename, filename);

					FIBITMAP * bitmap = ImageTools::LoadImageFromFile(target_dir / filename);
					if (bitmap == nullptr)
					{
						result = false;
						break;
					}

					int width = int(FreeImage_GetWidth(bitmap));
					int height = int(FreeImage_GetHeight(bitmap));
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
					atlas_count = int(bitmaps.size());
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

		bool SaveIntoJSON(nlohmann::json & json, BitmapAnimationInfo const & src)
		{
			return SaveIntoJSON(json, (ImageAnimationDescription const &)src);
		}

		bool LoadFromJSON(nlohmann::json const & json, BitmapAnimationInfo & dst)
		{
			return LoadFromJSON(json, (ImageAnimationDescription &)dst);
		}

		bool SaveIntoJSON(nlohmann::json & json, BitmapLayout const & src)
		{
			if (!json.is_object())
				json = nlohmann::json::object();

			JSONTools::SetAttribute(json, "bitmap_index", src.bitmap_index);
			JSONTools::SetAttribute(json, "x", src.x);
			JSONTools::SetAttribute(json, "y", src.y);
			JSONTools::SetAttribute(json, "width", src.width);
			JSONTools::SetAttribute(json, "height", src.height);
			JSONTools::SetAttribute(json, "bottomleft_texcoord", src.bottomleft_texcoord);
			JSONTools::SetAttribute(json, "topright_texcoord", src.topright_texcoord);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, BitmapLayout & dst)
		{
			if (!json.is_object())
				return false;
			JSONTools::GetAttribute(json, "bitmap_index", dst.bitmap_index);
			JSONTools::GetAttribute(json, "x", dst.x);
			JSONTools::GetAttribute(json, "y", dst.y);
			JSONTools::GetAttribute(json, "width", dst.width);
			JSONTools::GetAttribute(json, "height", dst.height);
			JSONTools::GetAttribute(json, "bottomleft_texcoord", dst.bottomleft_texcoord);
			JSONTools::GetAttribute(json, "topright_texcoord", dst.topright_texcoord);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json, CharacterLayout const & src)
		{
			SaveIntoJSON(json, (BitmapLayout const &)src); // call 'super' method (implies json = nlohmann::json::object();)

			JSONTools::SetAttribute(json, "advance_x", src.advance.x);
			JSONTools::SetAttribute(json, "advance_y", src.advance.y);
			JSONTools::SetAttribute(json, "bitmap_left", src.bitmap_left);
			JSONTools::SetAttribute(json, "bitmap_top", src.bitmap_top);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, CharacterLayout & dst)
		{
			if (!json.is_object())
				return false;

			LoadFromJSON(json, (BitmapLayout & )dst); // call 'super' method

			JSONTools::GetAttribute(json, "advance_x", dst.advance.x);
			JSONTools::GetAttribute(json, "advance_y", dst.advance.y);
			JSONTools::GetAttribute(json, "bitmap_left", dst.bitmap_left);
			JSONTools::GetAttribute(json, "bitmap_top", dst.bitmap_top);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json, BitmapInfo const & src)
		{
			SaveIntoJSON(json, (NamedObject const &)src);  // call 'super' method (implies json = nlohmann::json::object();)
			SaveIntoJSON(json, (BitmapLayout const &)src); // call 'super' method (implies json = nlohmann::json::object();)

			if (src.animation_info != nullptr)
			{
				json["animation_info"] = nlohmann::json::object();
				SaveIntoJSON(json["animation_info"], *src.animation_info);
			}
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, BitmapInfo & dst)
		{
			if (!json.is_object())
				return false;

			LoadFromJSON(json, (NamedObject & )dst); // call 'super' method
			LoadFromJSON(json, (BitmapLayout & )dst); // call 'super' method

			nlohmann::json const * animation_json = JSONTools::GetStructure(json, "animation_info");
			if (animation_json != nullptr)
			{
				dst.animation_info = new BitmapAnimationInfo;
				if (dst.animation_info != nullptr)
					LoadFromJSON(*animation_json, *dst.animation_info);
			}
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json, CharacterInfo const & src)
		{
			SaveIntoJSON(json, (NamedObject const &)src);     // call 'super' method (implies json = nlohmann::json::object();)
			SaveIntoJSON(json, (CharacterLayout const &)src); // call 'super' method (implies json = nlohmann::json::object();)
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, CharacterInfo & dst)
		{
			if (!json.is_object())
				return false;
			LoadFromJSON(json, (NamedObject &)dst); // call 'super' method
			LoadFromJSON(json, (CharacterLayout &)dst); // call 'super' method
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json, FontInfo const & src)
		{
			SaveIntoJSON(json, (NamedObject const &)src); // call 'super' method (implies json = nlohmann::json::object();)

			JSONTools::SetAttribute(json, "glyph_width", src.glyph_width);
			JSONTools::SetAttribute(json, "glyph_height", src.glyph_height);
			JSONTools::SetAttribute(json, "ascender", src.ascender);
			JSONTools::SetAttribute(json, "descender", src.descender);
			JSONTools::SetAttribute(json, "face_height", src.face_height);
			JSONTools::SetAttribute(json, "elements", src.elements);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, FontInfo & dst)
		{
			if (!json.is_object())
				return false;

			LoadFromJSON(json, (NamedObject & )dst); // call 'super' method

			JSONTools::GetAttribute(json, "glyph_width", dst.glyph_width);
			JSONTools::GetAttribute(json, "glyph_height", dst.glyph_height);
			JSONTools::GetAttribute(json, "ascender", dst.ascender);
			JSONTools::GetAttribute(json, "descender", dst.descender);
			JSONTools::GetAttribute(json, "face_height", dst.face_height);
			JSONTools::GetAttribute(json, "elements", dst.elements);
			return true;
		}

		bool SaveIntoJSON(nlohmann::json & json, FolderInfo const & src)
		{
			SaveIntoJSON(json, (NamedObject const &)src); // call 'super' method (implies json = nlohmann::json::object();)
			if (src.bitmaps.size())
				JSONTools::SetAttribute(json, "bitmaps", src.bitmaps);
			if (src.fonts.size())
				JSONTools::SetAttribute(json, "fonts", src.fonts);
			if (src.folders.size())
				JSONTools::SetAttribute(json, "folders", src.folders);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json, FolderInfo & dst)
		{
			if (!json.is_object())
				return false;
			LoadFromJSON(json, (NamedObject &)dst); // call 'super' method

			JSONTools::GetAttribute(json, "bitmaps", dst.bitmaps);
			JSONTools::GetAttribute(json, "fonts", dst.fonts);
			JSONTools::GetAttribute(json, "folders", dst.folders);
			return true;
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

