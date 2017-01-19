#include <chaos/SpriteTextParser.h>
#include <chaos/MathTools.h>

namespace chaos
{
	// ============================================================
	// SpriteTextGeneratorData methods
	// ============================================================

	BitmapAtlas::CharacterSet const * SpriteTextGeneratorData::GetCharacterSetFromName(char const * character_set_name) const
	{
		BitmapAtlas::CharacterSet const * result = atlas.GetCharacterSet(character_set_name);
		if (result == nullptr)
		{
			// for convenience, if we cannot find the character set, try to use the one on the top of the stack
			if (generator_stack.size() > 0)
				result = generator_stack.back().character_set;
			// if we still have no character set, take the very first available
			if (result == nullptr)
			{
				auto const & character_sets = atlas.GetCharacterSets();
				if (character_sets.size() > 0)
					result = character_sets[0].get();
			}
		}
		return result;
	}

	void SpriteTextGeneratorData::PushDuplicate()
	{
		SpriteTextStackElement element = generator_stack.back();
		generator_stack.push_back(element); // push a duplicate of previous element
	}

	void SpriteTextGeneratorData::PushCharacterSet(BitmapAtlas::CharacterSet const * character_set)
	{
		SpriteTextStackElement element = generator_stack.back();
		if (character_set != nullptr)
			element.character_set = character_set;
		generator_stack.push_back(element); // push a copy of previous element, except the character set
	}

	void SpriteTextGeneratorData::PushColor(glm::vec3 const & color)
	{
		SpriteTextStackElement element = generator_stack.back();
		element.color = color;
		generator_stack.push_back(element); // push a copy of previous element, except the color
	}

	void SpriteTextGeneratorData::EmitCharacters(char c, int count, SpriteTextGeneratorParams const & params)
	{
		// get current character set
		BitmapAtlas::CharacterSet const * character_set = generator_stack.back().character_set;
		if (character_set == nullptr)
			return;

		// get entry corresponding to the glyph
		BitmapAtlas::CharacterEntry const * entry = character_set->GetEntry(c);
		if (entry == nullptr)
			return;

		// emit the characters
		for (int i = 0; i < count; ++i)
			EmitCharacter(c, entry, character_set, params);
	}


	void SpriteTextGeneratorData::EmitCharacter(char c, BitmapAtlas::CharacterEntry const * entry, BitmapAtlas::CharacterSet const * character_set, SpriteTextGeneratorParams const & params)
	{
		SpriteTextToken token;
		token.character = c;
		token.character_entry = entry;
		token.character_set = character_set;
		token.color = generator_stack.back().color;
		InsertTokenInLine(token, params);
	}

	void SpriteTextGeneratorData::EmitBitmap(BitmapAtlas::BitmapEntry const * entry, SpriteTextGeneratorParams const & params)
	{
		SpriteTextToken token;
		token.bitmap_entry = entry;
		InsertTokenInLine(token, params);
	}

	void SpriteTextGeneratorData::InsertTokenInLine(SpriteTextToken & token, SpriteTextGeneratorParams const & params)
	{
		// if there was no line, insert the very first one ...
		if (generator_result.size() == 0)
			generator_result.push_back(SpriteTextLine());

		// insert the token
		if (token.bitmap_entry != nullptr)
		{
			// restrict the bitmap to the size of the line
			float factor = MathTools::CastAndDiv<float>(params.line_height - 2 * params.bitmap_padding.y, token.bitmap_entry->height);

			token.position = bitmap_position + params.bitmap_padding;

			token.size.x = factor * (float)token.bitmap_entry->width;
			token.size.y = params.line_height - 2 * params.bitmap_padding.y;

			bitmap_position.x += token.size.x + params.character_spacing + params.bitmap_padding.x * 2.0f;
			character_position.x += token.size.x + params.character_spacing + params.bitmap_padding.x * 2.0f;
		}
		else if (token.character_entry != nullptr)
		{
			// get the descender 
			SpriteTextStackElement const & context = generator_stack.back();
			float descender = (context.character_set == nullptr) ? 0.0f : context.character_set->descender;

			// scale the character back to the size of the scanline
			float factor = MathTools::CastAndDiv<float>(params.line_height, token.character_set->ascender - token.character_set->descender);

			token.position = character_position - glm::vec2(0.0f, descender) + // character_position.y is BELOW the scanline (at the descender level)
				factor * glm::vec2(
				(float)(token.character_entry->bitmap_left),
				(float)(token.character_entry->bitmap_top - token.character_entry->height) // XXX : -token.character_entry->height => to have BOTTOM LEFT CORNER
				);

			token.size.x = factor * (float)token.character_entry->width;
			token.size.y = factor * (float)token.character_entry->height;
		
			// XXX : Some fonts are in italic. The 'advance' cause some 'overide' in character bounding box.
			//       That's great for characters that are near one another
			//       But with bitmap that causes real overide.
			//       => that's why we are using two position : 'bitmap_position' & 'character_position'
			bitmap_position.x = token.position.x + params.character_spacing + token.size.x;
			character_position.x = token.position.x + params.character_spacing + factor * (float)(token.character_entry->advance.x); 
		}
		generator_result.back().push_back(token);
	}

	void SpriteTextGeneratorData::EndCurrentLine(SpriteTextGeneratorParams const & params)
	{
		// update position
		float delta_y = params.line_height + params.line_spacing;

		bitmap_position.x = 0.0f;
		bitmap_position.y -= delta_y;
		character_position.x = 0.0f;
		character_position.y -= delta_y;
		// if there was no line, insert the very first one ...
		if (generator_result.size() == 0)
			generator_result.push_back(SpriteTextLine());
		// ... then you can add a new line
		generator_result.push_back(SpriteTextLine());
	}

	bool SpriteTextGeneratorData::StartMarkup(char const * text, int & i, class SpriteTextGenerator & generator, SpriteTextGeneratorParams const & params)
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
				auto bitmap = generator.GetBitmap(markup.c_str());
				if (bitmap != nullptr)
				{
					if (c == ']')
					{
						EmitBitmap(bitmap, params);
						return true;
					}
					return false; // ill-formed string
				}
				// if ']' is found, do nothing because, we are about to push a color/character set on the stack that is to be immediatly popped
				if (c == ']')
					return true;
				// color markup found
				auto color = generator.GetColor(markup.c_str());
				if (color != nullptr)
				{
					PushColor(*color);
					return true;
				}
				// character set markup found
				auto character_set = generator.GetCharacterSet(markup.c_str());
				if (character_set != nullptr)
				{
					PushCharacterSet(character_set);
					return true;
				}
				// a markup has been detected but we don'k know to what it corresponds, so push a duplicate on the stack 
				// because we expect a markup closure later
				PushDuplicate();
				return true;
			}
			++i;
		}
		return false; // markup started, but not finished : ill-formed
	}

	// ============================================================
	// SpriteTextGenerator methods
	// ============================================================

	glm::vec3 const * SpriteTextGenerator::GetColor(char const * name) const
	{
		auto it = colors.find(name);
		if (it == colors.end())
			return nullptr;
		return &it->second;
	}

	BitmapAtlas::BitmapEntry const * SpriteTextGenerator::GetBitmap(char const * name) const
	{
		auto it = bitmaps.find(name);
		if (it == bitmaps.end())
			return nullptr;
		return it->second;
	}
	BitmapAtlas::CharacterSet const * SpriteTextGenerator::GetCharacterSet(char const * name) const
	{
		auto it = character_sets.find(name);
		if (it == character_sets.end())
			return nullptr;
		return it->second;
	}

	bool SpriteTextGenerator::IsNameValid(char const * name) const
	{
		// ignore empty name
		if (name == nullptr)
			return false;
		// ensure name is a valid variable name
		if (!StringTools::IsVariableName(name))
			return false;
		// ensure name is not already used by a color
		if (colors.find(name) != colors.end())
			return false;
		// ensure name is not already used by a bitmap
		if (bitmaps.find(name) != bitmaps.end())
			return false;
		// ensure name is not already used by a character set
		if (character_sets.find(name) != character_sets.end())
			return false;
		return true;
	}

	bool SpriteTextGenerator::AddColor(char const * name, glm::vec3 const & color)
	{
		if (!IsNameValid(name))
			return false;
		colors.insert(std::make_pair(name, color));
		return true;
	}

	bool SpriteTextGenerator::AddCharacterSet(char const * name, char const * font_name)
	{
		assert(name != nullptr);
		assert(font_name != nullptr);

		BitmapAtlas::CharacterSet const * character_set = atlas.GetCharacterSet(font_name);
		if (character_set == nullptr)
			return false;
		return AddCharacterSet(name, character_set);
	}

	bool SpriteTextGenerator::AddCharacterSet(char const * name, BitmapAtlas::CharacterSet const * character_set)
	{
		assert(character_set != nullptr);

		if (!IsNameValid(name))
			return false;
		character_sets.insert(std::make_pair(name, character_set));
		return true;
	}

	bool SpriteTextGenerator::AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name)
	{
		assert(name != nullptr);
		assert(bitmap_set_name != nullptr);
		assert(bitmap_name != nullptr);

		BitmapAtlas::BitmapSet const * bitmap_set = atlas.GetBitmapSet(bitmap_set_name);
		if (bitmap_set == nullptr)
			return false;
		BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name);
		if (entry == nullptr)
			return false;
		return AddBitmap(name, entry);
	}

	bool SpriteTextGenerator::AddBitmap(char const * name, BitmapAtlas::BitmapEntry const * entry)
	{
		assert(entry != nullptr);

		if (!IsNameValid(name))
			return false;
		bitmaps.insert(std::make_pair(name, entry));
		return true;
	}



	bool SpriteTextGenerator::GenerateLines(char const * text, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
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
				generator_data.EndCurrentLine(params);
			}
			// tabulation : no different handling if previous character was an escape character
			else if (c == '\t')
			{
				generator_data.EmitCharacters(' ', (params.tab_size < 1) ? 1 : params.tab_size, params);
			}
			// if escape is set, simply display the incoming character no matter what it is (except \n \r \t)
			else if (escape_character)
			{
				generator_data.EmitCharacters(c, 1, params);
			}
			// start an escape
			else if (new_escape_character)
			{

			}
			// close previously started markup 
			else if (c == ']')
			{
				if (generator_data.generator_stack.size() <= 1) // the very first element is manually inserted. It should never be popped
					return false;
				generator_data.generator_stack.pop_back();
			}
			// start a new markup
			else if (c == '[')
			{
				if (!generator_data.StartMarkup(text, ++i, *this, params)) // ill-formed markup
					return false;
			}
			// finally, this is not a special character  		
			else
			{
				generator_data.EmitCharacters(c, 1, params);
			}

			escape_character = !escape_character && new_escape_character;
		}

		if (generator_data.generator_stack.size() != 1) // all markups should be correctly closed (except the very first we have manually inserted)
			return false;

		return true;
	}

	bool SpriteTextGenerator::GetBoundingBox(SpriteTextLine const & generator_line, glm::vec2 & min_line_position, glm::vec2 & max_line_position) const
	{
		min_line_position.x = min_line_position.y = std::numeric_limits<float>::max();
		max_line_position.x = max_line_position.y = -std::numeric_limits<float>::max();

		if (generator_line.size() == 0)
			return false;

		for (SpriteTextToken const & token : generator_line)
		{
			min_line_position = glm::min(min_line_position, token.position);
			max_line_position = glm::max(max_line_position, token.position + token.size);
		}
		return true;
	}

	bool SpriteTextGenerator::GetBoundingBox(SpriteTextResult const & generator_result, glm::vec2 & min_position, glm::vec2 & max_position) const
	{
		bool result = false;
		if (generator_result.size() > 0)
		{
			min_position.x = min_position.y = std::numeric_limits<float>::max();
			max_position.x = max_position.y = -std::numeric_limits<float>::max();

			for (auto const & line : generator_result)
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

	void SpriteTextGenerator::MoveSprites(SpriteTextLine & generator_line, glm::vec2 const & offset)
	{
		if (offset.x != 0.0f || offset.y != 0.0f)
			for (SpriteTextToken & token : generator_line)
				token.position += offset;
	}

	void SpriteTextGenerator::MoveSprites(SpriteTextResult & generator_result, glm::vec2 const & offset)
	{
		if (offset.x != 0.0f || offset.y != 0.0f)
			for (auto & line : generator_result)
				for (SpriteTextToken & token : line)
					token.position += offset;
	}

	bool SpriteTextGenerator::MoveSpritesToHotpoint(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
	{
		// compute the min/max bounding box
		glm::vec2 min_position;
		glm::vec2 max_position;
		if (!GetBoundingBox(generator_data.generator_result, min_position, max_position)) // no sprite, nothing to do
			return true;

		// displace all the sprites to match the position
		glm::vec2 offset =
			params.position -
			Hotpoint::Convert(min_position, max_position - min_position, Hotpoint::BOTTOM_LEFT, params.hotpoint_type);

		MoveSprites(generator_data.generator_result, offset);

		return true;
	}

	bool SpriteTextGenerator::GenerateSprites(char const * text, SpriteManager * sprite_manager, SpriteTextResult * generator_result, SpriteTextGeneratorParams const & params)
	{
		assert(text != nullptr);

		// initialize parse params stack with a default element that defines current color and fonts
		SpriteTextGeneratorData generator_data(atlas);

		SpriteTextStackElement element;
		element.color = params.default_color;
		element.character_set = generator_data.GetCharacterSetFromName(params.character_set_name.c_str());
		generator_data.generator_stack.push_back(element);

		// all steps to properly generate the result
		if (!GenerateLines(text, params, generator_data))
			return false;

		


		



	#if 0

		if (!RemoveUselessWhitespaces(generator_data))
			return false;
		if (!CutLines(params, generator_data))
			return false;
		if (!JustifyLines(params, generator_data))
			return false;
		if (!RemoveWhitespaces(generator_data))
			return false;
	#endif
		if (!MoveSpritesToHotpoint(params, generator_data))
			return false;

		// output the sprites if wanted
		if (sprite_manager != nullptr)
			if (!GenerateSprites(sprite_manager, params, generator_data))
				return false;

		// output the result if wanted
		if (generator_result != nullptr)
			*generator_result = std::move(generator_data.generator_result);

		return true;
	}







#if 0

	bool SpriteTextGenerator::RemoveWhitespaces(SpriteTextGeneratorData & generator_data)
	{
		for (auto & line : generator_data.generator_result)
		{
			auto it = std::remove_if(line.begin(), line.end(), [](SpriteTextToken const & token) {
				return (token.type == SpriteTextToken::TOKEN_WHITESPACE);
			});

			line.erase(it, line.end());
		}
		return true;
	}

	bool SpriteTextGenerator::RemoveUselessWhitespaces(SpriteTextGeneratorData & generator_data)
	{
		// remove whitespace at the end of lines
		for (auto & line : generator_data.generator_result)
			while (line.size() > 0 && line.back().type == SpriteTextToken::TOKEN_WHITESPACE)
				line.pop_back();
		// remove all empty lines at the end
		while (generator_data.generator_result.size() > 0 && generator_data.generator_result.back().size() == 0)
			generator_data.generator_result.pop_back();
		return true;
	}

	bool SpriteTextGenerator::CutLines(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
	{
		if (params.max_text_width > 0)
		{
			SpriteTextResult generator_result;

			float y = 0.0f;
			for (auto const & line : generator_data.generator_result)
			{
				// line may have been left empty after useless whitespace removal. 
				// Can simply ignore it, no sprites will be generated for that
				if (line.size() != 0)
					CutOneLine(y, line, generator_result, params, generator_data);
				// update the y position of characters
				y -= params.line_height + params.line_spacing;
			}
			generator_data.generator_result = std::move(generator_result); // replace the line after filtering
		}
		return true;
	}

	void SpriteTextGenerator::FlushLine(float & x, float & y, SpriteTextLine & current_line, SpriteTextResult & generator_result, SpriteTextGeneratorParams const & params)
	{
		x = 0.0f;
		y += params.line_height;

		generator_result.push_back(std::move(current_line));
		current_line = SpriteTextLine();
	}

	void SpriteTextGenerator::CutOneLine(float & y, SpriteTextLine const & line, SpriteTextResult & generator_result, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
	{

	}

	bool SpriteTextGenerator::JustifyLines(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
	{
		if (params.alignment == SpriteTextGeneratorParams::ALIGN_LEFT)
			return true;

		glm::vec2 min_position;
		glm::vec2 max_position;
		if (!GetBoundingBox(generator_data.generator_result, min_position, max_position)) // nothing to do for empty sprite
			return true;

		float W1 = max_position.x - min_position.x;
		for (SpriteTextLine & line : generator_data.generator_result)
		{
			glm::vec2 min_line_position;
			glm::vec2 max_line_position;
			if (GetBoundingBox(line, min_line_position, max_line_position))
			{
				float W2 = max_line_position.x - min_line_position.x;
				if (params.alignment == SpriteTextGeneratorParams::ALIGN_RIGHT)
				{
					MoveSprites(line, glm::vec2(W1 - W2, 0.0f));
				}
				else if (params.alignment == SpriteTextGeneratorParams::ALIGN_CENTER)
				{
					MoveSprites(line, glm::vec2((W1 - W2) * 0.5f, 0.0f));
				}
				else if (params.alignment == SpriteTextGeneratorParams::ALIGN_JUSTIFY)
				{
					if (W1 != W2)
					{
						// count the number of whitespace token
						int   whitespace_count = 0;
						float whitespace_width = 0.0f;
						for (SpriteTextToken const & token : line)
						{
							// XXX : there may be multiple fonts on the same line, so several whitespace size.
							//       there is no universal reference for a whitespace size
							if (token.type == SpriteTextToken::TOKEN_CHARACTER)
								whitespace_width = max(whitespace_width, token.size.x); // considere that the widdest character is a reference for whitespace size
							else if (token.type == SpriteTextToken::TOKEN_WHITESPACE)
								++whitespace_count;
						}
						// no whitespace, we cannot redistribute extra size => next line
						if (whitespace_count == 0 || whitespace_width == 0.0f)
							continue;

						// count how much space must be redistributed for each whitespace
						float extra_whitespace_width = MathTools::CastAndDiv<float>(W1 - W2, whitespace_count);

						// new whitespace cannot be X time greater than initial one
						static float const FACTOR_LIMIT = 2.0f;
						if ((whitespace_width + extra_whitespace_width) > FACTOR_LIMIT * whitespace_width) // => else ignore
							continue;

						// redistribute extra space
						float offset = 0.0f;
						for (SpriteTextToken & token : line)
						{
							token.position.x += offset;
							if (token.type == SpriteTextToken::TOKEN_WHITESPACE)
								offset += extra_whitespace_width;
						}
					}
				}
			}
		}
		return true;
	}

#endif

	bool SpriteTextGenerator::GenerateSprites(SpriteManager * sprite_manager, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data)
	{
		for (SpriteTextLine const & line : generator_data.generator_result)
		{
			for (SpriteTextToken const & token : line)
			{
				if (token.IsBitmap())
					sprite_manager->AddSpriteBitmap(token.bitmap_entry, token.position, token.size, Hotpoint::BOTTOM_LEFT);
				else if (token.IsVisibleCharacter())
					sprite_manager->AddSpriteCharacter(token.character_entry, token.position, token.size, Hotpoint::BOTTOM_LEFT, token.color);
			}
		}
		return true;
	}

};