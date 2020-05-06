#include <chaos/BitmapAtlasInput.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ImageTools.h>
#include <chaos/FontTools.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/JSONTools.h>



namespace chaos
{
	namespace BitmapAtlas
	{

		// ========================================================================
		// BitmapInfoInputAnimationDescription : an utility class for JSON files coming as image descriptors
		// ========================================================================

		class BitmapInfoInputAnimationDescription
		{
		public:

			/** if the animation is stored inside a grid */
			BitmapGridAnimationInfo grid_data;
			/** the duration of a frame in seconds */
			float frame_duration = -1.0f;
			/** the duration of a the whole animation in seconds */
			float anim_duration = -1.0f;
			/** the default wrap mode */
			WrapMode default_wrap_mode = WrapMode::none; // let the code decide in a single location (see  BitmapInfo::GetEffectiveRequestWrapMode(...))
			/** the directory path that contains the child images */
			boost::filesystem::path images_path;
		};

		bool SaveIntoJSON(nlohmann::json & json_entry, BitmapInfoInputAnimationDescription const & src)
		{
			if (!json_entry.is_object())
				json_entry = nlohmann::json::object();
			JSONTools::SetAttribute(json_entry, "grid_data", src.grid_data);
			JSONTools::SetAttribute(json_entry, "images_path", src.images_path);
			JSONTools::SetAttribute(json_entry, "frame_duration", src.frame_duration);
			JSONTools::SetAttribute(json_entry, "anim_duration", src.anim_duration);
			JSONTools::SetAttribute(json_entry, "default_wrap_mode", src.default_wrap_mode);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, BitmapInfoInputAnimationDescription & dst)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "grid_data", dst.grid_data);
			JSONTools::GetAttribute(json_entry, "images_path", dst.images_path);
			JSONTools::GetAttribute(json_entry, "frame_duration", dst.frame_duration);
			JSONTools::GetAttribute(json_entry, "anim_duration", dst.anim_duration);
			JSONTools::GetAttribute(json_entry, "default_wrap_mode", dst.default_wrap_mode);
			return true;
		}

		// ========================================================================
		// ObjectBaseInput implementation
		// ========================================================================

		void ObjectBaseInput::RegisterResource(FIBITMAP * bitmap, bool release)
		{
			atlas_input->RegisterResource(bitmap, release);
		}

		void ObjectBaseInput::RegisterResource(FIMULTIBITMAP * multi_bitmap, bool release)
		{
			atlas_input->RegisterResource(multi_bitmap, release);
		}

		void ObjectBaseInput::RegisterResource(FT_Library library, bool release)
		{
			atlas_input->RegisterResource(library, release);
		}

		void ObjectBaseInput::RegisterResource(FT_Face face, bool release)
		{
			atlas_input->RegisterResource(face, release);
		}

		// ========================================================================
		// FolderInfoInput implementation
		// ========================================================================

		FolderInfoInput * FolderInfoInput::AddFolder(char const * name, TagType tag)
		{
			assert(name != nullptr);

			FolderInfoInput * result = GetFolderInfo(name);
			if (result == nullptr)
			{
				result = new FolderInfoInput;
				if (result != nullptr)
				{
					result->atlas_input = atlas_input;
					result->name = name;
					result->tag = tag;
					folders.push_back(std::move(std::unique_ptr<FolderInfoInput>(result)));
				}
			}
			return result;
		}

		FontInfoInput * FolderInfoInput::AddFont(char const * font_name, FT_Library library, bool release_library, char const * name, TagType tag, FontInfoInputParams const & params)
		{
			assert(font_name != nullptr);

			// create a library if necessary
			if (library == nullptr)
			{
				FT_Error error = FT_Init_FreeType(&library);
				if (error)
					return nullptr;
				release_library = true;
			}

			// load the face and set pixel size
			FT_Face face = nullptr;

			Buffer<char> buffer = FileTools::LoadFile(font_name, false); // for direct access to resource directory
			if (buffer != nullptr)
				FT_New_Memory_Face(library, (FT_Byte const *)buffer.data, (FT_Long)buffer.bufsize, 0, &face);

			if (face == nullptr)
			{
				if (release_library)
					FT_Done_FreeType(library); // delete library if necessary
				return nullptr;
			}			
			return AddFontImpl(library, face, release_library, true, name, tag, params);
		}

		FontInfoInput * FolderInfoInput::AddFont(FT_Face face, bool release_face, char const * name, TagType tag, FontInfoInputParams const & params)
		{
			return AddFontImpl(nullptr, face, false, release_face, name, tag, params);
		}

		FontInfoInput * FolderInfoInput::AddFontImpl(FT_Library library, FT_Face face, bool release_library, bool release_face, char const * name, TagType tag, FontInfoInputParams const & params)
		{
			assert(name != nullptr);
			assert(face != nullptr);

			// search whether the font already exists
			if (GetFontInfo(name) != nullptr)
				return nullptr;

			FontInfoInput * result = new FontInfoInput;
			if (result == nullptr)
				return nullptr;

			// set font size
			// XXX : order is important. Face.size.metrics will not be initialized elsewhere
			FT_Error error = FT_Set_Pixel_Sizes(face, params.max_character_width, params.max_character_height);
			if (error != 0)
				return nullptr;

			// new character set input
			result->atlas_input = atlas_input;
			result->name = name;
			result->max_character_width = params.max_character_width;
			result->max_character_height = params.max_character_height;
			result->ascender = face->size->metrics.ascender / 64;     // take the FT_Pixel_Size(...) into consideration
			result->descender = face->size->metrics.descender / 64;   // take the FT_Pixel_Size(...) into consideration 
			result->face_height = face->size->metrics.height / 64;    // take the FT_Pixel_Size(...) into consideration

			RegisterResource(library, release_library);
			RegisterResource(face, release_face);

			// generate glyph cache

			// if user does not provide a list of charset for the fonts, use this hard coded one
			static char const * DEFAULT_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

			char const * characters = (params.characters.length() > 0) ?
				params.characters.c_str() :
				DEFAULT_CHARACTERS;

			std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache = FontTools::GetGlyphCacheForString(face, characters);

			// transforms each info of the glyph map into a bitmap
			for (auto & glyph : glyph_cache)
			{
				int w = glyph.second.bitmap_glyph->bitmap.width;
				int h = glyph.second.bitmap_glyph->bitmap.rows;

				FIBITMAP * bitmap = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap, PixelFormatType::RGBA);
				if (bitmap != nullptr || w <= 0 || h <= 0)  // if bitmap is zero sized (whitespace, the allocation failed). The info is still interesting                                          
				{
					char name[] = " ";
					sprintf_s(name, 2, "%c", glyph.first);

					CharacterInfoInput * info = new CharacterInfoInput;
					if (info == nullptr)
						continue;
					info->atlas_input = atlas_input;
					info->name = name;
					info->tag = glyph.first;
					if (bitmap != nullptr)
						info->description = ImageTools::GetImageDescription(bitmap);
					info->advance = glyph.second.advance;         // take the FT_Pixel_Size(...) into consideration
					info->bitmap_left = glyph.second.bitmap_left; // take the FT_Pixel_Size(...) into consideration
					info->bitmap_top = glyph.second.bitmap_top;   // take the FT_Pixel_Size(...) into consideration
					result->elements.push_back(std::move(std::unique_ptr<CharacterInfoInput>(info)));

					RegisterResource(bitmap, true);
				}
			}

			// release the glyph cache 
			for (auto & glyph : glyph_cache)
				FT_Done_Glyph((FT_Glyph)glyph.second.bitmap_glyph);

			fonts.push_back(std::move(std::unique_ptr<FontInfoInput>(result)));

			return result;
		}


        void FolderInfoInput::AddBitmapFilesData::SearchDirectoryEntries(FilePathParam const& path, bool search_files, bool search_directories)
        {
            // nothing more to search
            if ((files_searched || !search_files) && (directories_searched || !search_directories))
                return;

            // iterate over wanted directory
            boost::filesystem::path const& resolved_path = path.GetResolvedPath();

            boost::filesystem::directory_iterator end;
            for (boost::filesystem::directory_iterator it = FileTools::GetDirectoryIterator(resolved_path); it != end; ++it)
            {
                boost::filesystem::path const & p = it->path();

                boost::filesystem::file_status status = boost::filesystem::status(p);

                if (!files_searched && search_files && (status.type() == boost::filesystem::file_type::regular_file))
                    files.push_back(p);
                else if (!directories_searched && search_directories && (status.type() == boost::filesystem::file_type::directory_file))
                    directories.push_back(p);
            }
            files_searched |= search_files;
            directories_searched |= search_directories;
        }

		bool FolderInfoInput::AddBitmapFilesFromDirectory(FilePathParam const & path, bool recursive)
		{
            AddBitmapFilesData add_data;
            add_data.SearchDirectoryEntries(path, true, recursive);

			// step 1 : the files
			for (boost::filesystem::path const & p : add_data.files)
			{
				// skip already handled path
				if (std::find(add_data.ignore_files.begin(), add_data.ignore_files.end(), p) != add_data.ignore_files.end())
					continue;					
				// add bitmap
				AddBitmapImpl(p, nullptr, 0, add_data);
			}

			// step 2 : the directories
			for (boost::filesystem::path const & p : add_data.directories)
			{
				// skip already handled path
				if (std::find(add_data.ignore_directories.begin(), add_data.ignore_directories.end(), p) != add_data.ignore_directories.end())
					continue;
				// recurse
				FolderInfoInput * child_folder = AddFolder(BoostTools::PathToName(p).c_str(), 0);
				if (child_folder == nullptr)
					continue;
				child_folder->AddBitmapFilesFromDirectory(p, recursive);
			}
			return true;
		}

		BitmapInfoInput * FolderInfoInput::AddBitmap(FilePathParam const & path, char const * name, TagType tag)
		{
			return AddBitmapImpl(path, name, tag, AddBitmapFilesData());
		}

        static std::vector<FIBITMAP*> FindManifestChildImages(boost::filesystem::path const& directory_path)
        {
            std::vector<FIBITMAP*> child_images;
            std::vector<size_t> child_path_index;

            // search the images (and their path)
            boost::filesystem::directory_iterator end;
            for (boost::filesystem::directory_iterator it = FileTools::GetDirectoryIterator(directory_path); it != end; ++it)
            {
                boost::filesystem::path const& p = it->path();

                FIBITMAP* image = ImageTools::LoadImageFromFile(p);
                if (image != nullptr)
                {
                    child_images.push_back(image);

                    boost::filesystem::path noext_path = p;
                    noext_path.replace_extension();
                    child_path_index.push_back(StringTools::GetStringNumSuffix(noext_path.string())); // only store the filename without the extension
                }
            }

            // sort the images by path
            if (child_images.size() > 0)
            {
                // create an indirection vector (sort by file suffix myimage_123)
                boost::integer_range<size_t> range = boost::integer_range<size_t>(0, child_images.size());
                std::vector<size_t> indirection = std::vector<size_t>(range.begin(), range.end());

                std::sort(indirection.begin(), indirection.end(), [&child_path_index](size_t i1, size_t i2) {

                    return child_path_index[i1] < child_path_index[i2];
                });

                // reorder the images according to the indirection array
                std::vector<FIBITMAP*> result = child_images;
                for (size_t i = 0; i < result.size(); ++i)
                    result[i] = child_images[indirection[i]];
                return result;
            }
           return child_images;
        }

        BitmapInfoInput* FolderInfoInput::AddBitmapImpl(FilePathParam const& path, char const* name, TagType tag, AddBitmapFilesData& add_data)
        {
            // compute a name from the path if necessary
            boost::filesystem::path const& resolved_path = path.GetResolvedPath();

            // JSON manifest for a file or a directory
            if (FileTools::IsTypedFile(path, "json"))
            {
                // load the manifest
                nlohmann::json json_manifest;
                if (!JSONTools::LoadJSONFile(path, json_manifest, false))
                    return nullptr;

                // search whether a related file/directory exists
                add_data.SearchDirectoryEntries(path, true, true);

                // search whether there is a corresponding directory for the manifest
                boost::filesystem::path noext_path = resolved_path;
                noext_path.replace_extension();
                if (std::find(add_data.directories.begin(), add_data.directories.end(), noext_path) != add_data.directories.end())
                {
                    add_data.ignore_directories.push_back(noext_path);

                    std::vector<FIBITMAP*> child_images = FindManifestChildImages(noext_path);
                    if (child_images.size() > 0)
                        return AddBitmapWithManifestImpl(path, name, tag, &json_manifest, &child_images);
                }
                // search whether there is a corresponding file for the manifest
                else
                {
                    for (boost::filesystem::path const& p : add_data.files)
                    {
                        if (p == resolved_path) // ignore the manifest itself
                            continue;
                        boost::filesystem::path other_path = p;
                        other_path.replace_extension();
                        if (other_path == noext_path) // other file has same name (without extension)
                        {
                            add_data.ignore_files.push_back(p);
                            return AddBitmapWithManifestImpl(p, name, tag, &json_manifest, nullptr);
                        }
                    }
                }
                return nullptr;
            }
            // normal file
            else            
            {
                // search whether a manifest for the file exists
                nlohmann::json json_manifest;

                boost::filesystem::path json_path = resolved_path;
                json_path.replace_extension("json");
                JSONTools::LoadJSONFile(json_path, json_manifest, false);

                // do not individually load the manifest in recursive calls
                add_data.ignore_files.push_back(json_path);

                return AddBitmapWithManifestImpl(path, name, tag, json_manifest.empty() ? nullptr : &json_manifest, nullptr);
            }
        }

        BitmapInfoInput* FolderInfoInput::AddBitmapWithManifestImpl(FilePathParam const& path, char const* name, TagType tag, nlohmann::json const * json_manifest, std::vector<FIBITMAP*>* images)
        {
			// compute a name from the path if necessary
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();

			// test whether there is a grid describing the animation ... even if the grid_info is discarded due to manifest, we want to compute the final name with truncated suffixes
			std::string animated_name;

			ImageAnimationDescription animation_description;
			BitmapGridAnimationInfo::ParseFromName(resolved_path.string().c_str(), animation_description.grid_data, &animated_name);

			// search the name if not provided
			std::string generated_name;
			if (name == nullptr)
			{
				if (!animated_name.empty())
					generated_name = BoostTools::PathToName(animated_name);				
				else
					generated_name = BoostTools::PathToName(resolved_path);
				name = generated_name.c_str();
			}

            // test whether the object already exists
            if (GetBitmapInfo(name) != nullptr)
                return nullptr;

            // search if there is a JSON file to describe an animation
            BitmapInfoInputAnimationDescription manifest_animation_description;
            if (json_manifest != nullptr)
                LoadFromJSON(*json_manifest, manifest_animation_description);

			// load all pages for the bitmap
            std::vector<FIBITMAP*> pages;            
            if (images == nullptr)
            {
                pages = ImageTools::LoadMultipleImagesFromFile(path, &animation_description); // extract frame_rate from META DATA
                images = &pages;
            }
			// error case
			size_t count = images->size();
			if (images->size() == 0)
				return nullptr;

			// prefere JSON settings to name encoded values or GIF meta data for frame rate
			if (manifest_animation_description.anim_duration > 0.0f)
			{
				animation_description.frame_duration = -1; // XXX : erase data that can be found in the META data of the image, because this would lead to the 'anim_duration' being ignored at run time
				animation_description.anim_duration = manifest_animation_description.anim_duration;
			}
            if (manifest_animation_description.frame_duration > 0.0f)
                animation_description.frame_duration = manifest_animation_description.frame_duration;

            if (manifest_animation_description.grid_data.GetFrameCount() > 0)
                animation_description.grid_data = manifest_animation_description.grid_data;

			animation_description.default_wrap_mode = manifest_animation_description.default_wrap_mode; // default_wrap_mode is nor encoded into file nor in metadata

            // register resources for destructions			
			for (size_t i = 0; i < count; ++i)
				RegisterResource(images->operator[](i), true);

			// create the bitmap
			return AddBitmapImpl(*images, name, tag, &animation_description);
		}

		BitmapInfoInput * FolderInfoInput::AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag)
		{
			// prepare resource for destruction (in case of failure, there will not be memory leak)
			RegisterResource(bitmap, release_bitmap);

			// test whether the object already exists
			if (GetBitmapInfo(name) != nullptr)
				return nullptr;

			// make the insertion
			return AddBitmapImpl({ bitmap }, name, tag, nullptr);
		}

		BitmapInfoInput * FolderInfoInput::AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_animated_bitmap, char const * name, TagType tag)
		{
			// prepare resource for destruction (in case of failure, there will not be memory leak)
			RegisterResource(animated_bitmap, release_animated_bitmap);

			// test whether the object already exists
			if (GetBitmapInfo(name) != nullptr)
				return nullptr;

			// load all pages for the bitmap
			ImageAnimationDescription animation_description;

			std::vector<FIBITMAP *> pages = ImageTools::GetMultiImagePages(animated_bitmap, &animation_description); // extract frame_rate from META DATA

			size_t count = pages.size();
			for (size_t i = 0; i < count; ++i)
				RegisterResource(pages[i], true);

			// make the insertion
			return AddBitmapImpl(pages, name, tag, &animation_description);
		}

		BitmapInfoInput * FolderInfoInput::AddBitmapImpl(std::vector<FIBITMAP *> pages, char const * name, TagType tag, ImageAnimationDescription const * animation_description)
		{
			// create the result
			BitmapInfoInput * result = new BitmapInfoInput;
			if (result == nullptr)
				return nullptr;

			// set the surface description
			size_t page_count = pages.size();

			// finalize the result
			result->description = ImageTools::GetImageDescription(pages[0]);
			result->atlas_input = atlas_input;
			result->name = name;
			result->tag = tag;

			// insert result into the folder
			bitmaps.push_back(std::move(std::unique_ptr<BitmapInfoInput>(result))); // move for std::string copy
			result = bitmaps.back().get();

			// insert child animation frames
			if ((animation_description != nullptr && animation_description->grid_data.GetFrameCount() > 0) || page_count > 1)
			{
				BitmapAnimationInfoInput * animation_info = new BitmapAnimationInfoInput;
				if (animation_info != nullptr)
				{
					// give animation to the input
					if (animation_description != nullptr)
						animation_info->animation_description = *animation_description;
					result->animation_info = animation_info;
		
					// animated images with frames						
					if (page_count > 1)
					{
						for (size_t i = 1; i < page_count; ++i)
						{
							// create the child frame
							BitmapInfoInput * child_frame = new BitmapInfoInput; 
							if (child_frame == nullptr)
								continue;
							// insert the child frames inside the folder (as unamed, untagged)
							child_frame->atlas_input = atlas_input;
							child_frame->description = ImageTools::GetImageDescription(pages[i]);
							bitmaps.push_back(std::move(std::unique_ptr<BitmapInfoInput>(child_frame)));
							// insert the child frame inside the animation block
							animation_info->child_frames.push_back(child_frame);							
						}
					}
				}
			}
			return result;
		}

		// ========================================================================
		// AtlasInput implementation
		// ========================================================================
		
		AtlasInput::AtlasInput()
		{
			root_folder.atlas_input = this;
		}

		void AtlasInput::Clear()
		{
			bitmaps.clear();
			multi_bitmaps.clear();
			faces.clear(); // XXX : face / library destruction is important
			libraries.clear();
			AtlasBaseTemplate<BitmapInfoInput, FontInfoInput, FolderInfoInput, ReferencedObject>::Clear();
		}

		void AtlasInput::RegisterResource(FIBITMAP * bitmap, bool release)
		{
			if (bitmap == nullptr || !release)
				return;
			size_t count = bitmaps.size();
			for (size_t i = 0; i < count; ++i)
				if (bitmaps[i].get() == bitmap)
					return;
			bitmaps.push_back(std::move(bitmap_ptr(bitmap)));
		}

		void AtlasInput::RegisterResource(FIMULTIBITMAP * multi_bitmap, bool release)
		{
			if (multi_bitmap == nullptr || !release)
				return;
			size_t count = multi_bitmaps.size();
			for (size_t i = 0; i < count; ++i)
				if (multi_bitmaps[i].get() == multi_bitmap)
					return;
			multi_bitmaps.push_back(std::move(multibitmap_ptr(multi_bitmap)));
		}

		void AtlasInput::RegisterResource(FT_Library library, bool release)
		{
			if (library == nullptr || !release)
				return;
			size_t count = libraries.size();
			for (size_t i = 0; i < count; ++i)
				if (libraries[i].get() == library)
					return;
			libraries.push_back(std::move(library_ptr(library)));
		}

		void AtlasInput::RegisterResource(FT_Face face, bool release)
		{
			if (face == nullptr || !release)
				return;
			size_t count = faces.size();
			for (size_t i = 0; i < count; ++i)
				if (faces[i].get() == face)
					return;
			faces.push_back(std::move(face_ptr(face)));
		}

		bool AtlasInput::AddBitmapFilesFromDirectory(FilePathParam const & path, bool recursive)
		{
			return root_folder.AddBitmapFilesFromDirectory(path, recursive);
		}
		BitmapInfoInput * AtlasInput::AddBitmap(FilePathParam const & path, char const * name, TagType tag)
		{
			return root_folder.AddBitmap(path, name, tag);
		}
		BitmapInfoInput * AtlasInput::AddBitmap(FIBITMAP * bitmap, bool release_bitmap, char const * name, TagType tag)
		{
			return root_folder.AddBitmap(bitmap, release_bitmap, name, tag);
		}
		BitmapInfoInput * AtlasInput::AddBitmap(FIMULTIBITMAP * animated_bitmap, bool release_bitmap, char const * name, TagType tag)
		{
			return root_folder.AddBitmap(animated_bitmap, release_bitmap, name, tag);
		}

		FolderInfoInput * AtlasInput::AddFolder(char const * name, TagType tag)
		{
			return root_folder.AddFolder(name, tag);
		}

		FontInfoInput * AtlasInput::AddFont(
			char const * font_name,
			FT_Library library,
			bool release_library,
			char const * name,
			TagType tag,
			FontInfoInputParams const & params)
		{
			return root_folder.AddFont(font_name, library, release_library, name, tag, params);
		}

		FontInfoInput * AtlasInput::AddFont(
			FT_Face face,
			bool release_face,
			char const * name,
			TagType tag,
			FontInfoInputParams const & params)
		{
			return root_folder.AddFont(face, release_face, name, tag, params);
		}

		void AtlasInput::ApplyFilter(BitmapAtlasFilter * filter)
		{
			if (filter != nullptr)
				ApplyFilter(filter, &root_folder);
		}

		void AtlasInput::ApplyFilter(BitmapAtlasFilter* filter, FolderInfoInput * folder)
		{
			assert(filter != nullptr);

			// the bitmaps
			for (std::unique_ptr<BitmapInfoInput> & bitmap_input : folder->bitmaps)
				ApplyFilter(filter, bitmap_input.get());

			// the animations
			for (std::unique_ptr<FontInfoInput>& font_input : folder->fonts)
				ApplyFilter(filter, font_input.get());

			// recursive through the hierarchy
			for (std::unique_ptr<FolderInfoInput>& child_folder : folder->folders)
				ApplyFilter(filter, child_folder.get());			
		}

		void AtlasInput::ApplyFilter(BitmapAtlasFilter* filter, BitmapInfoInput* input)
		{
			assert(filter != nullptr);

			FIBITMAP* new_bitmap = filter->ProcessImage(input->description);
			if (new_bitmap != nullptr)
			{
				input->description = chaos::ImageTools::GetImageDescription(new_bitmap);
				RegisterResource(new_bitmap, true);
			}
		}

		void AtlasInput::ApplyFilter(BitmapAtlasFilter* filter, FontInfoInput* input)
		{
			assert(filter != nullptr);

			size_t count = input->elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				CharacterInfoInput* character = input->elements[i].get();
				if (character == nullptr)
					continue;
				ApplyFilter(filter, character);
			}
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

