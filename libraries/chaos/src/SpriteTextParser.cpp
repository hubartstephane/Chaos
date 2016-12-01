#include <chaos/SpriteTextParser.h>

namespace chaos
{




#if 0
	// XXX : we do not draw space because, for justification reason we want to skip space and tabs at end of line
	//       we cannot afford to have pixels drawn in that case. We still have to insert an entry in the line because
	//       we may have another character coming

#endif



	// ============================================================
	// TextParseToken methods
	// ============================================================

	float TextParseToken::GetWidth(ParseTextParams const & params) const
	{
		if (character_entry != nullptr)
			return character_entry->width * (params.character_height / character_entry->height);
		if (bitmap_entry != nullptr)
			return bitmap_entry->width * (params.character_height / bitmap_entry->height);
		return 0.0f;
	}

	// ============================================================
	// TextParserData methods
	// ============================================================

	BitmapAtlas::CharacterSet const * TextParserData::GetCharacterSetFromName(char const * character_set_name) const
	{
		BitmapAtlas::CharacterSet const * result = atlas.GetCharacterSet(character_set_name);
		if (result == nullptr)
		{
			// for convinience, if we cannot find the character set, try to use the one on the top of the stack
			if (parse_stack.size() > 0)
				result = parse_stack.back().character_set;
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

	void TextParserData::PushDuplicate()
	{
		TextParseStackElement element = parse_stack.back();
		parse_stack.push_back(element); // push a duplicate of previous element
	}

	void TextParserData::PushCharacterSet(BitmapAtlas::CharacterSet const * character_set)
	{
		TextParseStackElement element = parse_stack.back();
		if (character_set != nullptr)
			element.character_set = character_set;
		parse_stack.push_back(element); // push a copy of previous element, except the character set
	}

	void TextParserData::PushColor(glm::vec3 const & color)
	{
		TextParseStackElement element = parse_stack.back();
		element.color = color;
		parse_stack.push_back(element); // push a copy of previous element, except the color
	}

	void TextParserData::EmitCharacters(char c, int count, ParseTextParams const & params)
	{
		// get current character set
		BitmapAtlas::CharacterSet const * character_set = parse_stack.back().character_set;
		if (character_set == nullptr)
			return;

		// get entry corresponding to the glyph
		BitmapAtlas::CharacterEntry const * entry = character_set->GetEntry(c);
		if (entry == nullptr)
			return;

		// emit the characters
		for (int i = 0 ; i < count ; ++i)
			EmitCharacter(c, entry, character_set, params);
	}


	void TextParserData::EmitCharacter(char c, BitmapAtlas::CharacterEntry const * entry, BitmapAtlas::CharacterSet const * character_set, ParseTextParams const & params)
	{
		TextParseToken token;
		token.type            = (c != ' ') ? TextParseToken::TOKEN_CHARACTER : TextParseToken::TOKEN_WHITESPACE;
		token.character       = c;
		token.character_entry = entry;
		token.character_set   = character_set;
		InsertTokenInLine(token, params);
	}

	void TextParserData::EmitBitmap(BitmapAtlas::BitmapEntry const * entry, ParseTextParams const & params)
	{
		TextParseToken token;
		token.type         = TextParseToken::TOKEN_BITMAP;
		token.bitmap_entry = entry;
		InsertTokenInLine(token, params);
	}

	void TextParserData::InsertTokenInLine(TextParseToken & token, ParseTextParams const & params)
	{
		// if there was no line, insert the very first one ...
		if (lines.size() == 0)
			lines.push_back(std::vector<TextParseToken>());

		// insert the token in the list and decal current cursor
		token.position = position;
		lines.back().push_back(token);
		position += token.GetWidth(params);
	}

	void TextParserData::EndCurrentLine(ParseTextParams const & params)
	{
		// update position
		position.x  = 0.0f;
		position.y -= params.character_height;
		// if there was no line, insert the very first one ...
		if (lines.size() == 0)
			lines.push_back(std::vector<TextParseToken>()); 
		// ... then you can add a new line
		lines.push_back(std::vector<TextParseToken>()); 
	}

	bool TextParserData::StartMarkup(char const * text, int & i, class TextParser & parser, ParseTextParams const & params)
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
				auto bitmap = parser.GetBitmap(markup.c_str());
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
				auto color = parser.GetColor(markup.c_str());
				if (color != nullptr)
				{
					PushColor(*color);
					return true;
				}
				// character set markup found
				auto character_set = parser.GetCharacterSet(markup.c_str());
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
	// TextParser methods
	// ============================================================

	glm::vec3 const * TextParser::GetColor(char const * name) const
	{
		auto it = colors.find(name);
		if (it == colors.end())
			return nullptr;
		return &it->second;
	}

	BitmapAtlas::BitmapEntry const * TextParser::GetBitmap(char const * name) const
	{
		auto it = bitmaps.find(name);
		if (it == bitmaps.end())
			return nullptr;
		return it->second;
	}
	BitmapAtlas::CharacterSet const * TextParser::GetCharacterSet(char const * name) const
	{
		auto it = character_sets.find(name);
		if (it == character_sets.end())
			return nullptr;
		return it->second;
	}

	bool TextParser::IsNameValid(char const * name) const
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

	bool TextParser::AddColor(char const * name, glm::vec3 const & color)
	{
		if (!IsNameValid(name))
			return false;
		colors.insert(std::make_pair(name, color));
		return true;
	}

	bool TextParser::AddCharacterSet(char const * name, char const * font_name)
	{
		assert(name      != nullptr);
		assert(font_name != nullptr);

		BitmapAtlas::CharacterSet const * character_set = atlas.GetCharacterSet(font_name);
		if (character_set == nullptr)
			return false;
		return AddCharacterSet(name, character_set);
	}

	bool TextParser::AddCharacterSet(char const * name, BitmapAtlas::CharacterSet const * character_set)
	{
		assert(character_set != nullptr);

		if (!IsNameValid(name))
			return false;
		character_sets.insert(std::make_pair(name, character_set));
		return true;
	}

	bool TextParser::AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name)
	{
		assert(name            != nullptr);
		assert(bitmap_set_name != nullptr);
		assert(bitmap_name     != nullptr);

		BitmapAtlas::BitmapSet const * bitmap_set = atlas.GetBitmapSet(bitmap_set_name);
		if (bitmap_set == nullptr)
			return false;
		BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name);
		if (entry == nullptr)
			return false;
		return AddBitmap(name, entry);
	}

	bool TextParser::AddBitmap(char const * name, BitmapAtlas::BitmapEntry const * entry)
	{
		assert(entry != nullptr);

		if (!IsNameValid(name))
			return false;
		bitmaps.insert(std::make_pair(name, entry));
		return true;
	}

	bool TextParser::ParseText(char const * text, ParseTextParams const & params)
	{
		assert(text != nullptr);

		// initialize parse params stack with a default element that defines current color and fonts
		TextParserData parse_data(atlas);

		TextParseStackElement element;
		element.color = params.default_color;
		element.character_set = parse_data.GetCharacterSetFromName(params.character_set_name.c_str());
		parse_data.parse_stack.push_back(element);

		// all steps to properly generate the result
		if (!GenerateLines(text, params, parse_data))
			return false;
		if (!RemoveUselessWhitespaces(params, parse_data))
			return false;
		if (!CutLines(params, parse_data))
			return false;
		if (!JustifyLines(params, parse_data))
			return false;
		if (!GenerateSprites(params, parse_data))
			return false;

		return true;
	}

	bool TextParser::GenerateLines(char const * text, ParseTextParams const & params, TextParserData & parse_data)
	{
		// iterate over all characters
		bool escape_character = false;
		for (int i = 0 ; text[i] != 0 ; ++i)
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
				parse_data.EndCurrentLine(params);
			}
			// tabulation : no different handling if previous character was an escape character
			else if (c == '\t') 
			{			
				parse_data.EmitCharacters(' ', (params.tab_size < 1)? 1 : params.tab_size, params);
			}
			// if escape is set, simply display the incomming character no matter what it is (except \n \r \t)
			else if (escape_character) 
			{
				parse_data.EmitCharacters(c, 1, params);	
			}
			// start an escape
			else if (new_escape_character) 
			{

			}
			// close previously started markup 
			else if (c == ']') 
			{
				if (parse_data.parse_stack.size() >= 1) // the very first element is manually inserted. It should never be popped
					return false;				
				parse_data.parse_stack.pop_back();
			}
			// start a new markup
			else if (c == '[') 
			{
				if (!parse_data.StartMarkup(text, ++i, *this, params)) // ill-formed markup
					return false;
			}
			// finally, this is not a special character  		
			else
			{
				parse_data.EmitCharacters(c, 1, params); 
			}

			escape_character = !escape_character && new_escape_character;
		}

		if (parse_data.parse_stack.size() != 1) // all markups should be correctly closed
			return false;

		return true;
	}

	bool TextParser::RemoveUselessWhitespaces(ParseTextParams const & params, TextParserData & parse_data)
	{
		// remove whitespaces at the end of lines
		for (auto & line : parse_data.lines)
			while (line.size() > 0 && line.back().type == TextParseToken::TOKEN_WHITESPACE)
				line.pop_back();
		// remove all empty lines at the end
		while (parse_data.lines.size() > 0 && parse_data.lines.back().size() == 0)
			parse_data.lines.pop_back();
		return true;
	}

	std::vector<std::pair<size_t, size_t>> TextParser::GroupTokens(std::vector<TextParseToken> const & line)
	{
		std::vector<std::pair<size_t, size_t>> result;

		size_t last_group_type = TextParseToken::TOKEN_NONE;

		size_t count = line.size();
		for (size_t i = 0; i < count; ++i)
		{
			TextParseToken const & token = line[i];
			if (token.type == TextParseToken::TOKEN_BITMAP)
				result.push_back(std::make_pair(i, i)); // bitmap are always in their own group        
			else if (token.type != last_group_type)
				result.push_back(std::make_pair(i, i)); // create a new group for this element of a new type          
			else
			{
				if (result.size() == 0)
					result.push_back(std::make_pair(i, i)); // insert the first group
				else
					++result.back().second = i; // concat element in its group
			}
			last_group_type = token.type;
		}

		return result;
	}

	bool TextParser::CutLines(ParseTextParams const & params, TextParserData & parse_data)
	{
		if (params.max_text_width > 0)
		{
			std::vector<std::vector<TextParseToken>> result_lines;

			float y = 0.0f;
			for (auto const & line : parse_data.lines)
			{
				// line may have been left empty after useless whitespace removal. 
				// Can simply ignore it, no sprites will be generated for that
				if (line.size() != 0)
					CutOneLine(y, line, result_lines, params, parse_data);
				// update the y position of characters
				y += params.character_height;      
			}
			parse_data.lines = std::move(result_lines); // replace the line after filtering
		}
		return true;
	}

	void TextParser::InsertAllTokensInLine(float & x, float & y, std::pair<size_t, size_t> const & group, std::vector<TextParseToken> const & line, std::vector<TextParseToken> & current_line)
	{
		for (size_t i = group.first ; i <= group.second ; ++i)
		{
			TextParseToken const & token = line[i]; // the token to insert


		}
	}

	void TextParser::FlushLine(
		float & x, float & y, 
		std::vector<TextParseToken> & current_line, 
		std::vector<std::vector<TextParseToken>> & result_lines,
		ParseTextParams const & params)
	{
		x  = 0.0f;
		y += params.character_height;

		result_lines.push_back(std::move(current_line));
		current_line = std::vector<TextParseToken>();
	}

	void TextParser::CutOneLine(
		float & y, 
		std::vector<TextParseToken> const & line, 
		std::vector<std::vector<TextParseToken>> & result_lines, 
		ParseTextParams const & params, 
		TextParserData & parse_data)
	{
		size_t initial_line_count = result_lines.size();

		// token are grouped as consecutive elements of same type
		// except for bitmaps that are in groups of one element
		// group = [index first element, index of last element]
		std::vector<std::pair<size_t, size_t>> token_groups = GroupTokens(line);

		// rules
		//   -an initial line may be splitted into multiples lines
		//   -do not insert WHITESPACES at the begining of thoses new lines (except for the very first line maybe)

		std::vector<TextParseToken> current_line;
		float x = 0.0f;

		// iterate over all groups a try to insert them in lines
		size_t group_count = token_groups.size();
		for (size_t i = 0; i < group_count; ++i)
		{
			auto const & group = token_groups[i];

			int group_type = line[group.first].type;

			if (group_type == TextParseToken::TOKEN_WHITESPACE)
			{
				if (result_lines.size() != initial_line_count) // this is an additionnal line
					if (current_line.size() == 0)                // that have no previous entry : skip the group
						continue;

				InsertAllTokensInLine(x, y, group, line, current_line); // insert all whitespaces in this line
			}
			else
			{
				float w1 = line[group.first].position.x;
				float w2 = line[group.second].position.x + line[group.second].GetWidth(params);

				float group_width = (w2 - w1);

				// the group cannot fully fit in this line. Is it worth splitting the group on several lines ?
				if (x + group_width > params.max_text_width)
				{
					// the group would fit entirely in the next line 
					if (group_width <= params.max_text_width)
					{
						FlushLine(x, y, current_line, result_lines, params);            
						InsertAllTokensInLine(x, y, group, line, current_line); // go to next line, insert the whole group
					}
					// the group is too big, even for empty line
					else
					{
						// try to reduce bitmap clamping by changing line
						if (x > 0.0f && group_type == TextParseToken::TOKEN_BITMAP)
						{
							FlushLine(x, y, current_line, result_lines, params);
							InsertAllTokensInLine(x, y, group, line, current_line); // go to next line, insert the bitmap (the group)
						}
						else
						{


						}
					}
				}
				// the group can fully be contained in this line
				else
				{
					InsertAllTokensInLine(x, y, group, line, current_line); // you can fully insert the group with no splitting
				}
			}
		}
	}

	bool TextParser::JustifyLines(ParseTextParams const & params, TextParserData & parse_data)
	{


		return true;
	}

	bool TextParser::GenerateSprites(ParseTextParams const & params, TextParserData & parse_data)
	{


		return true;
	}

};