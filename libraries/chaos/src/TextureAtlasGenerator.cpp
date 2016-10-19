#include <chaos/TextureAtlasGenerator.h>
#include <chaos/TextureAtlas.h>
#include <chaos/ImageTools.h>
#include <chaos/MathTools.h>

namespace chaos
{
  
  // ========================================================================
  // AtlasRectangle implementation
  // ========================================================================

  bool AtlasRectangle::IsFullyInside(AtlasRectangle const & big) const
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

  bool AtlasRectangle::IsIntersecting(AtlasRectangle const & big) const
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
  // TextureAtlasInputBase implementation
  // ========================================================================

  bool TextureAtlasInputBase::AddTextureFilesFromDirectory(boost::filesystem::path const & p)
  {
    // enumerate the source directory
    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(p) ; it != end ; ++it)
      AddTextureFile(it->path(), nullptr);                           // this will reject files that are not images .. not an error
    return true;
  }

  bool TextureAtlasInputBase::AddTextureFile(boost::filesystem::path const & path, char const * name)
  {
    if (boost::filesystem::is_regular_file(path))
      return AddTextureFile(path.string().c_str(), name);
    return false;
  }

  bool TextureAtlasInputBase::AddTextureFile(char const * filename, char const * name)
  {   
    assert(filename != nullptr);

    bool result = false;

    FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
    if (image != nullptr)
    {
      result = AddImageSource(
          (name != nullptr) ? name : boost::filesystem::path(filename).filename().string().c_str(), // XXX : cannot use an intermediate temporary because the filesystem.string() is a temp object
          image, true); 
      if (!result)
        FreeImage_Unload(image);    
    }
    return result;
  }

  bool TextureAtlasInputBase::AddFakeImageSource(char const * name)
  {
    assert(name != nullptr);

    int w = 15 * (1 + rand() % 10);
    int h = 15 * (1 + rand() % 10);

    bool result = false;

    FIBITMAP * image = FreeImage_Allocate(w, h, 32); // allocate an image
    if (image != nullptr)
    {
      unsigned char c = 55 + (rand() % 200);

      unsigned char color[] = { c, c, c, 255 }; // B G R A

      FreeImage_FillBackground(image, color, 0); // create a background color

      result = AddImageSource(name, image, true);
      if (!result)
        FreeImage_Unload(image);
    }
    return result;
  }

  // ========================================================================
  // TextureAtlasInput implementation
  // ========================================================================

  bool TextureAtlasInput::AddImageSource(char const * name, FIBITMAP * image, bool release_bitmap)
  {
    assert(name  != nullptr);
    assert(image != nullptr);

    TextureAtlasInputEntry new_entry;

    new_entry.bitmap         = image;
    new_entry.width          = (int)FreeImage_GetWidth(new_entry.bitmap);
    new_entry.height         = (int)FreeImage_GetHeight(new_entry.bitmap);
    new_entry.bpp            = (int)FreeImage_GetBPP(new_entry.bitmap);
    new_entry.name           = name;
    new_entry.release_bitmap = release_bitmap;

    entries.push_back(std::move(new_entry)); // move for std::string copy
    return true;
  }

  void TextureAtlasInput::Clear()
  {
    // destroy the entries
    for (TextureAtlasInputEntry & input_entry : entries)
      if (input_entry.bitmap != nullptr)
        if (input_entry.release_bitmap)
          FreeImage_Unload(input_entry.bitmap);
    entries.empty();
  }

  // ========================================================================
  // TextureAtlasGenerator implementation
  // ========================================================================

  AtlasRectangle TextureAtlasGenerator::GetAtlasRectangle() const
  {
    AtlasRectangle result;
    result.x      = 0;
    result.y      = 0;
    result.width  = params.atlas_width;
    result.height = params.atlas_height;
    return result;
  }

  AtlasRectangle TextureAtlasGenerator::GetRectangle(TextureAtlasEntry const & entry) const
  {
    AtlasRectangle result;
    result.x      = entry.x;
    result.y      = entry.y;
    result.width  = entry.width;
    result.height = entry.height;
    return result; 
  }
  
  AtlasRectangle TextureAtlasGenerator::AddPadding(AtlasRectangle const & r) const
  {
    AtlasRectangle result = r;
    result.x      -= params.atlas_padding;
    result.y      -= params.atlas_padding;
    result.width  += 2 * params.atlas_padding;
    result.height += 2 * params.atlas_padding;
    return result;
  }

  bool TextureAtlasGenerator::ComputeResult(TextureAtlasInput & in_input, TextureAtlas & in_output, TextureAtlasGeneratorParams const & in_params)
  {
    // clear generator from previous usage
    Clear();

    // store arguments inside
    params = in_params;
    input  = &in_input;
    output = &in_output;

    // prepare the result to receive new compuation
    output->Clear(); 

    // ensure BPP is valid
    if (params.atlas_bpp != 0 && params.atlas_bpp != 8 && params.atlas_bpp != 24 && params.atlas_bpp != 32)
      return false;

    // search max texture size
    int max_width  = -1;
    int max_height = -1;
    int max_bpp    = -1;

    for (TextureAtlasInputEntry & input_entry : input->entries)
    {
      if (max_width < 0 || max_width < input_entry.width)
        max_width = input_entry.width;
      if (max_height < 0 || max_height < input_entry.height)
        max_height = input_entry.height;
      if (max_bpp < 0 || max_bpp < input_entry.bpp)
        max_bpp = input_entry.bpp;
    }

    max_width  += params.atlas_padding * 2;
    max_height += params.atlas_padding * 2;

    // compute final atlas size
    if (params.atlas_width <= 0 || params.atlas_width < max_width)
      params.atlas_width = max_width;
    if (params.atlas_height <= 0 || params.atlas_height < max_height)
      params.atlas_height = max_height;

    if (params.force_power_of_2)
    {
      params.atlas_width  = MathTools::GetNearestPowerOf2(params.atlas_width);
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
    
    // generate an entry in output for each entry in input
    size_t count = input->entries.size();

    output->entries.reserve(count);
    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasInputEntry const & input_entry = input->entries[i];

      TextureAtlasEntry output_entry;
      output_entry.name   = input_entry.name;
      output_entry.atlas  = SIZE_MAX;
      output_entry.x      = 0;
      output_entry.y      = 0;
      output_entry.width  = input_entry.width;
      output_entry.height = input_entry.height;

      output->entries.push_back(output_entry);
    }
    // ensure this can be produced inside an atlas with size_restriction
    if (DoComputeResult())
    {
      if (EnsureValidResults())
      {
        output->bitmaps = GenerateAtlasTextures();
#if _DEBUG
        output->OutputAtlasSpaceOccupation(std::cout);
        output->OutputTextureInfo(std::cout);
#endif
        return true;
      }   
#if _DEBUG
      else
        output->OutputTextureInfo(std::cout);
#endif
    }    
    return false;
  }


  bool TextureAtlasGenerator::EnsureValidResults(std::ostream & stream) const
  {
    bool result = true;

    size_t atlas_count = GetAtlasCount();
    size_t count       = output->entries.size();

    AtlasRectangle atlas_rectangle = GetAtlasRectangle();

    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasEntry const & t = output->entries[i];

      if (t.atlas >= atlas_count)
      {
        stream << "Texture atlas value is invalid !!! (index = " << i << ")" << std::endl;
        result = false;
      }

      AtlasRectangle r = AddPadding(GetRectangle(t));
      if (!r.IsFullyInside(atlas_rectangle))
      {
        stream << "Texture should be fully inside any atlas !!! (index = " << i << ")" << std::endl;
        result = false;
      }
    }

    for (size_t i = 0 ; i < count - 1 ; ++i)
    {
      for (size_t j = i + 1 ; j < count ; ++j)
      {
        TextureAtlasEntry const & output_entry_1 = output->entries[i];
        TextureAtlasEntry const & output_entry_2 = output->entries[j];

        if (output_entry_1.atlas != output_entry_2.atlas)
          continue;

        AtlasRectangle r1 = AddPadding(GetRectangle(output_entry_1));
        AtlasRectangle r2 = AddPadding(GetRectangle(output_entry_2));

        if (r1.IsIntersecting(r2))
        {
          stream << "Collision between textures !!! (indices = " << i << " and " << j << ")" << std::endl;
          result = false;
        }
      }
    }

    return result;
  }

  bool TextureAtlasGenerator::HasInterctingTexture(size_t atlas_index, AtlasRectangle const & r) const
  {
    AtlasRectangle r1 = AddPadding(r);

    size_t count = output->entries.size();
    for (size_t i = 0 ; i < count  ; ++i)
    {
      TextureAtlasEntry const & output_entry = output->entries[i];
      if (output_entry.atlas != atlas_index)
        continue;

      AtlasRectangle r2 = AddPadding(GetRectangle(output_entry));
      if (r2.IsIntersecting(r1))
        return true;
    }
    return false;
  }

  std::vector<FIBITMAP *> TextureAtlasGenerator::GenerateAtlasTextures() const
  {
    unsigned char const color[] = {0, 0, 0, 0}; // B G R A

    std::vector<FIBITMAP *> result;

    // generate the images and save them
    size_t atlas_cout = GetAtlasCount();
    for (size_t i = 0 ; i < atlas_cout ; ++i)
    {
      FIBITMAP * image = FreeImage_Allocate(params.atlas_width, params.atlas_height, params.atlas_bpp);
      if (image != nullptr)
      {       
        FreeImage_FillBackground(image, color, 0);

        size_t count = output->entries.size(); // should be same than inputs
        for (size_t j = 0 ; j < count ; ++j)         
        {
          TextureAtlasInputEntry const & input_entry = input->entries[j];
          TextureAtlasEntry      const & output_entry = output->entries[j];

          if (output_entry.atlas != i || input_entry.bitmap == nullptr)
            continue;

          FreeImage_Paste(image, input_entry.bitmap, output_entry.x, output_entry.y, 255);
        }
      }
      result.push_back(image);
    }
    return result;
  }

  size_t TextureAtlasGenerator::GetAtlasCount() const
  {
    return atlas_definitions.size();
  }

  void TextureAtlasGenerator::Clear()
  {
    params = TextureAtlasGeneratorParams();
    input  = nullptr;
    output = nullptr;

    atlas_definitions.clear();
  }

  float TextureAtlasGenerator::GetAdjacentSurface(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, std::vector<int> const & collision, size_t x_count, size_t y_count, size_t u, size_t v, size_t dx, size_t dy) const
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

        result = ((float)std::abs(x1 - x2)) * ((float)(input_entry.height + 2 * params.atlas_padding));
      }
      else
      {
        int y1 = atlas_def.split_y[b];
        int y2 = atlas_def.split_y[v];

        result = ((float)std::abs(y1 - y2)) * ((float)(input_entry.width + 2 * params.atlas_padding));
      }
    }

    return result;
  }

  float TextureAtlasGenerator::FindBestPositionInAtlas(TextureAtlasInputEntry const & input_entry, AtlasDefinition const & atlas_def, int & x, int & y) const
  {
    float result = -1.0f;

    size_t  x_count = atlas_def.split_x.size();
    size_t  y_count = atlas_def.split_y.size();
    size_t count = x_count * y_count;

    std::vector<int> collision_table;
    collision_table.insert(collision_table.begin(), count, 1); // by default, we cannot place the texture at any position

    // find collision table
    AtlasRectangle r;
    r.width  = input_entry.width;
    r.height = input_entry.height;

    bool any_value = false;
    for (size_t u = 0 ; u < x_count ; ++u)
    {
      int px = atlas_def.split_x[u];
      if (px + input_entry.width + 2 * params.atlas_padding > params.atlas_width) // cannot puts the texture at this position (not fully inside the atlas)
        break;
      r.x = px + params.atlas_padding;

      for (size_t v = 0 ; v < y_count ; ++v)
      {
        int py = atlas_def.split_y[v];
        if (py + input_entry.height + 2 * params.atlas_padding > params.atlas_height)  // cannot puts the texture at this position (not fully inside the atlas)
          break;
        r.y = py + params.atlas_padding;

        bool collision = HasInterctingTexture(&atlas_def - &atlas_definitions[0], r);

        any_value |= !collision;

        if (!collision)
          collision_table[u * y_count + v] = 0;
      }
    }

    if (!any_value)
      return -1.0f; // texture can be set nowhere in that atlas

    // find the best position
    for (size_t u = 0 ; u < x_count ; ++u)
    {
      for (size_t v = 0 ; v < y_count ; ++v)
      {
        size_t index = u * y_count + v;
        if (!collision_table[index])
        {
          float surf1 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v, +1,  0);
          float surf2 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v, -1,  0);
          float surf3 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v,  0, +1);
          float surf4 = GetAdjacentSurface(input_entry, atlas_def, collision_table, x_count, y_count, u, v,  0, -1);

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

  void TextureAtlasGenerator::InsertOrdered(std::vector<int> & v, int value)
  {
    auto it = std::lower_bound(v.begin(), v.end(), value);
    if ((it != v.end()) && (*it == value))
      return;
    v.insert(it, value);
  }

  void TextureAtlasGenerator::InsertTextureInAtlas(TextureAtlasEntry & input_entry, AtlasDefinition & atlas_def, int x, int y)
  {
    input_entry.atlas = &atlas_def - &atlas_definitions[0];
    input_entry.x     = x + params.atlas_padding;
    input_entry.y     = y + params.atlas_padding;

    InsertOrdered(atlas_def.split_x, x);
    InsertOrdered(atlas_def.split_x, x + input_entry.width + 2 * params.atlas_padding);

    InsertOrdered(atlas_def.split_y, y);
    InsertOrdered(atlas_def.split_y, y + input_entry.height + 2 * params.atlas_padding);
  }

  bool TextureAtlasGenerator::DoComputeResult()
  {
    size_t count = input->entries.size();

    float p = (float)params.atlas_padding;

    // create an indirection list for texture sorted by surface
    std::vector<size_t> textures_indirection_table = CreateTextureIndirectionTable([this, p](size_t i1, size_t i2){

      TextureAtlasInputEntry const & input_entry_1 = input->entries[i1];
      TextureAtlasInputEntry const & input_entry_2 = input->entries[i2];

      if ((input_entry_1.height + p) * (input_entry_1.width + p) > (input_entry_2.height + p) * (input_entry_2.width + p))
        return true;
      return false;
    });

    for (size_t i = 0 ; i < count ; ++i)
    {
      int entry_index = textures_indirection_table[i];

      TextureAtlasInputEntry & input_entry = input->entries[entry_index];

      size_t best_atlas_index =  SIZE_MAX;
      int    best_x           =  0;
      int    best_y           =  0;
      float  best_score       = -1.0f;

      for (size_t j = 0 ; j < atlas_definitions.size() ; ++j)
      {
        int   x, y;
        float score = FindBestPositionInAtlas(input_entry, atlas_definitions[j], x, y);

        if (score < 0.0f)
          continue; // cannot insert the texture in this atlas

        if (score < best_score || best_score < 0) // new best position found
        {
          best_score       = score;
          best_x           = x;
          best_y           = y;
          best_atlas_index = j;
        }

        if (score == 0.0f) // no need to search any more in any atlases
          break;
      }

      if (best_atlas_index == SIZE_MAX) // not enough size in any existing atlas. create a new one
      {
        AtlasDefinition def;
        def.split_x.push_back(0);
        def.split_x.push_back(params.atlas_width + 2 * params.atlas_padding);
        def.split_y.push_back(0);
        def.split_y.push_back(params.atlas_height + 2 * params.atlas_padding);

        best_atlas_index = atlas_definitions.size();
        best_x           = 0;
        best_y           = 0;

        atlas_definitions.push_back(def);
      }

      InsertTextureInAtlas(output->entries[entry_index], atlas_definitions[best_atlas_index], best_x, best_y);
    }

    return true;
  }

  bool TextureAtlasGenerator::CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, TextureAtlasGeneratorParams const & in_params)
  {
    // fill the atlas
    TextureAtlasInput input;
    input.AddTextureFilesFromDirectory(src_dir);

    // create the atlas files
    TextureAtlas          atlas;
    TextureAtlasGenerator generator;  
    if (generator.ComputeResult(input, atlas, in_params))
      return atlas.SaveAtlas(filename);
    return false;
  }
};

