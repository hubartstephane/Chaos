#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/Hotpoint.h>
#include <chaos/ParticleTools.h>
#include <chaos/Object.h>
#include <chaos/ParticleManager.h>
#include <chaos/TextAlignment.h>

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

			/** constructor */
			GeneratorParams() = default;
			/** constructor */
			GeneratorParams(char const * in_font_name, float in_line_height, glm::vec2 const & in_position, Hotpoint in_hotpoint);

		public:

			/** the size to use for the line */
			float line_height = 32.0f;
			/** spacing between lines */
			float line_spacing = 5.0f;
			/** spacing between characters */
			float character_spacing = 0.0f;
			/** padding for bitmaps */
			glm::vec2 bitmap_padding = glm::vec2(0.0f, 0.0f);
			/** the text limits */
			float max_text_width = 0.0f;
			/** word wrap enabled */
			bool word_wrap = true;
			/** the maximum factor for space to apply when lines are to be justified. if a greater value should be applyed, ignore justification */
			float justify_space_factor = 2.0f;
			/** the line alignment */
			TextAlignment alignment = TextAlignment::LEFT;
			/** the color to use by default */
			glm::vec4 default_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			/** the font to use by default */
			std::string font_info_name;
			/** tab size */
			int tab_size = 2;

			/** the position */
			glm::vec2 position = glm::vec2(0.0f, 0.0f);
			/** the hotpoint */
			Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT;
		};

		/** json functions */
		bool SaveIntoJSON(nlohmann::json & json_entry, GeneratorParams const & src);
		/** json functions */
		bool LoadFromJSON(nlohmann::json const & json_entry, GeneratorParams & dst);

		/**
		* some parameters used during the final particle generation
		*/

		class CreateTextAllocationParams
		{
		public:

			/** enable background */
			bool create_background = false;
			/** the background color */
			glm::vec4 background_color = glm::vec4(0.4f, 0.4f, 0.4f, 0.4f);
			/** the background padding */
			float background_padding = 20.0f;
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
			/** the color of the particle */
			glm::vec4 color;
		};

		/**
		* Token : a token that is computed by the generator
		*/

		class Token : public Particle
		{

		public:

			/** returns true whether the token is a bitmap */
			bool IsBitmap() const { return (bitmap_layout != nullptr); }
			/** returns true whether the token is a character */
			bool IsCharacter() const { return (character_layout != nullptr); }
			/** returns true whethe the token correspond to a visible character */
			bool IsVisibleCharacter() const { return (IsCharacter() && character != ' '); }
			/** returns true whethe the token correspond to a whitespace character */
			bool IsWhitespaceCharacter() const { return (IsCharacter() && character == ' '); }

		public:

			/** the character */
			char character = 0;
			/** the corresponding bitmap (if valid) */
			BitmapAtlas::BitmapLayout const * bitmap_layout = nullptr;
			/** the corresponding character (if valid) */
			BitmapAtlas::CharacterLayout const * character_layout = nullptr;
			/** the corresponding character set (if valid) */
			BitmapAtlas::FontInfo const * font_info = nullptr;		
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
			glm::vec4 color;
			/** the character set selected */
			BitmapAtlas::FontInfo const * font_info = nullptr;
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
			Style & PushDuplicate(bool override_top_stack);
			/** add an element on generator stack : keep color, but change current font_info */
			Style & PushFontInfo(BitmapAtlas::FontInfo const * font_info, bool override_top_stack);
			/** add an element on generator stack : keep font_info, but change current color */
			Style & PushColor(glm::vec4 const & color, bool override_top_stack);
			/** get a character set from its name */
			BitmapAtlas::FontInfo const * GetFontInfoFromName(char const * font_info_name) const;
			/** recursively search a font inside the folder */
			BitmapAtlas::FontInfo const * GetFirstFont(BitmapAtlas::FolderInfo const * folder_info) const;

			/** start the markup */
			bool StartMarkup(char const * text, int & i, class Generator & generator);
			/** utility method to emit characters */
			void EmitCharacters(char c, int count);
			/** utility method to emit character */
			void EmitCharacter(char c, BitmapAtlas::CharacterLayout const * layout, BitmapAtlas::FontInfo const * font_info);
			/** emit a bitmap */
			void EmitBitmap(BitmapAtlas::BitmapLayout const * layout, bool use_font_color);
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

		class Generator : public Object
		{
			friend class GeneratorData;

		public:

			/** constructor with atlas initialization */
			Generator(BitmapAtlas::AtlasBase const & in_atlas) : 
				atlas(in_atlas) {}

			/** add a named color in the generator */
			bool AddColor(char const * name, glm::vec4 const & color);
			/** add a named bitmap in the generator */
			bool AddBitmap(char const * name, char const * folder_name, char const * bitmap_name);
			/** add a named bitmap in the generator */
			bool AddBitmap(char const * name, BitmapAtlas::BitmapInfo const * info);

			/** add a named character set in the generator */
			bool AddFontInfo(char const * name, char const * font_name);
			/** add a named character set in the generator */
			bool AddFontInfo(char const * name, BitmapAtlas::FontInfo const * font_info);

			/** the main method to generator a text */
			bool Generate(char const * text, GeneratorResult & result, GeneratorParams const & params = GeneratorParams());

		protected:

			/** the generation internal method */
			bool DoGenerate(char const * text, GeneratorData & generator_data);
			/** generate the lines, without cutting them */
			bool DoGenerateLines(char const * text, GeneratorData & generator_data);

			/** get a color by its name */
			glm::vec4 const * GetColor(char const * name) const;
			/** get a bitmap by its name */
			BitmapAtlas::BitmapInfo const * GetBitmapInfo(char const * name) const;
			/** get a character set by its name */
			BitmapAtlas::FontInfo const * GetFontInfo(char const * name) const;
			/** test whether a name is a key in one of the following maps : colors, bitmaps, font_infos */
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

			/** update lines according to justification */
			bool JustifyLines(GeneratorParams const & params, GeneratorData & generator_data);

#if 0


			/** cut the line when necessary */
			bool BreakLines(GeneratorParams const & params, GeneratorData & generator_data);
			/** utility method to cut one line an insert it into a new result */
			void BreakOneLine(float & y, TokenLine const & line, GeneratorResult & result_lines, GeneratorParams const & params, GeneratorData & generator_data);



			/** generate the sprites */
			bool GenerateSprites(SpriteManager * sprite_manager, GeneratorParams const & params, GeneratorData & generator_data);




			//void BreakLineIntoWords(TokenLine const & line, std::vector<LexicalTokenGroup> & result);

#endif

		public:

			/** the colors to use, indexed by a joker name */
			std::map<std::string, glm::vec4, StringTools::ci_less> colors;
			/** the bitmaps to use, indexed by a joker name */
			std::map<std::string, BitmapAtlas::BitmapInfo const *, StringTools::ci_less> bitmaps;
			/** the font_info to use, indexed by a joker name */
			std::map<std::string, BitmapAtlas::FontInfo const *, StringTools::ci_less> font_infos;

			/** the atlas where to find entries */
			BitmapAtlas::AtlasBase const & atlas;
		};


		/** generate an allocation for a generated text */
		ParticleAllocationBase * CreateTextAllocation(ParticleLayerBase * layer, GeneratorResult const & generator_result, CreateTextAllocationParams const & allocation_params = CreateTextAllocationParams());

	}; // namespace ParticleTextGenerator

}; // namespace chaos