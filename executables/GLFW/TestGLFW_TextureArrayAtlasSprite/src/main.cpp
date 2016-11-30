#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/MathTools.h>



#include <boost/algorithm/string.hpp>


// --------------------------------------------------------------------

//
// accepted markups: 
//
//   [COLORNAME 
//   <COLOR=MyColor> ... </COLOR> 
//   <FONT=MyFont>   ... </FONT>
//   <BITMAP=MyBitmap>
//
//

/** during parsing, some 'commands' are put on the stack for text formating, TextParseStackElement is such a command */
class TextParseStackElement
{
public:

	/** the color to use */
	glm::vec3 color;
	/** the character set selected */
	chaos::BitmapAtlas::CharacterSet const * character_set{nullptr};
};

class TextParseToken
{
public:

	static int const TOKEN_NONE       = 0;
	static int const TOKEN_BITMAP     = 1;
	static int const TOKEN_CHARACTER  = 2;
	static int const TOKEN_WHITESPACE = 3;

  /** get the width of the token after renormalization */
  float GetWidth(class ParseTextParams const & params) const;

public:

	/** the type of the token */
	int type{TOKEN_NONE};
  /** the character */
  char character{0};
	/** the position of the generated image */
	glm::vec2 position{0.0f, 0.0f};
	/** the corresponding bitmap (if valid) */
	chaos::BitmapAtlas::BitmapEntry const * bitmap_entry{nullptr};
	/** the corresponding character (if valid) */
	chaos::BitmapAtlas::CharacterEntry const * character_entry{nullptr};
  /** the corresponding character set (if valid) */
  chaos::BitmapAtlas::CharacterSet const * character_set{ nullptr };
};


/** a structure used to contains data during parsing */
class TextParserData
{
public:

	/** the constructor */
  TextParserData(chaos::BitmapAtlas::AtlasBase const & in_atlas) : atlas(in_atlas) {}

	/** start the markup */
	bool StartMarkup(char const * text, int & i, class TextParser & parser, ParseTextParams const & params);
	/** utility method to emit characters */
	void EmitCharacters(char c, int count, ParseTextParams const & params);
	/** utility method to emit character */
	void EmitCharacter(char c, chaos::BitmapAtlas::CharacterEntry const * entry, chaos::BitmapAtlas::CharacterSet const * character_set, ParseTextParams const & params);
	/** emit a bitmap */
	void EmitBitmap(chaos::BitmapAtlas::BitmapEntry const * entry, ParseTextParams const & params);
	/** end the current line */
	void EndCurrentLine(ParseTextParams const & params);
  /** insert a token */
  void InsertTokenInLine(TextParseToken & token, ParseTextParams const & params);

	/** duplicate the last stack element */
	void PushDuplicate();
	/** add an element on parse stack : keep color, but change current character_set */
	void PushCharacterSet(chaos::BitmapAtlas::CharacterSet const * character_set);
	/** add an element on parse stack : keep character_set, but change current color */
	void PushColor(glm::vec3 const & color);

	/** get a character set from its name */
	chaos::BitmapAtlas::CharacterSet const * GetCharacterSetFromName(char const * character_set_name) const;

public:

  /** the atlas in used */
  chaos::BitmapAtlas::AtlasBase const & atlas;
	/** current line position */
	glm::vec2 position{0.0f, 0.0f};
	/** the lines */
	std::vector<std::vector<TextParseToken>> lines;
	/** the stack used for parsing */
	std::vector<TextParseStackElement> parse_stack;
};


/** some parameters used during text parsing */
class ParseTextParams
{
public:

	static const int ALIGN_LEFT = 0;
	static const int ALIGN_RIGHT = 1;
	static const int ALIGN_CENTER = 2;
	static const int ALIGN_JUSTIFY = 3;

	/** the size to use for the characters */
	float character_height{ 32.0f };
	/** the text limits */
	float max_text_width{ 0.0f };
	/** word wrap enabled */
	bool word_wrap{ true };
	/** the line alignment */
	int alignment{ ALIGN_LEFT };
	/** the color to use by default */
	glm::vec3 default_color{ 1.0f, 1.0f, 1.0f };
	/** the font to use by default */
	std::string character_set_name;
	/** tab size */
	int tab_size{ 2 };
};


class TextParser
{
public:

	/** constructor with atlas initialization */
  TextParser(chaos::BitmapAtlas::AtlasBase const & in_atlas) : atlas(in_atlas) {}

	/** add a named color in the parser */
	bool AddColor(char const * name, glm::vec3 const & color);
	/** add a named bitmap in the parser */
	bool AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name);
	/** add a named bitmap in the parser */
	bool AddBitmap(char const * name, chaos::BitmapAtlas::BitmapEntry const * entry);

	/** add a named character set in the parser */
	bool AddCharacterSet(char const * name, char const * font_name);
	/** add a named character set in the parser */
	bool AddCharacterSet(char const * name, chaos::BitmapAtlas::CharacterSet const * character_set);

	/** the main method to parse a text */
	bool ParseText(char const * text, ParseTextParams const & params = ParseTextParams());

	/** get a color by its name */
	glm::vec3 const * GetColor(char const * name) const;
	/** get a bitmap by its name */
	chaos::BitmapAtlas::BitmapEntry const * GetBitmap(char const * name) const;
	/** get a character set by its name */
	chaos::BitmapAtlas::CharacterSet const * GetCharacterSet(char const * name) const;
	/** test whether a name is a key in one of the following maps : colors, bitmaps, character_sets */
	bool IsNameValid(char const * name) const;

protected:

  /** generate the lines, without cutting them */
	bool GenerateLines(char const * text, ParseTextParams const & params, TextParserData & parse_data);
  /** cut the lines so they are not too big. Cut them only when it is possible */
  bool CutLines(ParseTextParams const & params, TextParserData & parse_data);
  /** utility method to cut one line an insert it into a new result */
  void CutOneLine(float & y, std::vector<TextParseToken> const & line, std::vector<std::vector<TextParseToken>> & result_lines, ParseTextParams const & params, TextParserData & parse_data);
  /** goto next line */
  void FlushLine(float & x, float & y, std::vector<TextParseToken> & current_line, std::vector<std::vector<TextParseToken>> & result_lines, ParseTextParams const & params);
  /** insert all tokens of a group in one line */
  void InsertAllTokensInLine(float & x, float & y, std::pair<size_t, size_t> const & group, std::vector<TextParseToken> const & line, std::vector<TextParseToken> & current_line);
  /** remove whitespaces at end of lines, and empty lines at the end */
  bool RemoveUselessWhitespaces(ParseTextParams const & params, TextParserData & parse_data);
  /** update lines according to justification */
  bool JustifyLines(ParseTextParams const & params, TextParserData & parse_data);
  /** generate the sprites */
  bool GenerateSprites(ParseTextParams const & params, TextParserData & parse_data);
  /** group tokens */
  std::vector<std::pair<size_t, size_t>> GroupTokens(std::vector<TextParseToken> const & line);

public:

	/** the colors to use, indexed by a joker name */
	std::map<std::string, glm::vec3, chaos::StringTools::ci_less> colors;
	/** the bitmaps to use, indexed by a joker name */
	std::map<std::string, chaos::BitmapAtlas::BitmapEntry const *, chaos::StringTools::ci_less> bitmaps;
	/** the character_set to use, indexed by a joker name */
	std::map<std::string, chaos::BitmapAtlas::CharacterSet const *, chaos::StringTools::ci_less> character_sets;
	/** the atlas where to find entries */
	chaos::BitmapAtlas::AtlasBase const & atlas;
};



















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

chaos::BitmapAtlas::CharacterSet const * TextParserData::GetCharacterSetFromName(char const * character_set_name) const
{
	chaos::BitmapAtlas::CharacterSet const * result = atlas.GetCharacterSet(character_set_name);
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

void TextParserData::PushCharacterSet(chaos::BitmapAtlas::CharacterSet const * character_set)
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
	chaos::BitmapAtlas::CharacterSet const * character_set = parse_stack.back().character_set;
	if (character_set == nullptr)
		return;

	// get entry corresponding to the glyph
	chaos::BitmapAtlas::CharacterEntry const * entry = character_set->GetEntry(c);
	if (entry == nullptr)
		return;

	// emit the characters
	for (int i = 0 ; i < count ; ++i)
		EmitCharacter(c, entry, character_set, params);
}


void TextParserData::EmitCharacter(char c, chaos::BitmapAtlas::CharacterEntry const * entry, chaos::BitmapAtlas::CharacterSet const * character_set, ParseTextParams const & params)
{
  TextParseToken token;
  token.type            = (c != ' ') ? TextParseToken::TOKEN_CHARACTER : TextParseToken::TOKEN_WHITESPACE;
  token.character       = c;
  token.character_entry = entry;
  token.character_set   = character_set;
  InsertTokenInLine(token, params);
}

void TextParserData::EmitBitmap(chaos::BitmapAtlas::BitmapEntry const * entry, ParseTextParams const & params)
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

		if (!chaos::StringTools::IsVariableCharacter(c)) // searched string is contained in  [j .. i-1]
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

chaos::BitmapAtlas::BitmapEntry const * TextParser::GetBitmap(char const * name) const
{
	auto it = bitmaps.find(name);
	if (it == bitmaps.end())
		return nullptr;
	return it->second;
}
chaos::BitmapAtlas::CharacterSet const * TextParser::GetCharacterSet(char const * name) const
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
	if (!chaos::StringTools::IsVariableName(name))
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

	chaos::BitmapAtlas::CharacterSet const * character_set = atlas.GetCharacterSet(font_name);
	if (character_set == nullptr)
		return false;
	return AddCharacterSet(name, character_set);
}

bool TextParser::AddCharacterSet(char const * name, chaos::BitmapAtlas::CharacterSet const * character_set)
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

	chaos::BitmapAtlas::BitmapSet const * bitmap_set = atlas.GetBitmapSet(bitmap_set_name);
	if (bitmap_set == nullptr)
		return false;
	chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name);
	if (entry == nullptr)
		return false;
	return AddBitmap(name, entry);
}

bool TextParser::AddBitmap(char const * name, chaos::BitmapAtlas::BitmapEntry const * entry)
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



#if 0
// XXX : we do not draw space because, for justification reason we want to skip space and tabs at end of line
//       we cannot afford to have pixels drawn in that case. We still have to insert an entry in the line because
//       we may have another character coming

#endif

















// -----------------------------------------------

class SpriteVertex
{
public:

  glm::vec2 position;
  glm::vec3 texcoord;


};

class SpriteManager
{
public:

  /** theses are the offset are the ones to apply to position, if position correspond to a given border */

  static int const HOTPOINT_LEFT    = 1;
  static int const HOTPOINT_RIGHT   = 2;
  static int const HOTPOINT_HMIDDLE = 0;

  static int const HOTPOINT_BOTTOM  = 4;
  static int const HOTPOINT_TOP     = 8;
  static int const HOTPOINT_VMIDDLE = 0;

  static int const HOTPOINT_BOTTOM_LEFT  = HOTPOINT_BOTTOM  | HOTPOINT_LEFT;
  static int const HOTPOINT_TOP_LEFT     = HOTPOINT_TOP     | HOTPOINT_LEFT;
  static int const HOTPOINT_BOTTOM_RIGHT = HOTPOINT_BOTTOM  | HOTPOINT_RIGHT;
  static int const HOTPOINT_TOP_RIGHT    = HOTPOINT_TOP     | HOTPOINT_RIGHT;
  static int const HOTPOINT_CENTER       = HOTPOINT_VMIDDLE | HOTPOINT_HMIDDLE;

  /** given a hotpoint, a sprite size, and the hotpoint_type, returns the BottomLeft hotpoint position */
  static glm::vec2 GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type);
  /** given a hotpoint, a sprite size, and the hotpoint_type, returns the any other hotpoint_type position */
  static glm::vec2 GetHotpointPosition(glm::vec2 const & position, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type);


  void Initialize();

  void AddSprite(chaos::BitmapAtlas::BitmapEntry * entry, glm::vec2 const & position, glm::vec2 const & size, glm::vec2 const & handler);

protected:

  /** the vertex array */
  boost::intrusive_ptr<chaos::VertexArray> vertex_array;
  /** the vertex buffer */
  boost::intrusive_ptr<chaos::VertexBuffer> vertex_buffer;
  /** the declaration of the vertex buffer */
  chaos::VertexDeclaration declaration;
  /** the texture atlas */
  chaos::BitmapAtlas::TextureArrayAtlas atlas;
  /** the sprites */
  std::vector<SpriteVertex> sprites;
};

void SpriteManager::Initialize()
{

}

glm::vec2 SpriteManager::GetHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type)
{
  if (initial_hotpoint_type == final_hotpoint_type)
    return hotpoint;

  static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

  int initial_h_part = (initial_hotpoint_type >> 0) & 3;
  int initial_v_part = (initial_hotpoint_type >> 2) & 3;

  int final_h_part = (final_hotpoint_type >> 0) & 3;
  int final_v_part = (final_hotpoint_type >> 2) & 3;

  assert(initial_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(initial_v_part != 3);
  assert(final_h_part   != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(final_v_part   != 3);

  glm::vec2 result = hotpoint;
  result.x += size.x * (offset_factor[initial_h_part] - offset_factor[final_h_part]);
  result.y += size.y * (offset_factor[initial_v_part] - offset_factor[final_v_part]);
  return result;

}

glm::vec2 SpriteManager::GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type)
{
  static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

  int h_part = (hotpoint_type >> 0) & 3;
  int v_part = (hotpoint_type >> 2) & 3;

  assert(h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(v_part != 3);

  glm::vec2 result = hotpoint;
  result.x += size.x * offset_factor[h_part];
  result.y += size.y * offset_factor[v_part];
  return result;
}



void SpriteManager::AddSprite(chaos::BitmapAtlas::BitmapEntry * entry, glm::vec2 const & position, glm::vec2 const & size, glm::vec2 const & handler)
{
  glm::vec2 atlas_size = atlas.GetAtlasDimension();
  glm::vec2 pos_bottom_left = position + size * handler;
  glm::vec2 pos_top_right = pos_bottom_left + size;

  float tex_x1 = chaos::MathTools::CastAndDiv<float>(entry->x, atlas_size.x);
  float tex_y1 = chaos::MathTools::CastAndDiv<float>(entry->y, atlas_size.y);
  float tex_x2 = chaos::MathTools::CastAndDiv<float>(entry->x + entry->width, atlas_size.x);
  float tex_y2 = chaos::MathTools::CastAndDiv<float>(entry->y + entry->height, atlas_size.y);

  float bitmap_index = (float)entry->bitmap_index;

  SpriteVertex bl;
  bl.position = pos_bottom_left;
  bl.texcoord.x = tex_x1;
  bl.texcoord.y = tex_y1;
  bl.texcoord.z = bitmap_index;

  SpriteVertex tr;
  tr.position = pos_top_right;
  tr.texcoord.x = tex_x2;
  tr.texcoord.y = tex_y2;
  tr.texcoord.z = bitmap_index;

  SpriteVertex tl;
  tl.position.x = pos_bottom_left.x;
  tl.position.y = pos_top_right.y;
  tl.texcoord.x = tex_x1;
  tl.texcoord.y = tex_y2;
  tl.texcoord.z = bitmap_index;

  SpriteVertex br;
  br.position.x = pos_top_right.x;
  br.position.y = pos_bottom_left.y;
  br.texcoord.x = tex_x2;
  br.texcoord.y = tex_y1;
  br.texcoord.z = bitmap_index;

  sprites.push_back(bl);
  sprites.push_back(br);
  sprites.push_back(tr);

  sprites.push_back(bl);
  sprites.push_back(tr);
  sprites.push_back(tl);
}




















#if 0
int operator ()(std::string const & a, std::string const & b) const
{
	char const * s1 = a.c_str();
	char const * s2 = b.c_str();

	if (s1 == nullptr)
		return (s2 == nullptr)? 0 : -1;
	if (s2 == nullptr)
		return +1;

	int  i  = 0;
	char c1 = toupper(s1[i]);
	char c2 = toupper(s2[i]);			
	while (c1 != 0 && c2 != 0)
	{
		if (c1 != c2)
			return (c1 > c2)? +1 : -1;				
		++i;				
		c1 = toupper(s1[i]);
		c2 = toupper(s2[i]);
	}

	if (c1 == c2)
		return 0;
	return (c1 > c2)? +1 : -1;	
}
#endif






// --------------------------------------------------------------------

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1()
  {


	  std::string ss;
	  std::string s2;

	  auto pp = ss.c_str();
	  auto p2 = s2.c_str();

	  std::map<std::string, int, chaos::StringTools::ci_less> m;
	  m["toto"] = 3;
	  m["atoto"] = 34;
	  m["ztoto"] = 35;

	  int u = m["TOTO"];


	  char const * str1 = "hello, world!";
	  char const * str2 = "HELLO, WORLD!";

	  if (boost::iequals(str1, str2))
	  {
		  int i = 0;
		  ++i;
	  }


  }

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

                              // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_camera.GlobalToLocal();
    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    chaos::GLProgramData const & program_data = program_box->GetProgramData();

    glUseProgram(program_box->GetResourceID());
    program_data.SetUniform("projection", projection);
    program_data.SetUniform("world_to_camera", world_to_camera);
    program_data.SetUniform("local_to_world", local_to_world);

    return true;
  }

  virtual void Finalize() override
  {
    program_box = nullptr;
    atlas.Clear();
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }


  bool LoadAtlas(boost::filesystem::path const & resources_path)
  {
    return atlas.LoadAtlas(resources_path / "MyAtlas.json");
  }

  bool ParseText()
  {
    TextParser parser(atlas);
    ParseTextParams params;
    params.max_text_width = 300;

    bool result = parser.ParseText("Hello worl[d]\n", params);



    return result;
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // load atlas
    if (!LoadAtlas(resources_path))
      return false;

    // parse the text
    if (!ParseText())
      return false;

    // load programs      
    program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
    if (program_box == nullptr)
      return false;

    // place camera
    fps_camera.fps_controller.position.y = 0.0f;
    fps_camera.fps_controller.position.z = 10.0f;

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {

  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    fps_camera.OnMouseButton(glfw_window, button, action, modifier);
  }

  virtual void OnMouseMove(double x, double y) override
  {
    fps_camera.OnMouseMove(glfw_window, x, y);
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::GLProgram>  program_box;
  // the atlas
  chaos::BitmapAtlas::TextureArrayAtlas atlas;
  // the camera
  chaos::MyGLFWFpsCamera fps_camera;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


