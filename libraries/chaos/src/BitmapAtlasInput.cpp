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
				FT_New_Memory_Face(library, (FT_Byte const *)buffer.data, buffer.bufsize, 0, &face);

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

				FIBITMAP * bitmap = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap, PixelFormat::FORMAT_RGBA);
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

		bool FolderInfoInput::AddBitmapFilesFromDirectory(FilePathParam const & path, bool recursive)
		{
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();
			// enumerate the source directory
			boost::filesystem::directory_iterator end;
			for (boost::filesystem::directory_iterator it = FileTools::GetDirectoryIterator(resolved_path); it != end; ++it)
			{
				if (recursive && boost::filesystem::is_directory(*it))
				{
					FolderInfoInput * child_folder = AddFolder(BoostTools::PathToName(it->path()).c_str(), 0);
					if (child_folder == nullptr)
						continue;
					child_folder->AddBitmapFilesFromDirectory(it->path(), recursive);
				}
				else
				{
					// this will reject files that are not images .. not an error
					AddBitmap(it->path(), nullptr, 0);
				}
			}
			return true;
		}

		BitmapInfoInput * FolderInfoInput::AddBitmap(FilePathParam const & path, char const * name, TagType tag)
		{
			BitmapInfoInput * result = nullptr;

			// early exit
			if (FileTools::IsTypedFile(path, "JSON"))
				return nullptr;

			// compute a name from the path if necessary
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();

			// search if there is a JSON file to describe an animation
			boost::filesystem::path json_path = resolved_path;
			json_path.replace_extension("json");

			nlohmann::json json_file;
			if (JSONTools::LoadJSONFile(json_path, json_file, false))
			{


				result = result;
			}





			
			// test whether there is a grid describing the animation
			ImageAnimationDescription animation_description;

			std::string animated_name;

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



			// shuanimation


			// load all pages for the bitmap
			std::vector<FIBITMAP *> pages = ImageTools::LoadMultipleImagesFromFile(path, &animation_description); // may update frame rate

			size_t count = pages.size();
			for (size_t i = 0; i < count; ++i)
				RegisterResource(pages[i], true);

			// create the bitmap
			result = AddBitmapImpl(
				pages,
				name,
				tag,
				&animation_description
			);
			return result;
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


			// shuanimation


			std::vector<FIBITMAP *> pages = ImageTools::GetMultiImagePages(animated_bitmap, &animation_description);

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









			// shuanimation

#if 1


			// insert child animation frames
			if ((animation_description != nullptr && !animation_description->grid_data.IsEmpty()) || page_count > 1)
			{
				BitmapAnimationInfoInput * animation_info = new BitmapAnimationInfoInput;
				if (animation_info != nullptr)
				{
					result->animation_info = animation_info;
					if (animation_description != nullptr)
						result->animation_info->animation_description = *animation_description;

					// use grid animation system only if we are not on a GIF

					// shuanimation


					//if (page_count == 1 && animation_description != nullptr && !animation_description->grid_data.IsEmpty())
					//	animation_info->animation_description.grid_data = *grid_animation_info;

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
#endif

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

	}; // namespace BitmapAtlas

}; // namespace chaos

