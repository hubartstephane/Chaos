#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/Hotpoint.h>
#include <chaos/ParticleTools.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>

namespace chaos
{


	namespace ParticleTextGenerator
	{
		/**
		 * some parameters used during text parsing
		 */

		class GeneratorParams
		{
		public:

			static const int ALIGN_LEFT = 0;
			static const int ALIGN_RIGHT = 1;
			static const int ALIGN_CENTER = 2;
			static const int ALIGN_JUSTIFY = 3;

			/** the size to use for the line */
			float line_height = 32.0f;
			/** spacing between lines */
			float line_spacing = 5.0f;
			/** spacing between characters */
			float character_spacing = 5.0f;
			/** padding for bitmaps */
			glm::vec2 bitmap_padding = glm::vec2(5.0f, 5.0f);
			/** the text limits */
			float max_text_width = 0.0f;
			/** word wrap enabled */
			bool word_wrap = true;
			/** the maximum factor for space to apply when lines are to be justified. if a greater value should be applyed, ignore justification */
			float justify_space_factor = 2.0f;
			/** the line alignment */
			int alignment = ALIGN_LEFT;
			/** the color to use by default */
			glm::vec3 default_color = glm::vec3(1.0f, 1.0f, 1.0f);
			/** the font to use by default */
			std::string character_set_name;
			/** tab size */
			int tab_size = 2;

			/** the position */
			glm::vec2 position = glm::vec2(0.0f, 0.0f);
			/** the hotpoint */
			int hotpoint_type = Hotpoint::BOTTOM_LEFT;
		};
	
		/**
		 * Particle : a base particle class
		 */

		class Particle
		{
		public:

			/** the corners of the particle */
			ParticleCorners   corners;
			/** the coordinates in atlas */
			ParticleTexcoords texcoords;	
			/** the colour of the particle */
			glm::vec3 color;
		};

		/**
		* Token : a token that is computed by the generator
		*/

		class Token : public Particle
		{

		public:

			/** returns true whether the token is a bitmap */
			bool IsBitmap() const { return (bitmap_entry != nullptr); }
			/** returns true whether the token is a character */
			bool IsCharacter() const { return (character_entry != nullptr); }
			/** returns true whethe the token correspond to a visible character */
			bool IsVisibleCharacter() const { return (IsCharacter() && character != ' '); }
			/** returns true whethe the token correspond to a whitespace character */
			bool IsWhitespaceCharacter() const { return (IsCharacter() && character == ' '); }

		public:

			/** the character */
			char character = 0;
			/** the corresponding bitmap (if valid) */
			BitmapAtlas::BitmapEntry const * bitmap_entry = nullptr;
			/** the corresponding character (if valid) */
			BitmapAtlas::CharacterEntry const * character_entry = nullptr;
			/** the corresponding character set (if valid) */
			BitmapAtlas::CharacterSet const * character_set = nullptr;		
		};

		using TokenLine = std::vector<Token>;

		/**
		 * GeneratorResult : the result of the parsing
		 */

		class GeneratorResult
		{
		public:

			/** contructor */
			GeneratorResult();
			/** Clearing the result */
			void Clear();
			/** get the number of token */
			size_t GetTokenCount() const;

		public:

			/** the line generated */
			std::vector< TokenLine > token_lines;
			/** the bounding box */
			ParticleCorners bounding_box;
		};

		/**
		 * Style : during parsing, some 'commands' are put on the stack for text formating, Style is such a command 
		 */

		class Style
		{
		public:

			/** the color to use */
			glm::vec3 color;
			/** the character set selected */
			BitmapAtlas::CharacterSet const * character_set = nullptr;
		};

		/**
		* GeneratorData : an utility structure used during particles generation
		*/

		class GeneratorData
		{

		public:
			/** constructor */
			GeneratorData(GeneratorResult & in_result, GeneratorParams const & in_params, BitmapAtlas::AtlasBase const & in_atlas):				
				result(in_result),
				params(in_params),
				atlas(in_atlas)
			{
			}

		public:

			/** duplicate the last stack element */
			Style & PushDuplicate();
			/** add an element on generator stack : keep color, but change current character_set */
			Style & PushCharacterSet(BitmapAtlas::CharacterSet const * character_set);
			/** add an element on generator stack : keep character_set, but change current color */
			Style & PushColor(glm::vec3 const & color);
			/** get a character set from its name */
			BitmapAtlas::CharacterSet const * GetCharacterSetFromName(char const * character_set_name) const;


			/** start the markup */
			bool StartMarkup(char const * text, int & i, class Generator & generator);
			/** utility method to emit characters */
			void EmitCharacters(char c, int count);
			/** utility method to emit character */
			void EmitCharacter(char c, BitmapAtlas::CharacterEntry const * entry, BitmapAtlas::CharacterSet const * character_set);
			/** emit a bitmap */
			void EmitBitmap(BitmapAtlas::BitmapEntry const * entry);
			/** end the current line */
			void EndCurrentLine();
			/** insert a token */
			void InsertTokenInLine(Token & token);

		public:
		
			/** the result */
			GeneratorResult & result;
			/** the parameters from user */
			GeneratorParams const & params;
			/** the atlas in use */
			BitmapAtlas::AtlasBase const & atlas;

			/** the stack used for parsing */
			std::vector<Style> style_stack;

			/** current line position for a bitmap (below scanline, at descender level) */
			glm::vec2 bitmap_position = glm::vec2(0.0f, 0.0f);
			/** current line position for a character (below scanline, at descender level) */
			glm::vec2 character_position = glm::vec2(0.0f, 0.0f);
		};

		/**
		* Generator : the text generator
		*/

		class Generator : public ReferencedObject
		{
			friend class GeneratorData;

		public:

			/** constructor with atlas initialization */
			Generator(BitmapAtlas::AtlasBase const & in_atlas) : 
				atlas(in_atlas) {}

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
			bool Generate(char const * text, GeneratorResult & result, GeneratorParams const & params = GeneratorParams());

		protected:

			/** the generation internal method */
			bool DoGenerate(char const * text, GeneratorData & generator_data);
			/** generate the lines, without cutting them */
			bool DoGenerateLines(char const * text, GeneratorData & generator_data);

			/** get a color by its name */
			glm::vec3 const * GetColor(char const * name) const;
			/** get a bitmap by its name */
			BitmapAtlas::BitmapEntry const * GetBitmap(char const * name) const;
			/** get a character set by its name */
			BitmapAtlas::CharacterSet const * GetCharacterSet(char const * name) const;
			/** test whether a name is a key in one of the following maps : colors, bitmaps, character_sets */
			bool IsNameValid(char const * name) const;

			/** compute the bounding box for all sprite generated */
			bool GetBoundingBox(std::vector<TokenLine> const & result, glm::vec2 & min_position, glm::vec2 & max_position) const;
			/** compute the bounding box for a single line */
			bool GetBoundingBox(TokenLine const & line, glm::vec2 & min_line_position, glm::vec2 & max_line_position) const;


			/** move all sprites in a line */
			void MoveParticles(TokenLine & line, glm::vec2 const & offset);
			/** move all sprites */
			void MoveParticles(GeneratorResult & result, glm::vec2 const & offset);
			/** apply offset for hotpoint */
			bool MoveParticlesToHotpoint(GeneratorData & generator_data);


#if 0


			/** cut the line when necessary */
			bool BreakLines(GeneratorParams const & params, GeneratorData & generator_data);
			/** utility method to cut one line an insert it into a new result */
			void BreakOneLine(float & y, TokenLine const & line, GeneratorResult & result_lines, GeneratorParams const & params, GeneratorData & generator_data);
			/** update lines according to justification */
			bool JustifyLines(GeneratorParams const & params, GeneratorData & generator_data);


			/** generate the sprites */
			bool GenerateSprites(SpriteManager * sprite_manager, GeneratorParams const & params, GeneratorData & generator_data);




			//void BreakLineIntoWords(TokenLine const & line, std::vector<LexicalTokenGroup> & result);

#endif

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


		/** generate an allocation for a generated text */
		ParticleAllocation * CreateTextAllocation(ParticleLayer * layer, GeneratorResult const & result);


	
	}; // namespace ParticleTextGenerator

};