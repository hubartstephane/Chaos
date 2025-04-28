#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// AtlasBitmapLayout functions
	// ========================================================================

	ParticleTexcoords AtlasBitmapLayout::GetTexcoords() const
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
	glm::vec2 AtlasBitmapLayout::ApplyRatioToSize(glm::vec2 src) const
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
	// AtlasBitmapInfo functions
	// ========================================================================

	bool AtlasBitmapInfo::HasAnimation() const
	{
		if (animation_info == nullptr)
			return false;
		return true;
	}

	bool AtlasBitmapInfo::HasFrameAnimation() const
	{
		if (animation_info == nullptr)
			return false;
		return animation_info->IsFrameAnimation();
	}

	bool AtlasBitmapInfo::HasGridAnimation() const
	{
		if (animation_info == nullptr)
			return false;
		return animation_info->IsGridAnimation();
	}

	AtlasBitmapLayout AtlasBitmapInfo::DoGetFrameAnimationLayout(int index, WrapMode mode) const
	{
		// wrap the index
		if (!ApplyWrapMode(index, 0, animation_info->child_frame_count + 1 - 1, mode, index)) // the real number of frames is (child_frame_count + 1). So for clamping we use (child_frame_count + 1 - 1)
			return AtlasBitmapLayout();
		// find the bitmap further in the bitmapinfo array
		return this[static_cast<size_t>(index)];
	}

	AtlasBitmapLayout AtlasBitmapInfo::DoGetGridAnimationLayout(glm::ivec2 grid_index, WrapMode mode) const
	{
		BitmapGridAnimationInfo grid_data = animation_info->grid_data;

		// wrap the index
		if (!ApplyWrapMode(grid_index, glm::ivec2(0, 0), grid_data.grid_size - glm::ivec2(1, 1), mode, grid_index)) // -1 because the max_range belongs to the wanted values
			return AtlasBitmapLayout();

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
					return AtlasBitmapLayout();
				grid_index.x = (index % grid_data.grid_size.x);
				grid_index.y = (index / grid_data.grid_size.x);
			}
		}

		// XXX : for OpenGL, texture Y = 0 is bottom
		//       for bitmap,         Y = 0 is top
		//       => explains why (1. - Y)
		//
		// XXX : grid_index is top = 0 to bottom

		AtlasBitmapLayout result;
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

	WrapMode AtlasBitmapInfo::GetEffectiveRequestWrapMode(WrapMode src) const
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

	AtlasBitmapLayout AtlasBitmapInfo::GetAnimationLayoutFromTime(float time, WrapMode mode) const
	{
		// non animated bitmap
		if (animation_info == nullptr)
			return AtlasBitmapLayout();

		float frame_duration = animation_info->GetFrameDuration();
		if (frame_duration <= 0.0f)
			return AtlasBitmapLayout();

		// transform time into an index
		int index = int(time / frame_duration);
		return GetAnimationLayout(index, mode);
	}

	AtlasBitmapLayout AtlasBitmapInfo::GetAnimationLayout(int index, WrapMode mode) const
	{
		// non animated bitmap
		if (animation_info == nullptr)
			return AtlasBitmapLayout();

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
				return AtlasBitmapLayout();

			glm::ivec2 grid_index;
			grid_index.x = index % grid_data.grid_size.x;
			grid_index.y = index / grid_data.grid_size.x;
			return DoGetGridAnimationLayout(grid_index, mode);
		}
	}

	AtlasBitmapLayout AtlasBitmapInfo::GetAnimationLayout(glm::ivec2 const& grid_index, WrapMode mode) const
	{
		// non animated bitmap
		if (animation_info == nullptr)
			return AtlasBitmapLayout();

		// get the mode to use
		mode = GetEffectiveRequestWrapMode(mode);

		// frame base animation
		if (animation_info->IsFrameAnimation())
		{
			if (mode == WrapMode::CHECK_RANGES && grid_index.y != 0)
				return AtlasBitmapLayout();
			return DoGetFrameAnimationLayout(grid_index.x, mode);
		}
		else
		{
			return DoGetGridAnimationLayout(grid_index, mode);
		}
		return AtlasBitmapLayout();
	}

	int AtlasBitmapInfo::GetAnimationImageCount() const
	{
		if (animation_info == nullptr)
			return 0;
		if (animation_info->IsFrameAnimation())
			return animation_info->child_frame_count + 1; // +1 for the very first image

		assert(animation_info->IsGridAnimation());
		return std::max((animation_info->grid_data.grid_size.x * animation_info->grid_data.grid_size.y) - animation_info->grid_data.skip_lasts, 0);
	}

	float AtlasBitmapInfo::GetAnimationDuration() const
	{
		if (animation_info == nullptr)
			return 0.0f;
		return animation_info->GetAnimationDuration();
	}

	float AtlasBitmapInfo::GetFrameDuration() const
	{
		if (animation_info == nullptr)
			return 0.0f;
		return animation_info->GetFrameDuration();
	}

	// ========================================================================
	// AtlasFolderInfo functions
	// ========================================================================

	void AtlasFolderInfo::DoCollectEntries(std::vector<AtlasBitmapInfo>& result)
	{
		size_t count = bitmaps.size();

		result.reserve(result.size() + count);
		for (size_t i = 0; i < count; ++i)
			result.push_back(bitmaps[i]);
	}

	void AtlasFolderInfo::DoCollectEntries(std::vector<AtlasCharacterInfo>& result)
	{
		size_t font_count = fonts.size();
		for (size_t i = 0; i < font_count; ++i)
		{
			font_type const& font_info = fonts[i];

			size_t count = font_info.elements.size();

			result.reserve(result.size() + count);
			for (size_t j = 0; j < count; ++j)
				result.push_back(font_info.elements[j]);
		}
	}

	void AtlasFolderInfo::DoCollectEntries(std::vector<AtlasBitmapLayout>& result, bool collect_bitmaps, bool collect_characters)
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
				font_type const& font_info = fonts[i];

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
		AtlasBaseTemplate<Object, AtlasBitmapInfo, AtlasFontInfo, AtlasFolderInfo>::Clear();
	}

	float AtlasBase::ComputeSurface(int bitmap_index) const
	{
		return DoComputeSurface(bitmap_index, &root_folder);
	}

	float AtlasBase::DoComputeSurface(int bitmap_index, AtlasFolderInfo const* folder_info) const
	{
		// XXX : ignore child entries. Their surface is already considered by their parent entry

		if (folder_info == nullptr)
			return 0.0f;

		float result = 0.0f;
		// surface for the bitmaps in the folder
		for (AtlasBitmapInfo const& bitmap_info : folder_info->bitmaps)
			if (bitmap_info.bitmap_index == bitmap_index || bitmap_index < 0)
				result += (float)(bitmap_info.width * bitmap_info.height);
		// surface for the fonts in the folder
		for (AtlasFontInfo const& font_info : folder_info->fonts)
			for (AtlasCharacterInfo const& character_info : font_info.elements)
				if (character_info.bitmap_index == bitmap_index || bitmap_index < 0)
					result += (float)(character_info.width * character_info.height);
		// recursive calls
		size_t count = folder_info->folders.size();
		for (size_t i = 0; i < count; ++i)
			result += DoComputeSurface(bitmap_index, folder_info->folders[i].get());
		return result;
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

	bool Atlas::SaveAtlas(FilePathParam const& path) const
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

	bool Atlas::SaveAtlasBitmaps(boost::filesystem::path const& target_dir, boost::filesystem::path const& index_filename, boost::filesystem::path const& bitmap_filename) const
	{
		bool result = true;
		// save them
		size_t count = bitmaps.size();
		for (size_t i = 0; (i < count) && result; ++i)
		{
			FIBITMAP* image = bitmaps[i].get();
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

	bool Atlas::SaveAtlasIndex(boost::filesystem::path const& target_dir, boost::filesystem::path const& index_filename, boost::filesystem::path const& bitmap_filename) const
	{
		// generate a file for the index (JSON format)
		std::ofstream stream(index_filename.string().c_str());
		if (stream)
		{
			nlohmann::json json;
			// insert the files
			json["bitmaps"] = nlohmann::json::array();

			size_t count = bitmaps.size();
			for (size_t i = 0; i < count; ++i)
			{
				FIBITMAP* image = bitmaps[i].get();
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
			SaveIntoJSON(&json["root_folder"], root_folder);

			// format the JSON into string and insert it into stream
			stream << json.dump(4);
			return true;
		}
		return false;
	}

	bool Atlas::LoadAtlas(FilePathParam const& path)
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
			BitmapAtlasLog::Error("Atlas::LoadAtlas: fail to load [%s]", index_filename.string().c_str());
			return false;
		}

		// parse JSON file
		nlohmann::json json;
		if (JSONTools::Parse(buffer.data, json))
			return LoadAtlas(&json, target_dir);
		return false;
	}

	bool Atlas::LoadAtlas(nlohmann::json const* json, boost::filesystem::path const& target_dir)
	{
		bool result = true;

		// clean the object
		Clear();

		// load the files
		if (nlohmann::json const* json_files = JSONTools::GetAttributeStructureNode(json, "bitmaps"))
		{
			for (nlohmann::json const& json_filename : *json_files)
			{
				std::string filename;
				chaos::LoadFromJSON(&json_filename, filename);

				FIBITMAP* bitmap = ImageTools::LoadImageFromFile(target_dir / filename);
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
		char const* extension = "png"; // by default
		if (image_format == FIF_PNG)
			extension = "png";
		else if (image_format == FIF_EXR)
			extension = "exr";

		char buffer[30]; // far big enough
		sprintf_s(buffer, "_%d.%s", index, extension);
		return bitmap_filename.concat(buffer);
	}

	void Atlas::SplitFilename(FilePathParam const& path, boost::filesystem::path& target_dir, boost::filesystem::path& index_filename, boost::filesystem::path& bitmap_filename) const
	{
		boost::filesystem::path const& resolved_path = path.GetResolvedPath();

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

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapAnimationInfo const& src)
	{
		return SaveIntoJSON(json, (ImageAnimationDescription const&)src);
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapAnimationInfo& dst)
	{
		return LoadFromJSON(config, (ImageAnimationDescription&)dst);
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapLayout const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "bitmap_index", src.bitmap_index);
		JSONTools::SetAttribute(json, "x", src.x);
		JSONTools::SetAttribute(json, "y", src.y);
		JSONTools::SetAttribute(json, "width", src.width);
		JSONTools::SetAttribute(json, "height", src.height);
		JSONTools::SetAttribute(json, "bottomleft_texcoord", src.bottomleft_texcoord);
		JSONTools::SetAttribute(json, "topright_texcoord", src.topright_texcoord);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapLayout& dst)
	{
		JSONTools::GetAttribute(config, "bitmap_index", dst.bitmap_index);
		JSONTools::GetAttribute(config, "x", dst.x);
		JSONTools::GetAttribute(config, "y", dst.y);
		JSONTools::GetAttribute(config, "width", dst.width);
		JSONTools::GetAttribute(config, "height", dst.height);
		JSONTools::GetAttribute(config, "bottomleft_texcoord", dst.bottomleft_texcoord);
		JSONTools::GetAttribute(config, "topright_texcoord", dst.topright_texcoord);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasCharacterLayout const& src)
	{
		if (!SaveIntoJSON(json, (AtlasBitmapLayout const&)src)) // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		JSONTools::SetAttribute(json, "advance_x", src.advance.x);
		JSONTools::SetAttribute(json, "advance_y", src.advance.y);
		JSONTools::SetAttribute(json, "bitmap_left", src.bitmap_left);
		JSONTools::SetAttribute(json, "bitmap_top", src.bitmap_top);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasCharacterLayout& dst)
	{
		if (!LoadFromJSON(config, (AtlasBitmapLayout&)dst)) // call 'super' method
			return false;
		JSONTools::GetAttribute(config, "advance_x", dst.advance.x);
		JSONTools::GetAttribute(config, "advance_y", dst.advance.y);
		JSONTools::GetAttribute(config, "bitmap_left", dst.bitmap_left);
		JSONTools::GetAttribute(config, "bitmap_top", dst.bitmap_top);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasBitmapInfo const& src)
	{
		if (!SaveIntoJSON(json, (NamedInterface const&)src))  // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		if (!SaveIntoJSON(json, (AtlasBitmapLayout const&)src)) // call 'super' method (implies json = nlohmann::json::object();)
			return false;

		if (src.animation_info != nullptr)
			JSONTools::SetAttribute(json, "animation_info", *src.animation_info);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasBitmapInfo& dst)
	{
		if (!LoadFromJSON(config, (NamedInterface&)dst)) // call 'super' method
			return false;
		if (!LoadFromJSON(config, (AtlasBitmapLayout&)dst)) // call 'super' method
			return false;
		if (JSONReadConfiguration animation_config = JSONTools::GetAttributeStructureNode(config, "animation_info"))
		{
			dst.animation_info = new AtlasBitmapAnimationInfo;
			if (dst.animation_info != nullptr)
				LoadFromJSON(animation_config, *dst.animation_info);
		}
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasCharacterInfo const& src)
	{
		if (!SaveIntoJSON(json, (NamedInterface const&)src))     // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		if (!SaveIntoJSON(json, (AtlasCharacterLayout const&)src)) // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasCharacterInfo& dst)
	{
		if (!LoadFromJSON(config, (NamedInterface&)dst)) // call 'super' method
			return false;
		if (!LoadFromJSON(config, (AtlasCharacterLayout&)dst)) // call 'super' method
			return false;
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasFontInfo const& src)
	{
		if (!SaveIntoJSON(json, (NamedInterface const&)src)) // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		JSONTools::SetAttribute(json, "glyph_width", src.glyph_width);
		JSONTools::SetAttribute(json, "glyph_height", src.glyph_height);
		JSONTools::SetAttribute(json, "ascender", src.ascender);
		JSONTools::SetAttribute(json, "descender", src.descender);
		JSONTools::SetAttribute(json, "face_height", src.face_height);
		JSONTools::SetAttribute(json, "elements", src.elements);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasFontInfo& dst)
	{
		if (!LoadFromJSON(config, (NamedInterface&)dst)) // call 'super' method
			return false;
		JSONTools::GetAttribute(config, "glyph_width", dst.glyph_width);
		JSONTools::GetAttribute(config, "glyph_height", dst.glyph_height);
		JSONTools::GetAttribute(config, "ascender", dst.ascender);
		JSONTools::GetAttribute(config, "descender", dst.descender);
		JSONTools::GetAttribute(config, "face_height", dst.face_height);
		JSONTools::GetAttribute(config, "elements", dst.elements);
		return true;
	}

	bool DoSaveIntoJSON(nlohmann::json* json, AtlasFolderInfo const& src)
	{
		if (!SaveIntoJSON(json, (NamedInterface const&)src)) // call 'super' method (implies json = nlohmann::json::object();)
			return false;
		if (src.bitmaps.size())
			JSONTools::SetAttribute(json, "bitmaps", src.bitmaps);
		if (src.fonts.size())
			JSONTools::SetAttribute(json, "fonts", src.fonts);
		if (src.folders.size())
			JSONTools::SetAttribute(json, "folders", src.folders);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, AtlasFolderInfo& dst)
	{
		if (!LoadFromJSON(config, (NamedInterface&)dst)) // call 'super' method
			return false;
		JSONTools::GetAttribute(config, "bitmaps", dst.bitmaps);
		JSONTools::GetAttribute(config, "fonts", dst.fonts);
		JSONTools::GetAttribute(config, "folders", dst.folders);
		return true;
	}

}; // namespace chaos

