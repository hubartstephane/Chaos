#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ImageTools.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/Application.h>

namespace chaos
{
	namespace BitmapAtlas
	{

		// ========================================================================
		// AtlasInput implementation
		// ========================================================================

		void AtlasInput::Clear()
		{
			// destroy the bitmaps
			for (BitmapSetInput * bitmap_set : bitmap_sets)
				delete(bitmap_set);
			bitmap_sets.clear();
			// destroy the fonts
			for (CharacterSetInput * character_set : character_sets)
				delete(character_set);
			character_sets.clear();
		}

		BitmapSetInput * AtlasInput::FindBitmapSetInput(char const * name)
		{
			size_t count = bitmap_sets.size();
			for (size_t i = 0; i < count; ++i)
			{
				BitmapSetInput * input = bitmap_sets[i];
				if (input == nullptr)
					continue;
				if (input->name == name)
					return input;
			}
			return nullptr;
		}

		BitmapSetInput const * AtlasInput::FindBitmapSetInput(char const * name) const
		{
			size_t count = bitmap_sets.size();
			for (size_t i = 0; i < count; ++i)
			{
				BitmapSetInput const * input = bitmap_sets[i];
				if (input == nullptr)
					continue;
				if (input->name == name)
					return input;
			}
			return nullptr;
		}

		BitmapSetInput * AtlasInput::AddBitmapSet(char const * name)
		{
			assert(name != nullptr);

			BitmapSetInput * result = FindBitmapSetInput(name);
			if (result == nullptr)
			{
				result = new BitmapSetInput;
				if (result != nullptr)
				{
					result->name = name;
					bitmap_sets.push_back(result);
				}
			}
			return result;
		}

		CharacterSetInput * AtlasInput::FindCharacterSetInput(char const * name)
		{
			size_t count = character_sets.size();
			for (size_t i = 0; i < count; ++i)
			{
				CharacterSetInput * input = character_sets[i];
				if (input == nullptr)
					continue;
				if (input->name == name)
					return input;
			}
			return nullptr;
		}

		CharacterSetInput const * AtlasInput::FindCharacterSetInput(char const * name) const
		{
			size_t count = character_sets.size();
			for (size_t i = 0; i < count; ++i)
			{
				CharacterSetInput const * input = character_sets[i];
				if (input == nullptr)
					continue;
				if (input->name == name)
					return input;
			}
			return nullptr;
		}

		CharacterSetInput * AtlasInput::AddCharacterSet(char const * name, char const * font_name, FT_Library library, bool release_library, CharacterSetInputParams const & params)
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
			FT_Face face;
			FT_Error error = FT_New_Face(library, font_name, 0, &face);
			if (error)
			{
				if (release_library)
					FT_Done_FreeType(library); // delete library if necessary
				return nullptr;
			}

			return AddCharacterSetImpl(name, library, face, release_library, true, params);
		}

		CharacterSetInput * AtlasInput::AddCharacterSet(char const * name, FT_Face face, bool release_face, CharacterSetInputParams const & params)
		{
			return AddCharacterSetImpl(name, nullptr, face, false, release_face, params);
		}

		CharacterSetInput * AtlasInput::AddCharacterSetImpl(char const * name, FT_Library library, FT_Face face, bool release_library, bool release_face, CharacterSetInputParams const & params)
		{
			assert(name != nullptr);
			assert(face != nullptr);

			CharacterSetInput * result = FindCharacterSetInput(name);
			if (result == nullptr)
			{
				result = new CharacterSetInput;
				if (result != nullptr)
				{
					// set font size
					// XXX : order is important. Face.size.metrics will not be initialized elsewhere
					FT_Error error = FT_Set_Pixel_Sizes(face, params.max_character_width, params.max_character_height);
					if (error != 0)
					{
						delete(result);
						return nullptr;
					}

					// new character set input
					result->name = name;
					result->library = library;
					result->face = face;
					result->release_library = release_library;
					result->release_face = release_face;
					result->max_character_width = params.max_character_width;
					result->max_character_height = params.max_character_height;
					result->ascender = face->size->metrics.ascender / 64;     // take the FT_Pixel_Size(...) into consideration
					result->descender = face->size->metrics.descender / 64;   // take the FT_Pixel_Size(...) into consideration 
					result->face_height = face->size->metrics.height / 64;    // take the FT_Pixel_Size(...) into consideration

					// generate glyph cache

					// if user does not provide a list of charset for the fonts, use this hard coded one
					static char const * DEFAULT_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

					char const * characters = (params.characters.length() > 0) ?
						params.characters.c_str() :
						DEFAULT_CHARACTERS;

					std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache = FontTools::GetGlyphCacheForString(result->face, characters);

					// transforms each entry of the glyph map into a bitmap
					for (auto & glyph : glyph_cache)
					{
						int w = glyph.second.bitmap_glyph->bitmap.width;
						int h = glyph.second.bitmap_glyph->bitmap.rows;

						FIBITMAP * bitmap = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap, PixelFormat::FORMAT_RGBA);
						if (bitmap != nullptr || w <= 0 || h <= 0)  // if bitmap is zero sized (whitespace, the allocation failed). The entry is still interesting                                          
						{
							char name[] = " ";
							sprintf_s(name, 2, "%c", glyph.first);

							CharacterEntryInput entry;
							entry.name = name;
							entry.tag = glyph.first;
							if (bitmap != nullptr)
								entry.description = ImageTools::GetImageDescription(bitmap);
							entry.bitmap = bitmap;
							entry.release_bitmap = true;
							entry.advance = glyph.second.advance;         // take the FT_Pixel_Size(...) into consideration
							entry.bitmap_left = glyph.second.bitmap_left; // take the FT_Pixel_Size(...) into consideration
							entry.bitmap_top = glyph.second.bitmap_top;   // take the FT_Pixel_Size(...) into consideration
							result->elements.push_back(std::move(entry));
						}
					}

					// release the glyph cache 
					for (auto & glyph : glyph_cache)
						FT_Done_Glyph((FT_Glyph)glyph.second.bitmap_glyph);

					character_sets.push_back(result);
				}
			}
			return result;
		}

		// ========================================================================
		// BitmapSetInput implementation
		// ========================================================================

		BitmapSetInput::~BitmapSetInput()
		{
			for (BitmapEntryInput & element : elements)
				if (element.release_bitmap)
					FreeImage_Unload(element.bitmap);
			elements.clear();
		}

		bool BitmapSetInput::AddBitmapFilesFromDirectory(FilePathParam const & path)
		{
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();
			// enumerate the source directory
			boost::filesystem::directory_iterator end;
			for (boost::filesystem::directory_iterator it(resolved_path); it != end; ++it)
				AddBitmapFile(it->path(), nullptr, 0);                           // this will reject files that are not images .. not an error
			return true;
		}

		bool BitmapSetInput::AddBitmapFile(FilePathParam const & path, char const * name, int tag)
		{
			bool result = false;

			FIBITMAP * bitmap = ImageTools::LoadImageFromFile(path);
			if (bitmap != nullptr)
			{
				boost::filesystem::path const & resolved_path = path.GetResolvedPath();

				result = AddBitmap(
					(name != nullptr) ? name : BoostTools::PathToName(resolved_path).c_str(), // XXX : cannot use an intermediate temporary because the filesystem.string() is a temp object
					bitmap, true, tag);
				if (!result)
					FreeImage_Unload(bitmap);
			}
			return result;
		}

		BitmapEntryInput * BitmapSetInput::FindEntry(int tag)
		{
			for (BitmapEntryInput & element : elements)
				if (element.tag == tag)
					return &element;
			return nullptr;
		}

		BitmapEntryInput const * BitmapSetInput::FindEntry(int tag) const 
		{
			for (BitmapEntryInput const & element : elements)
				if (element.tag == tag)
					return &element;
			return nullptr;
		}

		BitmapEntryInput * BitmapSetInput::FindEntry(char const * name)
		{
			for (BitmapEntryInput & element : elements)
				if (element.name == name)
					return &element;
			return nullptr;
		}

		BitmapEntryInput const * BitmapSetInput::FindEntry(char const * name) const 
		{
			for (BitmapEntryInput const & element : elements)
				if (element.name == name)
					return &element;
			return nullptr;
		}

		bool BitmapSetInput::AddBitmap(char const * name, FIBITMAP * bitmap, bool release_bitmap, int tag)
		{
			assert(name != nullptr);
			assert(bitmap != nullptr);

			if (FindEntry(name) != nullptr)
				return false;

			BitmapEntryInput new_entry;

			new_entry.name = name;
			new_entry.bitmap = bitmap;
			new_entry.description = ImageTools::GetImageDescription(bitmap);
			new_entry.release_bitmap = release_bitmap;
			new_entry.tag            = tag;

			elements.push_back(std::move(new_entry)); // move for std::string copy
			return true;
		}

		// ========================================================================
		// CharacterSetInput implementation
		// ========================================================================

		CharacterSetInput::~CharacterSetInput()
		{
			// release face
			if (face != nullptr)
				if (release_face)
					FT_Done_Face(face);
			// release library
			if (library != nullptr)
				if (release_library)
					FT_Done_FreeType(library);
			// release the bitmaps
			for (CharacterEntryInput & element : elements)
				if (element.release_bitmap)
					FreeImage_Unload(element.bitmap);
		}

		// ========================================================================
		// Rectangle implementation
		// ========================================================================

		bool Rectangle::IsFullyInside(Rectangle const & big) const
		{
			if (x < big.x)
				return false;
			if (x + width > big.x + big.width)
				return false;
			if (y < big.y)
				return false;
			if (y + height > big.y + big.height)
				return false;
			return true;
		}

		bool Rectangle::IsIntersecting(Rectangle const & big) const
		{
			if (x >= big.x + big.width)
				return false;
			if (x + width <= big.x)
				return false;
			if (y >= big.y + big.height)
				return false;
			if (y + height <= big.y)
				return false;
			return true;
		}

		// ========================================================================
		// AtlasGenerator implementation
		// ========================================================================

		void AtlasGenerator::Clear()
		{
			params = AtlasGeneratorParams();
			input = nullptr;
			output = nullptr;
			atlas_definitions.clear();
		}

		Rectangle AtlasGenerator::GetAtlasRectangle() const
		{
			Rectangle result;
			result.x = 0;
			result.y = 0;
			result.width = params.atlas_width;
			result.height = params.atlas_height;
			return result;
		}

		Rectangle AtlasGenerator::GetRectangle(BitmapEntry const & entry) const
		{
			Rectangle result;
			result.x = entry.x;
			result.y = entry.y;
			result.width = entry.width;
			result.height = entry.height;
			return result;
		}

		Rectangle AtlasGenerator::AddPadding(Rectangle const & r) const
		{
			Rectangle result = r;
			result.x -= params.atlas_padding;
			result.y -= params.atlas_padding;
			result.width += 2 * params.atlas_padding;
			result.height += 2 * params.atlas_padding;
			return result;
		}

		bool AtlasGenerator::EnsureValidResults(BitmapEntryInputVector const & entries, std::ostream & stream) const
		{
			bool result = true;

			Rectangle atlas_rectangle = GetAtlasRectangle();

			// individual tests
			size_t bitmap_count = atlas_definitions.size(); // output->bitmaps not generated yet
			for (BitmapEntryInput const * entry_input : entries)
			{
				BitmapEntry const * entry = entry_input->output_entry;

				// test whether all entry's bitmap_index are initialized
				if (entry->bitmap_index < 0)
				{
					stream << "Entry encoutered with uninitialized bitmap_index : [" << entry->name << " , " << entry->tag << "]" << std::endl;
					result = false;
				}
				// test whether all entry's bitmap_index are valid
				else if (entry->bitmap_index >= (int)bitmap_count)
				{
					stream << "Entry encoutered with invalid bitmap_index : [" << entry->name << " , " << entry->tag << "]" << std::endl;
					result = false;
				}
				// test whether all entry fits inside the atlas
				Rectangle r = AddPadding(GetRectangle(*entry));
				if (!r.IsFullyInside(atlas_rectangle))
				{
					stream << "Entry encoutered that does not fit inside the atlas : [" << entry->name << " , " << entry->tag << "]" << std::endl;
					result = false;
				}
			}

			// test whether a collision exists between 2 elements
			size_t count = entries.size();
			for (size_t i = 0; i < count - 1; ++i)
			{
				for (size_t j = i + 1; j < count; ++j)
				{
					BitmapEntry const * entry1 = entries[i]->output_entry;
					BitmapEntry const * entry2 = entries[j]->output_entry;

					if (entry1->bitmap_index != entry2->bitmap_index) // ignore entries not in the same bitmap
						continue;

					Rectangle r1 = AddPadding(GetRectangle(*entry1));
					Rectangle r2 = AddPadding(GetRectangle(*entry2));

					if (r1.IsIntersecting(r2))
					{
						stream << "Collision between entries : " << std::endl;
						stream << "  [" << entry1->name << " , " << entry1->tag << "]" << std::endl;
						stream << "  [" << entry2->name << " , " << entry2->tag << "]" << std::endl;
						result = false;
					}
				}
			}
			return result;
		}

		bool AtlasGenerator::HasIntersectingEntry(BitmapEntryInputVector const & entries, int bitmap_index, Rectangle const & r) const
		{
			Rectangle r1 = AddPadding(r);

			for (BitmapEntryInput const * entry : entries)
			{
				if (entry->output_entry->bitmap_index != bitmap_index)
					continue;
				Rectangle r2 = AddPadding(GetRectangle(*entry->output_entry));
				if (r2.IsIntersecting(r1))
					return true;
			}
			return false;
		}

		std::vector<unique_bitmap_ptr> AtlasGenerator::GenerateBitmaps(BitmapEntryInputVector const & entries, PixelFormat const & final_pixel_format) const
		{		
			std::vector<unique_bitmap_ptr> result;

			// generate the bitmaps
			size_t bitmap_count = atlas_definitions.size();
			for (size_t i = 0; i < bitmap_count; ++i)
			{			
				unique_bitmap_ptr bitmap = unique_bitmap_ptr(ImageTools::GenFreeImage(final_pixel_format, params.atlas_width, params.atlas_height));
				if (bitmap != nullptr)
				{
					// set the background to black
					ImageDescription image_description = ImageTools::GetImageDescription(bitmap.get());

					ImageTools::FillImageBackground(image_description, params.background_color);

					// copy-paste all entries
					for (BitmapEntryInput const * entry_input : entries)
					{
						BitmapEntry const * entry = entry_input->output_entry;

						if (entry->bitmap_index != i)
							continue;
						if (entry_input->bitmap == nullptr)
							continue;

						// beware, according to FreeImage, the coordinate origin is top-left
						// to match with OpenGL (bottom-left), we have to make a swap
						int tex_x = entry->x;
						int tex_y = params.atlas_height - entry->y - entry->height;

						// copy and convert pixels
						ImageDescription src_desc = ImageTools::GetImageDescription(entry_input->bitmap);
						ImageDescription dst_desc = ImageTools::GetImageDescription(bitmap.get());

						ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, tex_x, tex_y, src_desc.width, src_desc.height, false);
					}
					result.push_back(std::move(bitmap));
				}
			}
			return result;
		}


		void AtlasGenerator::FillAtlasEntriesFromInput(BitmapEntryInputVector & result)
		{
			// fill with bitmap sets 
			for (BitmapSetInput * bitmap_set_input : input->bitmap_sets)
			{
				BitmapSet * bitmap_set = new BitmapSet;
				bitmap_set->name = bitmap_set_input->name;
				bitmap_set->tag = bitmap_set_input->tag;
				output->bitmap_sets.push_back(std::move(std::unique_ptr<BitmapSet>(bitmap_set))); // 'move' in mandatory because, unique_ptr has no copy operator

				size_t count = bitmap_set_input->elements.size();
				for (size_t i = 0; i < count; ++i)
				{
					BitmapEntryInput const & entry_input = bitmap_set_input->elements[i];

					BitmapEntry entry;
					entry.name = entry_input.name;
					entry.tag = entry_input.tag;
					entry.bitmap_index = -1;
					entry.x = 0;
					entry.y = 0;
					entry.width = entry_input.description.width;
					entry.height = entry_input.description.height;
					bitmap_set->elements.push_back(std::move(entry));
				}
				// once we are sure that Atlas.BitmapSet.Entry vector does not resize anymore, we can store pointers         
				for (size_t i = 0; i < count; ++i)
				{
					bitmap_set_input->elements[i].output_entry = &bitmap_set->elements[i];
					result.push_back(&bitmap_set_input->elements[i]);
				}
			}

			// fill with  character sets
			for (CharacterSetInput * character_set_input : input->character_sets)
			{
				CharacterSet * character_set = new CharacterSet;
				character_set->name = character_set_input->name;
				character_set->tag = character_set_input->tag;
				character_set->max_character_width = character_set_input->max_character_width;
				character_set->max_character_height = character_set_input->max_character_height;
				character_set->ascender = character_set_input->ascender;
				character_set->descender = character_set_input->descender;
				character_set->face_height = character_set_input->face_height;

				output->character_sets.push_back(std::move(std::unique_ptr<CharacterSet>(character_set)));

				size_t count = character_set_input->elements.size();
				for (size_t i = 0; i < count; ++i)
				{
					CharacterEntryInput const & entry_input = character_set_input->elements[i];

					CharacterEntry entry;
					entry.name = entry_input.name;
					entry.tag = entry_input.tag;
					entry.bitmap_index = -1;
					entry.x = 0;
					entry.y = 0;
					entry.width = entry_input.description.width;
					entry.height = entry_input.description.height;
					entry.advance = entry_input.advance;
					entry.bitmap_left = entry_input.bitmap_left;
					entry.bitmap_top = entry_input.bitmap_top;
					character_set->elements.push_back(std::move(entry));
				}
				// once we are sure that Atlas.CharacterSet.Entry vector does not resize anymore, we can store pointers         
				for (size_t i = 0; i < count; ++i)
				{
					character_set_input->elements[i].output_entry = &character_set->elements[i];
					result.push_back(&character_set_input->elements[i]);
				}
			}
		}

		bool AtlasGenerator::ComputeResult(AtlasInput const & in_input, Atlas & in_output, AtlasGeneratorParams const & in_params)
		{
			// clear generator from previous usage
			Clear();

			// store arguments inside
			params = in_params;
			input = &in_input;
			output = &in_output;

			// prepare the result to receive new computation
			output->Clear();

			// generate input entries and sets. Collect input entries
			BitmapEntryInputVector entries;
			FillAtlasEntriesFromInput(entries);

			// search max texture size
			int max_width = -1;
			int max_height = -1;

			PixelFormatMerger pixel_format_merger(params.merge_params);

			for (BitmapEntryInput const * entry : entries)
			{
				if (entry->description.width == 0 || entry->description.height == 0) // ignore empty bitmaps
					continue;

				if (max_width < 0 || max_width < entry->description.width)
					max_width = entry->description.width;
				if (max_height < 0 || max_height < entry->description.height)
					max_height = entry->description.height;

				pixel_format_merger.Merge(entry->description.pixel_format); // search the best pixel format
			}

			// test whether a correct pixel format has been found
			PixelFormat final_pixel_format = pixel_format_merger.GetResult();
			if (!final_pixel_format.IsValid())
				return false;

			// compute final atlas size
			max_width += params.atlas_padding * 2;
			max_height += params.atlas_padding * 2;

			if (params.atlas_width <= 0 || params.atlas_width < max_width)
				params.atlas_width = max_width;
			if (params.atlas_height <= 0 || params.atlas_height < max_height)
				params.atlas_height = max_height;

			if (params.force_power_of_2)
			{
				params.atlas_width = MathTools::GetNearestPowerOf2(params.atlas_width);
				params.atlas_height = MathTools::GetNearestPowerOf2(params.atlas_height);
			}

			if (params.force_square)
				params.atlas_width = params.atlas_height = max(params.atlas_width, params.atlas_height);

			// test the validity of size
			if (params.atlas_max_width > 0 && params.atlas_max_width < params.atlas_width)
				return false;
			if (params.atlas_max_height > 0 && params.atlas_max_height < params.atlas_height)
				return false;

			// ensure this can be produced inside an atlas with size_restriction
			if (DoComputeResult(entries))
			{
				if (EnsureValidResults(entries))
				{
					output->bitmaps = GenerateBitmaps(entries, final_pixel_format);
					output->atlas_count = (int)output->bitmaps.size();
					output->dimension = glm::ivec2(params.atlas_width, params.atlas_height);
#if _DEBUG
#if 0
					output->OutputAtlasSpaceOccupation(std::cout);
					output->OutputInfo(std::cout);
#endif
					if (params.debug_dump_atlas_dirname.size() > 0) // dump the atlas
					{
						Application * application = Application::GetInstance();
						if (application != nullptr)
							in_output.SaveAtlas(application->GetUserLocalTempPath() / params.debug_dump_atlas_dirname.c_str());
					}
#endif
					return true;
				}
#if 0 && _DEBUG
				else
					output->OutputInfo(std::cout);
#endif
			}

			// in case of failure, clean the atlas
			Clear();
			return false;
		}

		bool AtlasGenerator::DoComputeResult(BitmapEntryInputVector const & entries)
		{
			size_t count = entries.size();

			// create an indirection list for entries sorted by surface
			float padding = (float)params.atlas_padding;
			std::vector<size_t> textures_indirection_table = CreateIndirectionTable(count, [padding, &entries](size_t i1, size_t i2) {

				BitmapEntryInput const * entry_1 = entries[i1];
				BitmapEntryInput const * entry_2 = entries[i2];

				if ((entry_1->description.height + padding) * (entry_1->description.width + padding) > (entry_2->description.height + padding) * (entry_2->description.width + padding))
					return true;
				return false;
			});

			for (size_t i = 0; i < count; ++i)
			{
				size_t entry_index = textures_indirection_table[i];

				BitmapEntryInput const * input_entry = entries[entry_index];

				int   best_atlas_index = -1;
				int   best_x = 0;
				int   best_y = 0;
				float best_score = -1.0f;

				for (size_t j = 0; j < atlas_definitions.size(); ++j)
				{
					int   x, y;
					float score = FindBestPositionInAtlas(entries, *input_entry, atlas_definitions[j], x, y);

					if (score < 0.0f)
						continue; // cannot insert the texture in this atlas

					if (score < best_score || best_score < 0) // new best position found
					{
						best_score = score;
						best_x = x;
						best_y = y;
						best_atlas_index = (int)j;
					}

					if (score == 0.0f) // no need to search any more in any atlases
						break;
				}

				if (best_atlas_index == -1) // not enough size in any existing atlas. create a new one
				{
					AtlasDefinition def;
					def.split_x.push_back(0);
					def.split_x.push_back(params.atlas_width + 2 * params.atlas_padding);
					def.split_y.push_back(0);
					def.split_y.push_back(params.atlas_height + 2 * params.atlas_padding);

					best_atlas_index = (int)atlas_definitions.size();
					best_x = 0;
					best_y = 0;

					atlas_definitions.push_back(std::move(def));
				}
				InsertBitmapInAtlas(*entries[entry_index]->output_entry, atlas_definitions[best_atlas_index], best_x, best_y);
			}
			return true;
		}

		float AtlasGenerator::GetAdjacentSurface(BitmapEntryInput const & entry, AtlasDefinition const & atlas_def, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const
		{
			float result = 0.0f;

			size_t a = u;
			size_t b = v;

			// search the first intersection
			u += dx;
			v += dy;
			while ((u < x_count) && (u != SIZE_MAX) && (v < y_count) && (v != SIZE_MAX))
			{
				size_t index = u * y_count + v;
				if (collision[index])
					break;
				u += dx;
				v += dy;
			}

			u -= dx;
			v -= dy;

			if (u != a || v != b)
			{
				if (dx != 0)
				{
					int x1 = atlas_def.split_x[a];
					int x2 = atlas_def.split_x[u];

					result = ((float)std::abs(x1 - x2)) * ((float)(entry.description.height + 2 * params.atlas_padding));
				}
				else
				{
					int y1 = atlas_def.split_y[b];
					int y2 = atlas_def.split_y[v];

					result = ((float)std::abs(y1 - y2)) * ((float)(entry.description.width + 2 * params.atlas_padding));
				}
			}

			return result;
		}

		float AtlasGenerator::FindBestPositionInAtlas(BitmapEntryInputVector const & entries, BitmapEntryInput const & entry, AtlasDefinition const & atlas_def, int & x, int & y) const
		{
			float result = -1.0f;

			// not enought surface remaining. Early exit
			unsigned int max_surface = (unsigned int)(params.atlas_width * params.atlas_height);
			unsigned int entry_surface = (unsigned int)
				((entry.description.width + 2 * params.atlas_padding) *
				(entry.description.height + 2 * params.atlas_padding));

			if (atlas_def.surface_sum + entry_surface > max_surface)
				return -1.0f;

			size_t  x_count = atlas_def.split_x.size();
			size_t  y_count = atlas_def.split_y.size();
			size_t count = x_count * y_count;

			std::vector<int> collision_table;
			collision_table.insert(collision_table.begin(), count, 1); // by default, we cannot place the texture at any position

																	   // find collision table
			Rectangle r;
			r.width = entry.description.width;
			r.height = entry.description.height;

			bool any_value = false;
			for (size_t u = 0; u < x_count; ++u)
			{
				int px = atlas_def.split_x[u];
				if (px + entry.description.width + 2 * params.atlas_padding > params.atlas_width) // cannot puts the texture at this position (not fully inside the atlas)
					break;
				r.x = px + params.atlas_padding;

				for (size_t v = 0; v < y_count; ++v)
				{
					int py = atlas_def.split_y[v];
					if (py + entry.description.height + 2 * params.atlas_padding > params.atlas_height)  // cannot puts the texture at this position (not fully inside the atlas)
						break;
					r.y = py + params.atlas_padding;

					bool collision = HasIntersectingEntry(entries, (int)(&atlas_def - &atlas_definitions[0]), r);
					any_value |= !collision;

					if (!collision)
						collision_table[u * y_count + v] = 0;
				}
			}

			if (!any_value)
				return -1.0f; // texture can be set nowhere in that atlas

							  // find the best position
			for (size_t u = 0; u < x_count; ++u)
			{
				for (size_t v = 0; v < y_count; ++v)
				{
					size_t index = u * y_count + v;
					if (!collision_table[index])
					{
						float surf1 = GetAdjacentSurface(entry, atlas_def, collision_table, x_count, y_count, u, v, +1, 0);
						float surf2 = GetAdjacentSurface(entry, atlas_def, collision_table, x_count, y_count, u, v, -1, 0);
						float surf3 = GetAdjacentSurface(entry, atlas_def, collision_table, x_count, y_count, u, v, 0, +1);
						float surf4 = GetAdjacentSurface(entry, atlas_def, collision_table, x_count, y_count, u, v, 0, -1);

						float sum_surf = surf1 + surf2 + surf3 + surf4;

						if (sum_surf == 0.0f) // perfect fit
						{
							x = atlas_def.split_x[u];
							y = atlas_def.split_y[v];
							return 0.0f;
						}

						if (result < 0.0f || sum_surf < result)
						{
							x = atlas_def.split_x[u];
							y = atlas_def.split_y[v];
							result = sum_surf;
						}
					}
				}
			}

			return result;
		}

		void AtlasGenerator::InsertOrdered(std::vector<int> & v, int value)
		{
			auto it = std::lower_bound(v.begin(), v.end(), value);
			if ((it != v.end()) && (*it == value))
				return;
			v.insert(it, value);
		}

		void AtlasGenerator::InsertBitmapInAtlas(BitmapEntry & entry, AtlasDefinition & atlas_def, int x, int y)
		{
			entry.bitmap_index = (int)(&atlas_def - &atlas_definitions[0]);
			entry.x = x + params.atlas_padding;
			entry.y = y + params.atlas_padding;

			InsertOrdered(atlas_def.split_x, x);
			InsertOrdered(atlas_def.split_x, x + entry.width + 2 * params.atlas_padding);

			InsertOrdered(atlas_def.split_y, y);
			InsertOrdered(atlas_def.split_y, y + entry.height + 2 * params.atlas_padding);

			atlas_def.surface_sum += (unsigned int)
				((entry.width + 2 * params.atlas_padding) *
				(entry.height + 2 * params.atlas_padding));
		}

		bool AtlasGenerator::CreateAtlasFromDirectory(FilePathParam const & bitmaps_dir, FilePathParam const & path, AtlasGeneratorParams const & in_params)
		{
			// fill the atlas
			AtlasInput input;
			BitmapSetInput * bitmap_set = input.AddBitmapSet("files");
			bitmap_set->AddBitmapFilesFromDirectory(bitmaps_dir);
			// create the atlas files
			Atlas          atlas;
			AtlasGenerator generator;
			if (generator.ComputeResult(input, atlas, in_params))
				return atlas.SaveAtlas(path);
			return false;
		}

		// ========================================================================
		// TextureArrayAtlasGenerator implementation
		// ========================================================================

		TextureArrayAtlas * TextureArrayAtlasGenerator::ComputeResult(AtlasInput const & in_input, AtlasGeneratorParams const & in_params)
		{
			// generate a standard atlas to be converted
			chaos::BitmapAtlas::Atlas          atlas;
			chaos::BitmapAtlas::AtlasGenerator generator;
			if (!generator.ComputeResult(in_input, atlas, in_params))
				return nullptr;

			// generate texture Atlas
			chaos::BitmapAtlas::TextureArrayAtlas * result = new BitmapAtlas::TextureArrayAtlas;
			if (result == nullptr)
				return nullptr;
			if (!result->LoadFromBitmapAtlas(std::move(atlas)))
			{
				delete(result);
				return nullptr;
			}
			return result;
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

