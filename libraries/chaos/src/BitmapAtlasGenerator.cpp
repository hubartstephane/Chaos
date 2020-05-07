#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ImageTools.h>
#include <chaos/FontTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>
#include <chaos/BoostTools.h>
#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlasInput.h>

namespace chaos
{
	namespace BitmapAtlas
	{

		// ========================================================================
		// Utility functions
		// ========================================================================

		bool LoadFromJSON(nlohmann::json const& json_entry, AtlasGeneratorParams& dst)
		{
			if (!json_entry.is_object())
				return false;

			JSONTools::GetAttribute(json_entry, "force_power_of_2", dst.force_power_of_2);
			JSONTools::GetAttribute(json_entry, "force_square", dst.force_square);
			JSONTools::GetAttribute(json_entry, "duplicate_image_border", dst.duplicate_image_border);
			JSONTools::GetAttribute(json_entry, "atlas_width", dst.atlas_width);
			JSONTools::GetAttribute(json_entry, "atlas_height", dst.atlas_height);
			JSONTools::GetAttribute(json_entry, "atlas_max_width", dst.atlas_max_width);
			JSONTools::GetAttribute(json_entry, "atlas_max_height", dst.atlas_max_height);
			JSONTools::GetAttribute(json_entry, "atlas_padding", dst.atlas_padding);
			JSONTools::GetAttribute(json_entry, "background_color", dst.background_color);
			JSONTools::GetAttribute(json_entry, "merge_params", dst.merge_params);

			return true;
		}

		bool SaveIntoJSON(nlohmann::json& json_entry, AtlasGeneratorParams const& src)
		{
			if (!json_entry.is_object())
				json_entry = nlohmann::json::object();

			JSONTools::SetAttribute(json_entry, "force_power_of_2", src.force_power_of_2);
			JSONTools::SetAttribute(json_entry, "force_square", src.force_square);
			JSONTools::SetAttribute(json_entry, "duplicate_image_border", src.duplicate_image_border);
			JSONTools::SetAttribute(json_entry, "atlas_width", src.atlas_width);
			JSONTools::SetAttribute(json_entry, "atlas_height", src.atlas_height);
			JSONTools::SetAttribute(json_entry, "atlas_max_width", src.atlas_max_width);
			JSONTools::SetAttribute(json_entry, "atlas_max_height", src.atlas_max_height);
			JSONTools::SetAttribute(json_entry, "atlas_padding", src.atlas_padding);
			JSONTools::SetAttribute(json_entry, "background_color", src.background_color);
			JSONTools::SetAttribute(json_entry, "merge_params", src.merge_params);
			return true;
		}

		// ========================================================================
		// Utility functions
		// ========================================================================

		static BitmapLayout * GetBitmapLayout(BitmapInfoInput * info)
		{
			if (info->bitmap_output_info != nullptr)
				return info->bitmap_output_info;
			if (info->character_output_info != nullptr)
				return info->character_output_info;
			return nullptr;
		}

		static BitmapLayout const * GetBitmapLayout(BitmapInfoInput const * info)
		{
			if (info->bitmap_output_info != nullptr)
				return info->bitmap_output_info;
			if (info->character_output_info != nullptr)
				return info->character_output_info;
			return nullptr;
		}

		static NamedObject * GetNamedObject(BitmapInfoInput * info)
		{
			if (info->bitmap_output_info != nullptr)
				return info->bitmap_output_info;
			if (info->character_output_info != nullptr)
				return info->character_output_info;
			return nullptr;
		}

		static NamedObject const * GetNamedObject(BitmapInfoInput const * info)
		{
			if (info->bitmap_output_info != nullptr)
				return info->bitmap_output_info;
			if (info->character_output_info != nullptr)
				return info->character_output_info;
			return nullptr;
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

		Rectangle AtlasGenerator::GetRectangle(BitmapLayout const & layout) const
		{
			Rectangle result;
			result.x = layout.x;
			result.y = layout.y;
			result.width = layout.width;
			result.height = layout.height;
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

		bool AtlasGenerator::EnsureValidResults(BitmapInfoInputVector const & entries, std::ostream & stream) const
		{
			bool result = true;

			Rectangle atlas_rectangle = GetAtlasRectangle();

			// individual tests
			size_t bitmap_count = atlas_definitions.size(); // output->bitmaps not generated yet
			for (BitmapInfoInput const * entry_input : entries)
			{
				BitmapLayout const * layout = GetBitmapLayout(entry_input);
				if (layout == nullptr)
					continue;

				NamedObject const  * named  = GetNamedObject(entry_input);
				if (named == nullptr)
					continue;

				// test whether all info's bitmap_index are initialized
				if (layout->bitmap_index < 0)
				{
					stream << "Info encoutered with uninitialized bitmap_index : [" << named->name << " , " << named->tag << "]" << std::endl;
					result = false;
				}
				// test whether all info's bitmap_index are valid
				else if (layout->bitmap_index >= (int)bitmap_count)
				{
					stream << "Info encoutered with invalid bitmap_index : [" << named->name << " , " << named->tag << "]" << std::endl;
					result = false;
				}
				// test whether all info fits inside the atlas
				Rectangle r = AddPadding(GetRectangle(*layout));
				if (!r.IsFullyInside(atlas_rectangle))
				{
					stream << "Info encoutered that does not fit inside the atlas : [" << named->name << " , " << named->tag << "]" << std::endl;
					result = false;
				}
			}

			// test whether a collision exists between 2 elements
			size_t count = entries.size();
			for (size_t i = 0; i < count - 1; ++i)
			{
				for (size_t j = i + 1; j < count; ++j)
				{
					BitmapLayout const * layout1 = GetBitmapLayout(entries[i]);
					BitmapLayout const * layout2 = GetBitmapLayout(entries[j]);

					if (layout1 == nullptr || layout2 == nullptr)
						continue;

					if (layout1->bitmap_index != layout2->bitmap_index) // ignore entries not in the same bitmap
						continue;

					Rectangle r1 = AddPadding(GetRectangle(*layout1));
					Rectangle r2 = AddPadding(GetRectangle(*layout2));

					if (r1.IsIntersecting(r2))
					{
						NamedObject const * named1 = GetNamedObject(entries[i]);
						NamedObject const * named2 = GetNamedObject(entries[j]);

						if (named1 != nullptr && named2 != nullptr)
						{
							stream << "Collision between entries : " << std::endl;
							stream << "  [" << named1->name << " , " << named1->tag << "]" << std::endl;
							stream << "  [" << named2->name << " , " << named2->tag << "]" << std::endl;
						}
						result = false;
					}
				}
			}
			return result;
		}

		bool AtlasGenerator::HasIntersectingInfo(Rectangle const& r, std::vector<Rectangle> const& collision_rectangles) const
		{
			for (Rectangle const& other_r : collision_rectangles)
				if (other_r.IsIntersecting(r))
					return true;
			return false;
		}

		std::vector<bitmap_ptr> AtlasGenerator::GenerateBitmaps(BitmapInfoInputVector const & entries, PixelFormat const & final_pixel_format) const
		{		
			std::vector<bitmap_ptr> result;

			// generate the bitmaps
			size_t bitmap_count = atlas_definitions.size();
			for (size_t i = 0; i < bitmap_count; ++i)
			{			
				bitmap_ptr bitmap = bitmap_ptr(ImageTools::GenFreeImage(final_pixel_format, params.atlas_width, params.atlas_height));
				if (bitmap != nullptr)
				{
					// set the background to black
					ImageDescription image_description = ImageTools::GetImageDescription(bitmap.get());

					ImageTools::FillImageBackground(image_description, params.background_color);

					// copy-paste all entries
					for (BitmapInfoInput const * entry_input : entries)
					{
						BitmapLayout const * layout = GetBitmapLayout(entry_input);
						if (layout == nullptr)
							continue;

						if (layout->bitmap_index != i)
							continue;
						if (entry_input->description.IsEmpty(false))
							continue;

						// beware, according to FreeImage, the coordinate origin is top-left
						// to match with OpenGL (bottom-left), we have to make a swap
						int tex_x = layout->x;
						int tex_y = params.atlas_height - layout->y - layout->height;

						// copy and convert pixels
						ImageDescription src_desc = entry_input->description;
						ImageDescription dst_desc = ImageTools::GetImageDescription(bitmap.get());


						int w = src_desc.width;
						int h = src_desc.height;
						

						ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, tex_x, tex_y, w, src_desc.height, ImageTransform::NO_TRANSFORM);

						
						// XXX:
						// Duplicate the first/last rows/column of each subimage so that the sampling errors would give us a duplicate value
						// this force to have a padding of a least 1 (each image have its own padding zone) 
						//
						// +------+
						// |+----+|
						// ||    || Double border
						// |+----+|
						// +------+

						if (params.duplicate_image_border)
						{
							// XXX : it is possible to index dst texture to outside the range reserved surface (the double border) because
							//       dst_desc is descriptor on the whole image
							//       (we force a padding of at least 1)						

							// 4 edges
							ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, tex_x, tex_y - 1, w, 1, ImageTransform::NO_TRANSFORM);
							ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, tex_x - 1, tex_y, 1, h, ImageTransform::NO_TRANSFORM);

							ImageTools::CopyPixels(src_desc, dst_desc, 0, h - 1, tex_x, tex_y + h, w, 1, ImageTransform::NO_TRANSFORM);
							ImageTools::CopyPixels(src_desc, dst_desc, w - 1, 0, tex_x + w, tex_y, 1, h, ImageTransform::NO_TRANSFORM);

							// 4 extra corners
							ImageTools::CopyPixels(src_desc, dst_desc, 0, 0, tex_x - 1, tex_y - 1, 1, 1, ImageTransform::NO_TRANSFORM);
							ImageTools::CopyPixels(src_desc, dst_desc, w - 1, 0, tex_x + w, tex_y - 1, 1, 1, ImageTransform::NO_TRANSFORM);

							ImageTools::CopyPixels(src_desc, dst_desc, 0, h - 1, tex_x - 1, tex_y + h, 1, 1, ImageTransform::NO_TRANSFORM);
							ImageTools::CopyPixels(src_desc, dst_desc, w - 1, h - 1, tex_x + w, tex_y + h, 1, 1, ImageTransform::NO_TRANSFORM);
						}
					}
					result.push_back(std::move(bitmap));
				}
			}
			return result;
		}


		void AtlasGenerator::FillAtlasEntriesFromInput(BitmapInfoInputVector & result, FolderInfoInput * folder_info_input, FolderInfo * folder_info_output)
		{
			if (folder_info_input == nullptr)
				return;

			// recursively iterate over all sub folders
			size_t folder_count = folder_info_input->folders.size();
			folder_info_output->folders.reserve(folder_count + folder_info_output->folders.size());
			for (size_t i = 0; i < folder_count; ++i)
			{
				FolderInfoInput * child_folder_info_input = folder_info_input->folders[i].get();
				if (child_folder_info_input == nullptr)
					continue;

				FolderInfo * child_folder_info_output = new FolderInfo;
				if (child_folder_info_output == nullptr)
					continue;

				child_folder_info_output->name = child_folder_info_input->name;
				child_folder_info_output->tag = child_folder_info_input->tag;

				folder_info_output->folders.push_back(std::move(std::unique_ptr<FolderInfo>(child_folder_info_output)));
				FillAtlasEntriesFromInput(result, child_folder_info_input, child_folder_info_output);
			}

			// register the bitmaps
			size_t bitmap_count = folder_info_input->bitmaps.size();
			folder_info_output->bitmaps.reserve(bitmap_count + folder_info_output->bitmaps.size());
			for (size_t i = 0 ; i < bitmap_count ; ++i)
			{
				BitmapInfoInput const * bitmap_info_input = folder_info_input->bitmaps[i].get();
				if (bitmap_info_input == nullptr)
					continue;

				BitmapInfo bitmap_info_output;
				bitmap_info_output.name = bitmap_info_input->name;
				bitmap_info_output.tag = bitmap_info_input->tag;
				bitmap_info_output.bitmap_index = -1;
				bitmap_info_output.x = 0;
				bitmap_info_output.y = 0;
				bitmap_info_output.width = bitmap_info_input->description.width;
				bitmap_info_output.height = bitmap_info_input->description.height;

				folder_info_output->bitmaps.push_back(std::move(bitmap_info_output));
			}

			// once we are sure that Folder.Bitmaps does not resize anymore, we can store pointers    
			for (size_t i = 0; i < bitmap_count; ++i)
			{
				BitmapInfoInput * bitmap_info_input = folder_info_input->bitmaps[i].get();
				BitmapInfo      * bitmap_info = &folder_info_output->bitmaps[i];

				bitmap_info_input->bitmap_output_info = bitmap_info;
				result.push_back(bitmap_info_input);
			}

			// now that all BitmapInfoInput are linked to their BitmapInfo, handle animation
			for (size_t i = 0; i < bitmap_count; ++i)
			{
				BitmapInfoInput * bitmap_info_input = folder_info_input->bitmaps[i].get();
				BitmapInfo      * bitmap_info = &folder_info_output->bitmaps[i];

				// create the animation if necessary
				BitmapAnimationInfoInput * animation_info_input = bitmap_info_input->animation_info.get();
				if (animation_info_input != nullptr)
				{
					BitmapAnimationInfo * animation_info = new BitmapAnimationInfo;
					if (animation_info != nullptr)
					{
						// copy the animation
						animation_info->grid_data = animation_info_input->animation_description.grid_data;
						animation_info->child_frame_count = (int)animation_info_input->child_frames.size();
						animation_info->frame_duration = animation_info_input->animation_description.frame_duration;
						animation_info->anim_duration = animation_info_input->animation_description.anim_duration;
						animation_info->default_wrap_mode = animation_info_input->animation_description.default_wrap_mode;
						// store the animation
						bitmap_info->animation_info = animation_info;
					}
				}
			}

			// register the fonts 
			size_t font_count = folder_info_input->fonts.size();
			folder_info_output->fonts.reserve(font_count + folder_info_output->fonts.size());
			for (size_t i = 0 ; i < font_count ; ++i)
			{
				FontInfoInput const * font_info_input = folder_info_input->fonts[i].get();
				if (font_info_input == nullptr)
					continue;

				FontInfo font_info_output;
				font_info_output.name = font_info_input->name;
				font_info_output.tag = font_info_input->tag;
				font_info_output.glyph_width = font_info_input->glyph_width;
				font_info_output.glyph_height = font_info_input->glyph_height;
				font_info_output.ascender = font_info_input->ascender;
				font_info_output.descender = font_info_input->descender;
				font_info_output.face_height = font_info_input->face_height;

				size_t character_count = font_info_input->elements.size(); // now the elements in the font
				font_info_output.elements.reserve(character_count + font_info_output.elements.size());
				for (size_t i = 0; i < character_count; ++i)
				{
					CharacterInfoInput const * character_info_input = font_info_input->elements[i].get();

					CharacterInfo character_info_output;
					character_info_output.name = character_info_input->name;
					character_info_output.tag = character_info_input->tag;
					character_info_output.bitmap_index = -1;
					character_info_output.x = 0;
					character_info_output.y = 0;
					character_info_output.width = character_info_input->description.width;
					character_info_output.height = character_info_input->description.height;
					character_info_output.advance = character_info_input->advance;
					character_info_output.bitmap_left = character_info_input->bitmap_left;
					character_info_output.bitmap_top = character_info_input->bitmap_top;

					font_info_output.elements.push_back(std::move(character_info_output));
				}
				folder_info_output->fonts.push_back(std::move(font_info_output));
			}
			// once we are sure that Folder.Fonts vector does not resize anymore, we can store pointers    			
			for (size_t i = 0; i < font_count; ++i)
			{
				FontInfoInput * font_info_input  = folder_info_input->fonts[i].get();
				FontInfo & font_info_output = folder_info_output->fonts[i];

				size_t character_count = font_info_input->elements.size();
				for (size_t j = 0; j < character_count; ++j)
				{
					font_info_input->elements[j]->character_output_info = &font_info_output.elements[j];
					result.push_back(font_info_input->elements[j].get());
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

			// due to special image treatment we may want to have an extra padding (see texel interpolation and  AtlasGenerator::GenerateBitmaps(...)
			if (params.duplicate_image_border)
				++params.atlas_padding;

			// prepare the result to receive new computation
			output->Clear();

			// generate input entries and sets. Collect input entries 
			// we have to const_cast<> as a non-const data because we are modifying "output_info" members
			// this as no real side effect a the exit of this function on AtlasInput
			BitmapInfoInputVector entries;
			FillAtlasEntriesFromInput(entries, const_cast<FolderInfoInput *>(&in_input.root_folder), &output->root_folder);

			// search max texture size
			int max_width = -1;
			int max_height = -1;

			PixelFormatMerger pixel_format_merger(params.merge_params);

			for (BitmapInfoInput const * info : entries)
			{
				if (info->description.width == 0 || info->description.height == 0) // ignore empty bitmaps
					continue;

				if (max_width < 0 || max_width < info->description.width)
					max_width = info->description.width;
				if (max_height < 0 || max_height < info->description.height)
					max_height = info->description.height;

				pixel_format_merger.Merge(info->description.pixel_format); // search the best pixel format
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
				params.atlas_width = params.atlas_height = std::max(params.atlas_width, params.atlas_height);

			// test the validity of size
			if (params.atlas_max_width > 0 && params.atlas_max_width < params.atlas_width)
				return false;
			if (params.atlas_max_height > 0 && params.atlas_max_height < params.atlas_height)
				return false;

			// ensure this can be produced inside an atlas with size_restriction
			if (DoComputeResult(entries))
			{
#if _DEBUG
				if (EnsureValidResults(entries))
#endif // _DEBUG
				{
					output->bitmaps = GenerateBitmaps(entries, final_pixel_format);
					output->atlas_count = (int)output->bitmaps.size();
					output->dimension = glm::ivec2(params.atlas_width, params.atlas_height);					
					return true;
				}
			}

			// in case of failure, clean the atlas
			Clear();
			return false;
		}

		bool AtlasGenerator::DoComputeResult(BitmapInfoInputVector const & entries)
		{
			size_t count = entries.size();

			// create an indirection list for entries sorted by surface
			float padding = (float)params.atlas_padding;
			std::vector<size_t> textures_indirection_table = CreateIndirectionTable(count, [padding, &entries](size_t i1, size_t i2) 
			{
				BitmapInfoInput const * entry_1 = entries[i1];
				BitmapInfoInput const * entry_2 = entries[i2];

				if ((entry_1->description.height + padding) * (entry_1->description.width + padding) > (entry_2->description.height + padding) * (entry_2->description.width + padding))
					return true;
				return false;
			});

			for (size_t i = 0; i < count; ++i)
			{
				size_t entry_index = textures_indirection_table[i];

				BitmapInfoInput const * input_entry = entries[entry_index];

				int   best_atlas_index = -1;
				float best_score = -1.0f;

				glm::ivec2 best_position = glm::ivec2(0, 0);

				for (size_t j = 0; j < atlas_definitions.size(); ++j)
				{
					glm::ivec2 position = glm::ivec2(0, 0);

					// score < 0	=> failure
					// score == 0	=> perfect match, no need to search in other page
					// score > 0	=> search greater score
					float score = FindBestPositionInAtlas(entries, *input_entry, atlas_definitions[j], position);

					if (score < 0.0f)
						continue; // cannot insert the texture in this atlas

					if (score > best_score || best_score < 0) // new best position found
					{
						best_score       = score;
						best_position    = position;
						best_atlas_index = (int)j;
					}

					if (score == 0.0f) // no need to search any more in any atlases
						break;
				}

				if (best_atlas_index == -1) // not enough size in any existing atlas. create a new one
				{
					AtlasDefinition def;

					def.potential_bottomleft_corners.push_back(glm::ivec2(0, 0));

					best_atlas_index = (int)atlas_definitions.size();
					best_position = glm::ivec2(0, 0);

					atlas_definitions.push_back(std::move(def));
				}

				BitmapLayout * layout = GetBitmapLayout(entries[entry_index]);
				if (layout != nullptr)					
					InsertBitmapLayoutInAtlas(*layout, atlas_definitions[best_atlas_index], best_position);
			}
			return true;
		}

		float AtlasGenerator::FindBestPositionInAtlas(BitmapInfoInputVector const & entries, BitmapInfoInput const & info, AtlasDefinition const & atlas_def, glm::ivec2 & position) const
		{
			// not enought surface remaining. Early exit
			unsigned int max_surface = (unsigned int)(params.atlas_width * params.atlas_height);
			unsigned int entry_surface = (unsigned int)
				((info.description.width + 2 * params.atlas_padding) *
				(info.description.height + 2 * params.atlas_padding));

			if (atlas_def.surface_sum + entry_surface > max_surface)
				return -1.0f;

			// the rectangle for given bitmap including the padding
			Rectangle r;
			r.width  = info.description.width + 2 * params.atlas_padding;
			r.height = info.description.height + 2 * params.atlas_padding;

			for (glm::ivec2 const& p : atlas_def.potential_bottomleft_corners)
			{
				// position of the rectangle (padding included)
				r.x = p.x;
				r.y = p.y;
				// check whether the rectangle fully fill into to atlas page
				if (r.x + r.width >= params.atlas_width)
					continue;
				if (r.y + r.height >= params.atlas_height)
					continue;
				// check for other rectangles in the same page
				if (!HasIntersectingInfo(r, atlas_def.collision_rectangles))
				{
					position = p;
					return 0.0f; // perfect fit
				}
			}
			return -1.0f; // not found on this page
		}

		void AtlasGenerator::InsertBitmapLayoutInAtlas(BitmapLayout & layout, AtlasDefinition & atlas_def, glm::ivec2 const & position)
		{
			layout.bitmap_index = (int)(&atlas_def - &atlas_definitions[0]);
			layout.x = position.x + params.atlas_padding;
			layout.y = position.y + params.atlas_padding;

			// XXX : for OpenGL, texture Y = 0 is bottom
			//       for bitmap,         Y = 0 is top
			//       => explains why (1. - Y)

			// XXX : in image space, Y is top from bottom (0 is up)
			layout.bottomleft_texcoord.x = MathTools::CastAndDiv<float>(layout.x, params.atlas_width);
			layout.bottomleft_texcoord.y = 1.0f - MathTools::CastAndDiv<float>(layout.y + layout.height, params.atlas_height);
			layout.topright_texcoord.x = MathTools::CastAndDiv<float>(layout.x + layout.width, params.atlas_width);
			layout.topright_texcoord.y = 1.0f - MathTools::CastAndDiv<float>(layout.y, params.atlas_height);

			// erase the point from potential entries
			auto it = std::find(atlas_def.potential_bottomleft_corners.begin(), atlas_def.potential_bottomleft_corners.end(), position);
			if (it != atlas_def.potential_bottomleft_corners.end())
				atlas_def.potential_bottomleft_corners.erase(it);

			// insert 3 new corners as entries (bottom-right / top-left / top-right)
			int width  = layout.width + 2 * params.atlas_padding;
			int height = layout.height + 2 * params.atlas_padding;

			atlas_def.potential_bottomleft_corners.emplace_back(position.x + width, position.y);
			atlas_def.potential_bottomleft_corners.emplace_back(position.x, position.y + height);
			atlas_def.potential_bottomleft_corners.emplace_back(position.x + width, position.y + height);

			// insert new rectangle to test for collision
			Rectangle r;
			r.x = position.x;
			r.y = position.y;
			r.width = width;
			r.height = height;
			atlas_def.collision_rectangles.push_back(r);

			// compute sum of all surfaces used in this atlas page
			atlas_def.surface_sum += (unsigned int)
				((layout.width + 2 * params.atlas_padding) *
				(layout.height + 2 * params.atlas_padding));
		}

		bool AtlasGenerator::CreateAtlasFromDirectory(FilePathParam const & bitmaps_dir, FilePathParam const & path, bool recursive, AtlasGeneratorParams const & in_params)
		{
			// fill the atlas
			AtlasInput input;
			FolderInfoInput * folder_info = input.AddFolder("files", 0);
			folder_info->AddBitmapFilesFromDirectory(bitmaps_dir, recursive);
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

		TextureArrayAtlas * TextureArrayAtlasGenerator::ComputeResult(AtlasInput const & in_input, AtlasGeneratorParams const & in_params, char const * dump_atlas_dirname)
		{
			// generate a standard atlas to be converted
			chaos::BitmapAtlas::Atlas          atlas;
			chaos::BitmapAtlas::AtlasGenerator generator;
			if (!generator.ComputeResult(in_input, atlas, in_params))
				return nullptr;

			// dump the atlas on disk
			if (dump_atlas_dirname != nullptr)
			{
				Application* application = Application::GetInstance();
				if (application != nullptr)
					atlas.SaveAtlas(application->GetUserLocalTempPath() / dump_atlas_dirname);
			}

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

