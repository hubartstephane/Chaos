#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/SpriteManager.h>

namespace chaos
{
	/** some parameters used during text parsing */
	class SpriteTextGeneratorParams
	{
	public:

		static const int ALIGN_LEFT = 0;
		static const int ALIGN_RIGHT = 1;
		static const int ALIGN_CENTER = 2;
		static const int ALIGN_JUSTIFY = 3;

		/** the size to use for the line */
		float line_height{ 32.0f };
		/** spacing between lines */
		float line_spacing{ 5.0f };
		/** spacing between characters */
		float character_spacing{ 5.0f };
		/** padding for bitmaps */
		glm::vec2 bitmap_padding{ 5.0f, 5.0f };
		/** the text limits */
		float max_text_width{ 0.0f };
		/** word wrap enabled */
		bool word_wrap{ true };
		/** the factor for space to apply when lines are to be justified */
		float justify_space_factor{ 2.0f };
		/** the line alignment */
		int alignment{ ALIGN_LEFT };
		/** the color to use by default */
		glm::vec3 default_color{ 1.0f, 1.0f, 1.0f };
		/** the font to use by default */
		std::string character_set_name;
		/** tab size */
		int tab_size{ 2 };

		/** the position */
		glm::vec2 position{ 0.0f , 0.0f };
		/** the hotpoint */
		int hotpoint_type{ Hotpoint::BOTTOM_LEFT };
	};

	/** during parsing, some 'commands' are put on the stack for text formating, SpriteTextStackElement is such a command */
	class SpriteTextStackElement
	{
	public:

		/** the color to use */
		glm::vec3 color;
		/** the character set selected */
		BitmapAtlas::CharacterSet const * character_set{ nullptr };
	};

	/** a token generated during parsing */
	class SpriteTextToken
	{

	public:

		/** returns true whether the token is a bitmap */
		bool IsBitmap() const { return (bitmap_entry != nullptr); }
		/** returns true whether the token is a character */
		bool IsCharacter() const { return (character_entry != nullptr); }
		/** returns true whethe the token correspond to a visible character */
		bool IsVisibleCharacter() const { return (IsCharacter() && character != ' '); }

	public:

		/** the character */
		char character{ 0 };
		/** the position of the generated image (BOTTOM LEFT) */
		glm::vec2 position{ 0.0f, 0.0f };
		/** the size of the generated image */
		glm::vec2 size{ 0.0f, 0.0f };
		/** the color of the token */
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };
		/** the corresponding bitmap (if valid) */
		BitmapAtlas::BitmapEntry const * bitmap_entry{ nullptr };
		/** the corresponding character (if valid) */
		BitmapAtlas::CharacterEntry const * character_entry{ nullptr };
		/** the corresponding character set (if valid) */
		BitmapAtlas::CharacterSet const * character_set{ nullptr };
	};

	/** the of a line of parsing */
	using SpriteTextLine = std::vector<SpriteTextToken>;
	/** the result of parsing */
	using SpriteTextResult = std::vector<SpriteTextLine>;

	/** an utility structure used to contains data during parsing */
	class SpriteTextGeneratorData
	{
	public:

		/** the constructor */
		SpriteTextGeneratorData(BitmapAtlas::AtlasBase const & in_atlas) : atlas(in_atlas) {}

		/** start the markup */
		bool StartMarkup(char const * text, int & i, class SpriteTextGenerator & generator, SpriteTextGeneratorParams const & params);
		/** utility method to emit characters */
		void EmitCharacters(char c, int count, SpriteTextGeneratorParams const & params);
		/** utility method to emit character */
		void EmitCharacter(char c, BitmapAtlas::CharacterEntry const * entry, BitmapAtlas::CharacterSet const * character_set, SpriteTextGeneratorParams const & params);
		/** emit a bitmap */
		void EmitBitmap(BitmapAtlas::BitmapEntry const * entry, SpriteTextGeneratorParams const & params);
		/** end the current line */
		void EndCurrentLine(SpriteTextGeneratorParams const & params);
		/** insert a token */
		void InsertTokenInLine(SpriteTextToken & token, SpriteTextGeneratorParams const & params);

		/** duplicate the last stack element */
		void PushDuplicate();
		/** add an element on generator stack : keep color, but change current character_set */
		void PushCharacterSet(BitmapAtlas::CharacterSet const * character_set);
		/** add an element on generator stack : keep character_set, but change current color */
		void PushColor(glm::vec3 const & color);

		/** get a character set from its name */
		BitmapAtlas::CharacterSet const * GetCharacterSetFromName(char const * character_set_name) const;

	public:

		/** the atlas in used */
		BitmapAtlas::AtlasBase const & atlas;
		/** current line position for a bitmap (below scanline, at descender level) */
		glm::vec2 bitmap_position{ 0.0f, 0.0f };
		/** current line position for a character (below scanline, at descender level) */
		glm::vec2 character_position{ 0.0f, 0.0f };
		/** the result */
		SpriteTextResult generator_result;
		/** the stack used for parsing */
		std::vector<SpriteTextStackElement> generator_stack;
	};













	/** the text generator */
	class SpriteTextGenerator
	{
		friend class SpriteTextGeneratorData;

	public:

		/** constructor with atlas initialization */
		SpriteTextGenerator(BitmapAtlas::AtlasBase const & in_atlas) : atlas(in_atlas) {}

		/** add a named color in the generator */
		bool AddColor(char const * name, glm::vec3 const & color);
		/** add a named bitmap in the generator */
		bool AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name);
		/** add a named bitmap in the generator */
		bool AddBitmap(char const * name, BitmapAtlas::BitmapEntry const * entry);

		/** add a named character set in the generator */
		bool AddCharacterSet(char const * name, char const * font_name);
		/** add a named character set in the generator */
		bool AddCharacterSet(char const * name, BitmapAtlas::CharacterSet const * character_set);

		/** the main method to generator a text */
		bool GenerateSprites(char const * text, SpriteManager * sprite_manager = nullptr, SpriteTextResult * generator_result = nullptr, SpriteTextGeneratorParams const & params = SpriteTextGeneratorParams());

	protected:

		/** get a color by its name */
		glm::vec3 const * GetColor(char const * name) const;
		/** get a bitmap by its name */
		BitmapAtlas::BitmapEntry const * GetBitmap(char const * name) const;
		/** get a character set by its name */
		BitmapAtlas::CharacterSet const * GetCharacterSet(char const * name) const;
		/** test whether a name is a key in one of the following maps : colors, bitmaps, character_sets */
		bool IsNameValid(char const * name) const;



		


		/** generate the lines, without cutting them */
		bool GenerateLines(char const * text, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);

		/** cut the line when necessary */
		bool BreakLines(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);

	#if 0


		

		/** cut the lines so they are not too big. Cut them only when it is possible */
		bool CutLines(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);
		/** utility method to cut one line an insert it into a new result */
		void CutOneLine(float & y, SpriteTextLine const & line, SpriteTextResult & result_lines, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);




	#endif


		/** update lines according to justification */
		bool JustifyLines(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);
		/** apply offset for hotpoint */
		bool MoveSpritesToHotpoint(SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);

		/** generate the sprites */
		bool GenerateSprites(SpriteManager * sprite_manager, SpriteTextGeneratorParams const & params, SpriteTextGeneratorData & generator_data);

		/** compute the bounding box for all sprite generated */
		bool GetBoundingBox(SpriteTextResult const & generator_result, glm::vec2 & min_position, glm::vec2 & max_position) const;
		/** compute the bounding box for a single line */
		bool GetBoundingBox(SpriteTextLine const & generator_line, glm::vec2 & min_line_position, glm::vec2 & max_line_position) const;
		/** move all sprites in a line */
		void MoveSprites(SpriteTextLine & generator_line, glm::vec2 const & offset);
		/** move all sprites */
		void MoveSprites(SpriteTextResult & generator_result, glm::vec2 const & offset);

	public:

		/** the colors to use, indexed by a joker name */
		std::map<std::string, glm::vec3, StringTools::ci_less> colors;
		/** the bitmaps to use, indexed by a joker name */
		std::map<std::string, BitmapAtlas::BitmapEntry const *, StringTools::ci_less> bitmaps;
		/** the character_set to use, indexed by a joker name */
		std::map<std::string, BitmapAtlas::CharacterSet const *, StringTools::ci_less> character_sets;
		/** the atlas where to find entries */
		BitmapAtlas::AtlasBase const & atlas;
	};
};