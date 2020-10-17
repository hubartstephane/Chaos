#include <chaos/ParticleTextGenerator.h>
#include <chaos/MathTools.h>
#include <chaos/JSONTools.h>
#include <chaos/StringTools.h>
#include <chaos/ParticleTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/Hotpoint.h>
#include <chaos/LogTools.h>

namespace chaos
{
	namespace ParticleTextGenerator
	{	
		GeneratorParams::GeneratorParams(char const * in_font_info_name, float in_line_height, glm::vec2 const & in_position, Hotpoint in_hotpoint):
			line_height(in_line_height),
			font_info_name(in_font_info_name),
			position(in_position),
			hotpoint(in_hotpoint)
		{
		}

		bool SaveIntoJSON(nlohmann::json & json_entry, GeneratorParams const & src)
		{
			if (!json_entry.is_object())
				json_entry = nlohmann::json::object();

			JSONTools::SetAttribute(json_entry, "line_height", src.line_height);
			JSONTools::SetAttribute(json_entry, "line_spacing", src.line_spacing);
			JSONTools::SetAttribute(json_entry, "character_spacing", src.character_spacing);
			JSONTools::SetAttribute(json_entry, "bitmap_padding", src.bitmap_padding);
			JSONTools::SetAttribute(json_entry, "max_text_width", src.max_text_width);
			JSONTools::SetAttribute(json_entry, "word_wrap", src.word_wrap);
			JSONTools::SetAttribute(json_entry, "justify_space_factor", src.justify_space_factor);
			JSONTools::SetAttribute(json_entry, "alignment", src.alignment);
			JSONTools::SetAttribute(json_entry, "default_color", src.default_color);
			JSONTools::SetAttribute(json_entry, "font_info_name", src.font_info_name);
			JSONTools::SetAttribute(json_entry, "tab_size", src.tab_size);
			JSONTools::SetAttribute(json_entry, "position", src.position);
			JSONTools::SetAttribute(json_entry, "hotpoint", src.hotpoint);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const & json_entry, GeneratorParams & dst)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "line_height", dst.line_height);
			JSONTools::GetAttribute(json_entry, "line_spacing", dst.line_spacing);
			JSONTools::GetAttribute(json_entry, "character_spacing", dst.character_spacing);
			JSONTools::GetAttribute(json_entry, "bitmap_padding", dst.bitmap_padding);
			JSONTools::GetAttribute(json_entry, "max_text_width", dst.max_text_width);
			JSONTools::GetAttribute(json_entry, "word_wrap", dst.word_wrap);
			JSONTools::GetAttribute(json_entry, "justify_space_factor", dst.justify_space_factor);
			JSONTools::GetAttribute(json_entry, "alignment", dst.alignment);
			JSONTools::GetAttribute(json_entry, "default_color", dst.default_color);
			JSONTools::GetAttribute(json_entry, "font_info_name", dst.font_info_name);
			JSONTools::GetAttribute(json_entry, "tab_size", dst.tab_size);
			JSONTools::GetAttribute(json_entry, "position", dst.position);
			JSONTools::GetAttribute(json_entry, "hotpoint", dst.hotpoint);
			return true;
		}

		// ============================================================
		// GeneratorResult methods
		// ============================================================

		GeneratorResult::GeneratorResult()
		{
			Clear();
		}

		void GeneratorResult::Clear()
		{
			token_lines.clear();

			bounding_box.bottomleft = glm::vec2(0.0f, 0.0f);
			bounding_box.topright = glm::vec2(0.0f, 0.0f);
		}

		size_t GeneratorResult::GetTokenCount() const
		{
			size_t result = 0;
			for (size_t i = 0 ; i < token_lines.size() ; ++i)
				result += token_lines[i].size();	
			return result;
		}

		// ============================================================
		// GeneratorData methods
		// ============================================================

		Style & GeneratorData::PushDuplicate(bool override_top_stack)
		{
			if (!override_top_stack)
			{
				Style style = style_stack.back();
				style_stack.push_back(style); // push a duplicate of previous style
			}
			return style_stack[style_stack.size() - 1];
		}

		Style & GeneratorData::PushFontInfo(BitmapAtlas::FontInfo const * font_info, bool override_top_stack)
		{
			// push a copy of previous style, except the character set
			Style & result = PushDuplicate(override_top_stack);
			if (font_info != nullptr)
				result.font_info = font_info;
			return result;
		}

		Style & GeneratorData::PushColor(glm::vec4 const & color, bool override_top_stack)
		{
			// push a copy of previous style, except the color
			Style & result = PushDuplicate(override_top_stack);
			result.color = color;
			return result;
		}

		BitmapAtlas::FontInfo const * GeneratorData::GetFirstFont(BitmapAtlas::FolderInfo const * folder_info) const
		{
			if (folder_info == nullptr)
				return nullptr;
			if (folder_info->fonts.size() > 0)
				return &folder_info->fonts[0];

			BitmapAtlas::FontInfo const * result = nullptr;

			size_t count = folder_info->folders.size();
			for (size_t i = 0; (i < count) && (result == nullptr); ++i)
				result = GetFirstFont(folder_info->folders[i].get());
			return result;
		}

		BitmapAtlas::FontInfo const * GeneratorData::GetFontInfoFromName(char const * font_info_name) const
		{
			BitmapAtlas::FontInfo const * result = atlas.GetFontInfo(font_info_name, true);
			if (result == nullptr)
			{
				// for convenience, if we cannot find the character set, try to use the one on the top of the stack
				if (style_stack.size() > 0)
					result = style_stack.back().font_info;
				// if we still have no character set, take the very first available
				if (result == nullptr)
					result = GetFirstFont(&atlas.root_folder);
			}
			return result;
		}

		void GeneratorData::EmitCharacters(char c, int count)
		{
			// get current character set
			BitmapAtlas::FontInfo const * font_info = style_stack.back().font_info;
			if (font_info == nullptr)
				return;

			// get info corresponding to the glyph
			BitmapAtlas::CharacterInfo const * info = font_info->GetCharacterInfo(c);
			if (info == nullptr)
				return;

			// emit the characters
			for (int i = 0; i < count; ++i)
				EmitCharacter(c, info, font_info);
		}

		void GeneratorData::EmitCharacter(char c, BitmapAtlas::CharacterLayout const * layout, BitmapAtlas::FontInfo const * font_info)
		{
			Token token;
			token.character = c;
			token.character_layout = layout;
			token.font_info = font_info;
			token.color = style_stack.back().color;
			InsertTokenInLine(token);
		}

		void GeneratorData::EmitBitmap(BitmapAtlas::BitmapLayout const * layout, bool use_font_color)
		{
			Token token;
			token.bitmap_layout = layout;
			if (use_font_color)
				token.color = style_stack.back().color;
			else
				token.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			InsertTokenInLine(token);
		}

		void GeneratorData::InsertTokenInLine(Token & token)
		{
			// if there was no line, insert the very first one ...
			if (result.token_lines.size() == 0)
				result.token_lines.push_back(TokenLine());

			// insert the token
			if (token.bitmap_layout != nullptr)
			{
				// restrict the bitmap to the size of the line
				float factor = MathTools::CastAndDiv<float>(params.line_height - 2 * params.bitmap_padding.y, token.bitmap_layout->height);

				glm::vec2 bottomleft_position = bitmap_position + params.bitmap_padding;

				glm::vec2 token_size;
				token_size.x = factor * (float)token.bitmap_layout->width;
				token_size.y = params.line_height - 2 * params.bitmap_padding.y;

				// compute the particle data
				token.corners.bottomleft = bottomleft_position; 
				token.corners.topright   = bottomleft_position + token_size; 
				token.texcoords          = token.bitmap_layout->GetTexcoords();

				// next bitmap/character data

#if 0 // OLD and BROKEN */
				bitmap_position.x    += token_size.x + params.character_spacing + params.bitmap_padding.x * 2.0f;
				character_position.x += token_size.x + params.character_spacing + params.bitmap_padding.x * 2.0f;
#else
				bitmap_position.x    = bottomleft_position.x + params.character_spacing + params.bitmap_padding.x + token_size.x;
				character_position.x = bottomleft_position.x + params.character_spacing + params.bitmap_padding.x + token_size.x;
#endif

			}
			else if (token.character_layout != nullptr)
			{
				// get the descender 
				Style const & style = style_stack.back();
				float descender = (style.font_info == nullptr) ? 0.0f : style.font_info->descender;

				// scale the character back to the size of the scanline
				float factor = MathTools::CastAndDiv<float>(params.line_height, token.font_info->ascender - token.font_info->descender);

#if 0
				glm::vec2 bottomleft_position;
				bottomleft_position = character_position - glm::vec2(0.0f, descender) + // character_position.y is BELOW the scanline (at the descender level)
					factor * glm::vec2(
					(float)(token.character_layout->bitmap_left),
						(float)(token.character_layout->bitmap_top - token.character_layout->height) // XXX : -token.character_layout->height => to have BOTTOM LEFT CORNER
					);
#endif

				glm::vec2 bottomleft_position;
				bottomleft_position = character_position - glm::vec2(0.0f, descender * factor) + // character_position.y is BELOW the scanline (at the descender level)
					factor * glm::vec2(
						(float)(token.character_layout->bitmap_left),
						(float)(token.character_layout->bitmap_top - token.character_layout->height) // XXX : -token.character_layout->height => to have BOTTOM LEFT CORNER
					);

				glm::vec2 token_size;
				token_size.x = factor * (float)token.character_layout->width;   // may be 0 for escape characters
				token_size.y = factor * (float)token.character_layout->height;

				// compute the particle data
				token.corners.bottomleft = bottomleft_position; 
				token.corners.topright   = bottomleft_position + token_size; 
				token.texcoords = token.character_layout->GetTexcoords();

				// XXX : Some fonts are in italic. The 'advance' cause some 'override' in character bounding box.
				//       That's great for characters that are near one another
				//       But with bitmap that causes real overide.
				//       => that's why we are using two position : 'bitmap_position' & 'character_position'

#if 0 // OLD and BROKEN

				bitmap_position.x    = bottomleft_position.x + params.character_spacing + token_size.x;
				character_position.x = bottomleft_position.x + params.character_spacing + factor * (float)(token.character_layout->advance.x);
#else
				float token_width = (token_size.x > 0.0) ? // SPACE has size = 0
					token_size.x :
					(float)token.character_layout->advance.x;

				bitmap_position.x    = bottomleft_position.x + params.character_spacing + token_width;
				character_position.x = bottomleft_position.x + params.character_spacing + factor * (float)(token.character_layout->advance.x);
#endif
			}
			result.token_lines.back().push_back(token); // insert the token in the last line

		}

		void GeneratorData::EndCurrentLine()
		{
			// update position
			float delta_y = params.line_height + params.line_spacing;

			bitmap_position.x = 0.0f;
			bitmap_position.y -= delta_y;
			character_position.x = 0.0f;
			character_position.y -= delta_y;
			// if there was no line, insert the very first one ...
			if (result.token_lines.size() == 0)
				result.token_lines.push_back(TokenLine());
			// ... then you can add a new line
			result.token_lines.push_back(TokenLine());	
		}

		bool GeneratorData::StartMarkup(char const * text, int & i, class Generator & generator)
		{
			int j = i;
			while (text[i] != 0)
			{
				char c = text[i];

				if (!StringTools::IsVariableCharacter(c)) // searched string is contained in  [j .. i-1]
				{
					// no character : skip
					if (i - j < 1)
						return false; // ill-formed string								  
					// the markup
					std::string markup = std::string(&text[j], &text[i]);
					// markup correspond to a bitmap, the current character MUST be ']'
					auto bitmap = generator.GetBitmapInfo(markup.c_str());
					if (bitmap != nullptr)
					{
						if (c == ']') // normal bitmap 
						{
							EmitBitmap(bitmap, false);
							return true;
						}
						if (c == '*' &&  text[i + 1] == ']')  // bitmap that uses font color
						{
							++i;
							EmitBitmap(bitmap, true);
							return true;
						}
						return false; // ill-formed string
					}
					// if ']' is found, do nothing because, we are about to push a color/character set on the stack that is to be immediatly popped
					bool override_top_stack = false;
					if (c == ']')
						override_top_stack = true;
					// color markup found
					auto color = generator.GetColor(markup.c_str());
					if (color != nullptr)
					{
						PushColor(*color, override_top_stack);
						return true;
					}
					// character set markup found
					auto font_info = generator.GetFontInfo(markup.c_str());
					if (font_info != nullptr)
					{
						PushFontInfo(font_info, override_top_stack);
						return true;
					}
					// a markup has been detected but we don'k know to what it corresponds, so push a duplicate on the stack 
					// because we expect a markup closure later
					PushDuplicate(override_top_stack);
					return true;
				}
				++i;
			}
			return false; // markup started, but not finished : ill-formed
		}

		// ============================================================
		// Generator methods
		// ============================================================

		glm::vec4 const * Generator::GetColor(char const * name) const
		{
			auto it = colors.find(name);
			if (it == colors.end())
				return nullptr;
			return &it->second;
		}

		BitmapAtlas::BitmapInfo const * Generator::GetBitmapInfo(char const * name) const
		{
			auto it = bitmaps.find(name);
			if (it == bitmaps.end())
				return nullptr;
			return it->second;
		}
		BitmapAtlas::FontInfo const * Generator::GetFontInfo(char const * name) const
		{
			auto it = font_infos.find(name);
			if (it == font_infos.end())
				return nullptr;
			return it->second;
		}

		bool Generator::IsNameValid(char const * name) const
		{
			// ignore empty name
			if (name == nullptr)
				return false;
			// ensure name is a valid variable name
			if (!StringTools::IsVariableName(name, false)) // false => not strict => accept names starting with a figure
				return false;
			// ensure name is not already used by a color
			if (colors.find(name) != colors.end())
				return false;
			// ensure name is not already used by a bitmap
			if (bitmaps.find(name) != bitmaps.end())
				return false;
			// ensure name is not already used by a character set
			if (font_infos.find(name) != font_infos.end())
				return false;
			return true;
		}

		bool Generator::AddColor(char const * name, glm::vec4 const & color)
		{
			if (!IsNameValid(name))
				return false;
			colors.insert(std::make_pair(name, color));
			return true;
		}

		bool Generator::AddFontInfo(char const * name, char const * font_name)
		{
			assert(name != nullptr);
			assert(font_name != nullptr);

			BitmapAtlas::FontInfo const * font_info = atlas.GetFontInfo(font_name);
			if (font_info == nullptr)
				return false;
			return AddFontInfo(name, font_info);
		}

		bool Generator::AddFontInfo(char const * name, BitmapAtlas::FontInfo const * font_info)
		{
			assert(font_info != nullptr);

			if (!IsNameValid(name))
				return false;
			font_infos.insert(std::make_pair(name, font_info));
			return true;
		}

		bool Generator::AddBitmap(char const * name, char const * folder_name, char const * bitmap_name)
		{
			assert(name != nullptr);
			assert(folder_name != nullptr);
			assert(bitmap_name != nullptr);

			BitmapAtlas::FolderInfo const * folder_info = atlas.GetFolderInfo(folder_name);
			if (folder_info == nullptr)
				return false;
			BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(bitmap_name);
			if (info == nullptr)
				return false;
			return AddBitmap(name, info);
		}

		bool Generator::AddBitmap(char const * name, BitmapAtlas::BitmapInfo const * info)
		{
			assert(info != nullptr);

			if (!IsNameValid(name))
				return false;
			bitmaps.insert(std::make_pair(name, info));
			return true;
		}

		bool Generator::Generate(char const * text, GeneratorResult & result, GeneratorParams const & params)
		{
			assert(text != nullptr);

			// clear the result
			result.Clear();

			// initialize parse params stack with a default style that defines current color and fonts
			GeneratorData generator_data(result, params, atlas);

			Style style;
			style.color = params.default_color;
			style.font_info = generator_data.GetFontInfoFromName(params.font_info_name.c_str());
			generator_data.style_stack.push_back(style);

			// start the generation
			return DoGenerate(text, generator_data);
		}

		bool Generator::DoGenerate(char const * text, GeneratorData & generator_data)
		{
			// all steps to properly generate the result
			if (!DoGenerateLines(text, generator_data))
				return false;
			// justification
			if (!JustifyLines(generator_data.params, generator_data))
				return false;
			// finally, recenter the whole sprites
			if (!MoveParticlesToHotpoint(generator_data))
				return false;

			return true;
		}

		bool Generator::DoGenerateLines(char const * text, GeneratorData & generator_data)
		{
			// iterate over all characters
			bool escape_character = false;
			for (int i = 0; text[i] != 0; ++i)
			{
				char c = text[i];

				bool new_escape_character = (c == '\\');

				// ignore chariot return (UNIX/WINDOWS differences) : no different handling if previous character was an escape character
				if (c == '\r')
				{

				}
				// next line  : no different handling if previous character was an escape character
				else if (c == '\n')
				{
					generator_data.EndCurrentLine();
				}
				// tabulation : no different handling if previous character was an escape character
				else if (c == '\t')
				{
					int tab_size = std::min(generator_data.params.tab_size, 1);
					generator_data.EmitCharacters(' ', tab_size);
				}
				// if escape is set, simply display the incoming character no matter what it is (except \n \r \t)
				else if (escape_character)
				{
					generator_data.EmitCharacters(c, 1);
				}
				// start an escape
				else if (new_escape_character)
				{

				}
				// close previously started markup 
				else if (c == ']')
				{
					if (generator_data.style_stack.size() <= 1) // the very first style is manually inserted. It should never be popped
						return false;
					generator_data.style_stack.pop_back();
				}
				// start a new markup
				else if (c == '[')
				{
					if (!generator_data.StartMarkup(text, ++i, *this)) // ill-formed markup
						return false;
				}
				// finally, this is not a special character  		
				else
				{
					generator_data.EmitCharacters(c, 1);
				}

				escape_character = !escape_character && new_escape_character;
			}

			// all markups should be correctly closed (except the very first we have manually inserted)
			if (generator_data.style_stack.size() != 1) 
				return false;

			return true;
		}


		bool Generator::GetBoundingBox(TokenLine const & line, glm::vec2 & min_line_position, glm::vec2 & max_line_position) const
		{
			min_line_position.x = min_line_position.y = std::numeric_limits<float>::max();
			max_line_position.x = max_line_position.y = -std::numeric_limits<float>::max();

			if (line.size() == 0)
				return false;

			for (Token const & token : line)
			{
				min_line_position = glm::min(min_line_position, token.corners.bottomleft);
				max_line_position = glm::max(max_line_position, token.corners.topright);
			}
			return true;
		}

		bool Generator::GetBoundingBox(std::vector<TokenLine> const & token_lines, glm::vec2 & min_position, glm::vec2 & max_position) const
		{
			bool result = false;
			if (token_lines.size() > 0)
			{
				min_position.x = min_position.y = std::numeric_limits<float>::max();
				max_position.x = max_position.y = -std::numeric_limits<float>::max();

				for (TokenLine const & line : token_lines)
				{
					glm::vec2 min_line_position;
					glm::vec2 max_line_position;
					if (GetBoundingBox(line, min_line_position, max_line_position))
					{
						min_position = glm::min(min_position, min_line_position);
						max_position = glm::max(max_position, max_line_position);
						result = true;
					}
				}
			}
			return result;
		}

		void Generator::MoveParticles(TokenLine & line, glm::vec2 const & offset)
		{
			if (offset.x != 0.0f || offset.y != 0.0f)
			{
				for (Token & token : line)
				{
					token.corners.bottomleft += offset;	
					token.corners.topright += offset;	
				}	
			}
		}

		void Generator::MoveParticles(GeneratorResult & result, glm::vec2 const & offset)
		{
			if (offset.x != 0.0f || offset.y != 0.0f)
			{
				for (TokenLine & line : result.token_lines)
				{
					for (Token & token : line)
					{
						token.corners.bottomleft += offset;	
						token.corners.topright += offset;	
					}						
				}		
			}				
		}

		bool Generator::MoveParticlesToHotpoint(GeneratorData & generator_data)
		{
			// compute the min/max bounding box
			glm::vec2 min_position;
			glm::vec2 max_position;
			if (!GetBoundingBox(generator_data.result.token_lines, min_position, max_position)) // no sprite, nothing to do
				return true;

			// displace all the sprites to match the position
			glm::vec2 offset =
				generator_data.params.position -
				ConvertHotpoint(min_position, max_position - min_position, Hotpoint::BOTTOM_LEFT, generator_data.params.hotpoint);

			MoveParticles(generator_data.result, offset);

			// keep trace of the bounding box
			generator_data.result.bounding_box.bottomleft = min_position + offset;
			generator_data.result.bounding_box.topright   = max_position + offset;

			return true;
		}

		// XXX : JustifyLines(...) does not change the biggest line
		//                         it does not modify any Y coordinate of any character/bitmap
		//                         => the bounding_box of the whole text remains unchanged through this function
		bool Generator::JustifyLines(GeneratorParams const & params, GeneratorData & generator_data)
		{
			// left align : nothing to do
			if (params.alignment == TextAlignment::LEFT)
				return true;

			// justifaction : cannot increase line size if the factor is below 1.0
			if (params.alignment == TextAlignment::JUSTIFY && params.justify_space_factor <= 1.0f)
				return true;

			// compute the whole text bounding box
			glm::vec2 min_position;
			glm::vec2 max_position;
			if (!GetBoundingBox(generator_data.result.token_lines, min_position, max_position)) // nothing to do for empty sprite
				return true;

			// apply the modifications
			float W1 = max_position.x - min_position.x;
			for (TokenLine & line : generator_data.result.token_lines)
			{
				glm::vec2 min_line_position;
				glm::vec2 max_line_position;
				if (GetBoundingBox(line, min_line_position, max_line_position))
				{
					float W2 = max_line_position.x - min_line_position.x;

					// current line size is exactly the biggest line. No modification to do
					if (W1 == W2)
						continue;

					// right align
					if (params.alignment == TextAlignment::RIGHT)
					{
						MoveParticles(line, glm::vec2(W1 - W2, 0.0f));
					}
					// center align
					else if (params.alignment == TextAlignment::CENTER)
					{
						MoveParticles(line, glm::vec2((W1 - W2) * 0.5f, 0.0f));
					}
					// justification
					else if (params.alignment == TextAlignment::JUSTIFY && W2 < W1) // cannot justify to decrease line size 
					{
						// count the total size of whitespace token						
						float whitespace_width = 0.0f;
						for (Token const & token : line)
						{
							if (token.IsWhitespaceCharacter())
							{
								float factor = MathTools::CastAndDiv<float>(params.line_height, token.font_info->ascender - token.font_info->descender);

								whitespace_width += factor * token.character_layout->advance.x;
							}
						}

						// no whitespace, we cannot redistribute extra size => next line
						if (whitespace_width == 0.0f)
							continue;

						// compute the scale factor to apply to each whitespace : W1 = W2 + whitespace_width * whitespace_scale_factor

						float whitespace_scale_factor = (W1 - W2) / whitespace_width;

						// the scale factor to be applied is greater than what we want. Abandon fully the idea of justification
						if (whitespace_scale_factor > params.justify_space_factor)
							continue;

						// redistribute extra space
						float offset = 0.0f;
						for (Token & token : line)
						{
							token.corners.bottomleft.x += offset;
							token.corners.topright.x += offset;
							if (token.IsWhitespaceCharacter())
							{
								float factor = MathTools::CastAndDiv<float>(params.line_height, token.font_info->ascender - token.font_info->descender);

								offset += factor * token.character_layout->advance.x * whitespace_scale_factor;
							}
						}
					}
				}
			}
			return true;
		}

		ParticleAllocationBase * CreateTextAllocation(ParticleLayerBase * layer, GeneratorResult const & generator_result, CreateTextAllocationParams const & allocation_params)
		{
			assert(layer != nullptr);

			// check for compatibility
			if (!layer->IsParticleClassCompatible<ParticleDefault>())
			{
				assert(0);
				LogTools::Error("ParticleTextGenerator::CreateTextAllocation => IsParticleClassCompatible failure");
				return nullptr;
			}

			int extra_background = (allocation_params.create_background) ? 1 : 0;

			// create the allocation
			ParticleAllocationBase * result = layer->SpawnParticles(generator_result.GetTokenCount() + extra_background);
			if (result == nullptr)
				return nullptr;

			// spawn the particles
			ParticleAccessor<ParticleDefault> particles = result->GetParticleAccessor();
			if (particles.GetDataCount() == 0)
			{
				result->SubReference(); // error => destroy the allocation
				return nullptr;
			}

			size_t token_index = 0;
			// create the background
			if (allocation_params.create_background)
			{
				ParticleDefault & p = particles[token_index];

				glm::vec2 padding = glm::vec2(allocation_params.background_padding, allocation_params.background_padding);

				p.bounding_box = chaos::box2(std::make_pair(
					generator_result.bounding_box.bottomleft - padding,
					generator_result.bounding_box.topright   + padding));
				p.texcoords.bitmap_index = -1;
				p.texcoords.bottomleft   = glm::vec2(-1.0f, -1.0f);
				p.texcoords.topright     = glm::vec2(-1.0f, -1.0f);
				p.color = allocation_params.background_color;
				++token_index;
			}

			// convert the text			
			for (size_t i = 0; i < generator_result.token_lines.size(); ++i)
			{
				ParticleTextGenerator::TokenLine const & line = generator_result.token_lines[i];
				for (size_t j = 0; j < line.size(); ++j)
				{
					ParticleTextGenerator::Token const & token = line[j];

					ParticleDefault & p = particles[token_index];

					p.bounding_box = box2(std::make_pair(token.corners.bottomleft, token.corners.topright));
					p.texcoords = token.texcoords;
					p.color = token.color;
					++token_index;
				}
			}
			return result;
		}









#if 0






		bool Generator::GenerateSprites(char const * text, SpriteManager * sprite_manager, GeneratorResult * generator_result, GeneratorParams const & params)
		{
			assert(text != nullptr);

			// initialize parse params stack with a default style that defines current color and fonts
			GeneratorData generator_data(atlas);

			StyleDefinition style;
			style.color = params.default_color;
			style.font_info = generator_data.GetFontInfoFromName(params.font_info_name.c_str());
			generator_data.style_stack.push_back(style);

			// all steps to properly generate the result
			if (!GenerateLines(text, params, generator_data))
				return false;

			// break the lines
			if (!BreakLines(params, generator_data))
				return false;
			// justification
			if (!JustifyLines(params, generator_data))
				return false;
			// finally, recenter the whole sprites
			if (!MoveSpritesToHotpoint(params, generator_data))
				return false;


			// output the sprites if wanted
			if (sprite_manager != nullptr)
				if (!GenerateSprites(sprite_manager, params, generator_data))
					return false;

			// output the result if wanted
			if (generator_result != nullptr)
				std::swap(*generator_result, generator_data.generator_result);

			return true;
		}

		bool Generator::BreakLines(GeneratorParams const & params, GeneratorData & generator_data)
		{
			if (params.max_text_width <= 0.0f || !params.word_wrap)
				return true;

			GeneratorResult generator_result;

			float y = 0.0f;
			for (auto & line : generator_data.generator_result)
			{
				if (line.size() != 0)
					BreakOneLine(y, line, generator_result, params, generator_data);
				y -= params.line_height + params.line_spacing;
			}

			std::swap(generator_data.generator_result, generator_result);

			return true;
		}

		void Generator::BreakOneLine(float & y, TokenLine const & line, GeneratorResult & result_lines, GeneratorParams const & params, GeneratorData & generator_data)
		{
			bool leave_entry_unchanged = true;

			glm::vec2 min_line_position;
			glm::vec2 max_line_position;
			if (GetBoundingBox(line, min_line_position, max_line_position))
			{
				float W = max_line_position.x - min_line_position.x; // if the line is too small
				if (W > params.max_text_width)
				{
					//	std::vector<TokenGroup> token_group;
					//	BreakLineIntoWords(line, token_group);
					W = W;


				}
			}

			// copy the info, if nothing has been done
			if (leave_entry_unchanged)
				result_lines.push_back(line);
		}



		bool Generator::GenerateSprites(SpriteManager * sprite_manager, GeneratorParams const & params, GeneratorData & generator_data)
		{
			for (TokenLine const & line : generator_data.generator_result)
			{
				for (SpriteToken const & token : line)
				{
					if (token.IsBitmap())
						sprite_manager->AddSpriteBitmap(token.bitmap_layout, token.position, token.size, Hotpoint::BOTTOM_LEFT);
					else if (token.IsVisibleCharacter())
						sprite_manager->AddSpriteCharacter(token.character_layout, token.position, token.size, Hotpoint::BOTTOM_LEFT, token.color);
				}
			}
			return true;
		}

#endif



	}; // namespace ParticleTextGenerator
}; // namespace chaos