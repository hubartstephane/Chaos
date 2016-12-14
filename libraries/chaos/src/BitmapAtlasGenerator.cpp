#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ImageTools.h>
#include <chaos/MathTools.h>

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

    BitmapSetInput * AtlasInput::AddBitmapSet(char const * name)
    {
      assert(name != nullptr);

      BitmapSetInput * result = new BitmapSetInput;
      if (result != nullptr)
      {
        result->name = name;
        bitmap_sets.push_back(result);
      }
      return result;
    }

    CharacterSetInput * AtlasInput::AddCharacterSet(char const * name, FT_Library library, char const * font_name, char const * characters, bool release_library, CharacterSetInputParams const & params)
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

      return AddCharacterSetImpl(name, library, face, characters, release_library, true, params);
    }

    CharacterSetInput * AtlasInput::AddCharacterSet(char const * name, FT_Face face, char const * characters, bool release_face, CharacterSetInputParams const & params)
    {
      return AddCharacterSetImpl(name, nullptr, face, characters, false, release_face, params);
    }

    CharacterSetInput * AtlasInput::AddCharacterSetImpl(char const * name, FT_Library library, FT_Face face, char const * characters, bool release_library, bool release_face, CharacterSetInputParams const & params)
    {
      assert(name != nullptr);
      assert(face != nullptr);

      // if user does not provide a list of charset for the fonts, use this hard coded one
      static char const * DEFAULT_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

      CharacterSetInput * result = new CharacterSetInput;
      if (result != nullptr)
      {
        // new character set input
        result->name = name;
        result->library = library;
        result->face = face;
        result->release_library = release_library;
        result->release_face = release_face;
        result->glyph_width = params.glyph_width;
        result->glyph_height = params.glyph_height;
        result->ascender = face->ascender / 64;
        result->descender = face->descender / 64;
        result->line_spacing = face->height / 64;
        result->underline_position = face->underline_position / 64;

        // set font size
        FT_Error error = FT_Set_Pixel_Sizes(result->face, params.glyph_width, params.glyph_height);
        if (error != 0)
        {
          delete(result);
          return nullptr;
        }

        // generate glyph cache
        if (characters == nullptr || strlen(characters) == 0)
          characters = DEFAULT_CHARACTERS;

        std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache = FontTools::GetGlyphCacheForString(result->face, characters);

        // transforms each entry of the glyph map into a bitmap
        for (auto & glyph : glyph_cache)
        {
          int w = glyph.second.bitmap_glyph->bitmap.width;
          int h = glyph.second.bitmap_glyph->bitmap.rows;

          FIBITMAP * bitmap = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap, 32); 
          if (bitmap != nullptr || w <= 0 || h <= 0)  // if bitmap is zero sized (whitespace, the allocation failed). The entry is still interesting                                          
          {
            char name[] = " ";
            sprintf_s(name, 2, "%c", glyph.first);

            CharacterEntryInput entry;
            entry.name           = name;
            entry.tag            = glyph.first;
            entry.width          = (int)FreeImage_GetWidth(bitmap);
            entry.height         = (int)FreeImage_GetHeight(bitmap);
            entry.bpp            = (int)FreeImage_GetBPP(bitmap);
            entry.bitmap         = bitmap;
            entry.release_bitmap = true;
            entry.advance        = glyph.second.advance;
            entry.bitmap_left    = glyph.second.bitmap_left;
            entry.bitmap_top     = glyph.second.bitmap_top;
            result->elements.push_back(entry);
          }
        }

        // release the glyph cache 
        for (auto & glyph : glyph_cache)
          FT_Done_Glyph((FT_Glyph)glyph.second.bitmap_glyph);

        character_sets.push_back(result);
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

    bool BitmapSetInput::AddBitmapFilesFromDirectory(boost::filesystem::path const & p)
    {
      // enumerate the source directory
      boost::filesystem::directory_iterator end;
      for (boost::filesystem::directory_iterator it(p); it != end; ++it)
        AddBitmapFile(it->path(), nullptr);                           // this will reject files that are not images .. not an error
      return true;
    }

    bool BitmapSetInput::AddBitmapFile(boost::filesystem::path const & path, char const * name)
    {
      if (boost::filesystem::is_regular_file(path))
        return AddBitmapFile(path.string().c_str(), name);
      return false;
    }

    bool BitmapSetInput::AddBitmapFile(char const * filename, char const * name)
    {
      assert(filename != nullptr);

      bool result = false;

      FIBITMAP * bitmap = ImageTools::LoadImageFromFile(filename);
      if (bitmap != nullptr)
      {
        result = AddBitmap(
          (name != nullptr) ? name : boost::filesystem::path(filename).filename().string().c_str(), // XXX : cannot use an intermediate temporary because the filesystem.string() is a temp object
          bitmap, true);
        if (!result)
          FreeImage_Unload(bitmap);
      }
      return result;
    }

    bool BitmapSetInput::AddFakeBitmap(char const * name)
    {
      assert(name != nullptr);

      int w = 15 * (1 + rand() % 10);
      int h = 15 * (1 + rand() % 10);

      bool result = false;

      FIBITMAP * bitmap = FreeImage_Allocate(w, h, 32); // allocate an image
      if (bitmap != nullptr)
      {
        unsigned char color = 55 + (rand() % 200);
        unsigned char bgra[] = { color, color, color, 0 };

        FreeImage_FillBackground(bitmap, bgra, FI_COLOR_IS_RGBA_COLOR); // create a background color

        result = AddBitmap(name, bitmap, true);
        if (!result)
          FreeImage_Unload(bitmap);
      }
      return result;
    }

    bool BitmapSetInput::AddBitmap(char const * name, FIBITMAP * bitmap, bool release_bitmap)
    {
      assert(name != nullptr);
      assert(bitmap != nullptr);

      BitmapEntryInput new_entry;

      new_entry.name = name;
      new_entry.bitmap = bitmap;
      new_entry.width = (int)FreeImage_GetWidth(new_entry.bitmap);
      new_entry.height = (int)FreeImage_GetHeight(new_entry.bitmap);
      new_entry.bpp = (int)FreeImage_GetBPP(new_entry.bitmap);
      new_entry.release_bitmap = release_bitmap;

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
      int bitmap_count = atlas_definitions.size(); // output->bitmaps not generated yet
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
        else if (entry->bitmap_index >= bitmap_count)
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
      int count = entries.size();
      for (int i = 0; i < count - 1; ++i)
      {
        for (int j = i + 1; j < count; ++j)
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

    std::vector<unique_bitmap_ptr> AtlasGenerator::GenerateBitmaps(BitmapEntryInputVector const & entries) const
    {
      unsigned char const bgra[] = { 0, 0, 0, 0 }; // B G R A

      std::vector<unique_bitmap_ptr> result;

      // generate the bitmaps
      int bitmap_count = atlas_definitions.size();
      for (int i = 0; i < bitmap_count; ++i)
      {
        unique_bitmap_ptr bitmap = unique_bitmap_ptr(FreeImage_Allocate(params.atlas_width, params.atlas_height, params.atlas_bpp));
        if (bitmap)
        {
          FreeImage_FillBackground(bitmap.get(), bgra, FI_COLOR_IS_RGBA_COLOR);

          for (BitmapEntryInput const * entry_input : entries)
          {
            BitmapEntry const * entry = entry_input->output_entry;

            if (entry->bitmap_index != i)
              continue;

						// beware, according to FreeImage, the coordinate origin is top-left
						// to match with OpenGL (bottom-left), we have to make a swap
						int tex_x = entry->x;
						int tex_y = params.atlas_height - entry->y - entry->height;
						FreeImage_Paste(bitmap.get(), entry_input->bitmap, tex_x, tex_y, 255);
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
          entry.width = entry_input.width;
          entry.height = entry_input.height;
          bitmap_set->elements.push_back(entry);
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
        character_set->glyph_width = character_set_input->glyph_width;
        character_set->glyph_height = character_set_input->glyph_height;
        character_set->ascender = character_set_input->ascender;
        character_set->descender = character_set_input->descender;
        character_set->line_spacing = character_set_input->line_spacing;
        character_set->underline_position = character_set_input->underline_position;
        
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
          entry.width = entry_input.width;
          entry.height = entry_input.height;
          entry.advance = entry_input.advance;
          entry.bitmap_left = entry_input.bitmap_left;
          entry.bitmap_top = entry_input.bitmap_top;
          character_set->elements.push_back(entry);
        }
        // once we are sure that Atlas.CharacterSet.Entry vector does not resize anymore, we can store pointers         
        for (size_t i = 0; i < count; ++i)
        {
          character_set_input->elements[i].output_entry = &character_set->elements[i];
          result.push_back(&character_set_input->elements[i]);
        }
      }
    }

    bool AtlasGenerator::ComputeResult(AtlasInput & in_input, Atlas & in_output, AtlasGeneratorParams const & in_params)
    {
      // clear generator from previous usage
      Clear();

      // store arguments inside
      params = in_params;
      input = &in_input;
      output = &in_output;

      // prepare the result to receive new computation
      output->Clear();

      // ensure BPP is valid
      if (params.atlas_bpp != 0 && params.atlas_bpp != 8 && params.atlas_bpp != 24 && params.atlas_bpp != 32)
        return false;

      // generate input entries and sets. Collect input entries
      BitmapEntryInputVector entries;
      FillAtlasEntriesFromInput(entries);

      // search max texture size
      int max_width = -1;
      int max_height = -1;
      int max_bpp = -1;

      for (BitmapEntryInput const * entry : entries)
      {
        if (max_width < 0 || max_width < entry->width)
          max_width = entry->width;
        if (max_height < 0 || max_height < entry->height)
          max_height = entry->height;
        if (max_bpp < 0 || max_bpp < entry->bpp)
          max_bpp = entry->bpp;
      }

      max_width += params.atlas_padding * 2;
      max_height += params.atlas_padding * 2;

      // compute final atlas size
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

      // if necessary match BPP to textures in input
      if (params.atlas_bpp <= 0)
      {
        if (max_bpp != 8 && max_bpp != 24 && max_bpp != 32) // 16 BPP is unused
          max_bpp = 32;
        params.atlas_bpp = max_bpp;
      }

      // ensure this can be produced inside an atlas with size_restriction
      if (DoComputeResult(entries))
      {
        if (EnsureValidResults(entries))
        {
          output->bitmaps = GenerateBitmaps(entries);
          output->atlas_count = output->bitmaps.size();
          output->dimension = glm::ivec2(params.atlas_width, params.atlas_height);
#if _DEBUG
          output->OutputAtlasSpaceOccupation(std::cout);
          output->OutputInfo(std::cout);
#endif
          return true;
        }
#if _DEBUG
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

        if ((entry_1->height + padding) * (entry_1->width + padding) > (entry_2->height + padding) * (entry_2->width + padding))
          return true;
        return false;
      });

      for (size_t i = 0; i < count; ++i)
      {
        int entry_index = textures_indirection_table[i];

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
            best_atlas_index = j;
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

          best_atlas_index = atlas_definitions.size();
          best_x = 0;
          best_y = 0;

          atlas_definitions.push_back(def);
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

          result = ((float)std::abs(x1 - x2)) * ((float)(entry.height + 2 * params.atlas_padding));
        }
        else
        {
          int y1 = atlas_def.split_y[b];
          int y2 = atlas_def.split_y[v];

          result = ((float)std::abs(y1 - y2)) * ((float)(entry.width + 2 * params.atlas_padding));
        }
      }

      return result;
    }

    float AtlasGenerator::FindBestPositionInAtlas(BitmapEntryInputVector const & entries, BitmapEntryInput const & entry, AtlasDefinition const & atlas_def, int & x, int & y) const
    {
      float result = -1.0f;

      size_t  x_count = atlas_def.split_x.size();
      size_t  y_count = atlas_def.split_y.size();
      size_t count = x_count * y_count;

      std::vector<int> collision_table;
      collision_table.insert(collision_table.begin(), count, 1); // by default, we cannot place the texture at any position

                                                                 // find collision table
      Rectangle r;
      r.width = entry.width;
      r.height = entry.height;

      bool any_value = false;
      for (size_t u = 0; u < x_count; ++u)
      {
        int px = atlas_def.split_x[u];
        if (px + entry.width + 2 * params.atlas_padding > params.atlas_width) // cannot puts the texture at this position (not fully inside the atlas)
          break;
        r.x = px + params.atlas_padding;

        for (size_t v = 0; v < y_count; ++v)
        {
          int py = atlas_def.split_y[v];
          if (py + entry.height + 2 * params.atlas_padding > params.atlas_height)  // cannot puts the texture at this position (not fully inside the atlas)
            break;
          r.y = py + params.atlas_padding;

          bool collision = HasIntersectingEntry(entries, &atlas_def - &atlas_definitions[0], r);
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
      entry.bitmap_index = &atlas_def - &atlas_definitions[0];
      entry.x = x + params.atlas_padding;
      entry.y = y + params.atlas_padding;

      InsertOrdered(atlas_def.split_x, x);
      InsertOrdered(atlas_def.split_x, x + entry.width + 2 * params.atlas_padding);

      InsertOrdered(atlas_def.split_y, y);
      InsertOrdered(atlas_def.split_y, y + entry.height + 2 * params.atlas_padding);
    }

    bool AtlasGenerator::CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, AtlasGeneratorParams const & in_params)
    {
      // fill the atlas
      AtlasInput input;
      BitmapSetInput * bitmap_set = input.AddBitmapSet("files");
      bitmap_set->AddBitmapFilesFromDirectory(src_dir);
      // create the atlas files
      Atlas          atlas;
      AtlasGenerator generator;
      if (generator.ComputeResult(input, atlas, in_params))
        return atlas.SaveAtlas(filename);
      return false;
    }
  };
};

