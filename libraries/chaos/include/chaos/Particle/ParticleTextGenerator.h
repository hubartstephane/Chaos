namespace chaos
{
	namespace ParticleTextGenerator
	{
#ifdef CHAOS_FORWARD_DECLARATION

		class GeneratorParams;
		class CreateTextAllocationParams;
		class Particle;
		class Token;
		class GeneratorResult;
		class Style;
		class GeneratorData;
		class Generator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		CHAOS_DEFINE_LOG(TextGeneratorLog, "TextGenerator")

		/**
		* some parameters used during text parsing
		*/

		class CHAOS_API GeneratorParams
		{
		public:

			/** constructor */
			GeneratorParams() = default;
			/** constructor */
			GeneratorParams(char const* in_font_name, float in_line_height, glm::vec2 const& in_position, Hotpoint in_hotpoint);

		public:

			/** the size to use for the line */
			float line_height = 32.0f;
			/** spacing between lines */
			float line_spacing = 5.0f;
			/** spacing between characters */
			float character_spacing = 0.0f;
			/** padding for bitmaps */
			glm::vec2 bitmap_padding = { 0.0f, 0.0f };
			/** the text limits */
			float max_text_width = 0.0f;
			/** word wrap enabled */
			bool word_wrap = true;
			/** the maximum factor for space to apply when lines are to be justified. if a greater value should be applyed, ignore justification */
			float justify_space_factor = 2.0f;
			/** the line alignment */
			TextAlignment alignment = TextAlignment::LEFT;
			/** the color to use by default */
			glm::vec4 default_color = { 1.0f, 1.0f, 1.0f, 1.0f };
			/** the font to use by default */
			std::string font_info_name;
			/** tab size */
			size_t tab_size = 2;

			/** the position */
			glm::vec2 position = { 0.0f, 0.0f };
			/** the hotpoint */
			Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT;
		};

		/** json functions */
		CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, GeneratorParams const& src);
		/** json functions */
		CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, GeneratorParams& dst);

		/**
		* some parameters used during the final particle generation
		*/

		class CHAOS_API CreateTextAllocationParams
		{
		public:

			/** enable background */
			bool create_background = false;
			/** the background color */
			glm::vec4 background_color = { 0.4f, 0.4f, 0.4f, 0.4f };
			/** the background padding */
			float background_padding = 20.0f;
		};

		/**
		* Particle : a base particle class
		*/

		class CHAOS_API Particle
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

		class CHAOS_API Token : public Particle
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
			uint32_t character = 0;
			/** the corresponding bitmap (if valid) */
			AtlasBitmapLayout const* bitmap_layout = nullptr;
			/** the corresponding character (if valid) */
			AtlasCharacterLayout const* character_layout = nullptr;
			/** the corresponding character set (if valid) */
			AtlasFontInfo const* font_info = nullptr;
		};

		using TokenLine = std::vector<Token>;

		/**
		* GeneratorResult : the result of the parsing
		*/

		class CHAOS_API GeneratorResult
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
			std::vector<TokenLine> token_lines;
			/** the bounding box */
			ParticleCorners bounding_box;
		};

		/**
		* Style : during parsing, some 'commands' are put on the stack for text formating, Style is such a command
		*/

		class CHAOS_API Style
		{
		public:

			/** the color to use */
			glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
			/** the character set selected */
			AtlasFontInfo const* font_info = nullptr;
		};

		/**
		* GeneratorData : an utility structure used during particles generation
		*/

		class CHAOS_API GeneratorData
		{

		public:
			/** constructor */
			GeneratorData(Generator const& in_generator, GeneratorResult& in_result, GeneratorParams const& in_params) :
				generator(in_generator),
				result(in_result),
				params(in_params)
			{
			}

		public:

			/** duplicate the last stack element */
			Style& PushDuplicate(bool override_top_stack);
			/** add an element on generator stack : keep color, but change current font_info */
			Style& PushAtlasFontInfo(AtlasFontInfo const* font_info, bool override_top_stack);
			/** add an element on generator stack : keep font_info, but change current color */
			Style& PushColor(glm::vec4 const& color, bool override_top_stack);
			/** get a character set from its name */
			AtlasFontInfo const* GetAtlasFontInfoFromName(char const* font_info_name) const;
			/** recursively search a font inside the folder */
			AtlasFontInfo const* GetFirstFont(AtlasFolderInfo const* folder_info) const;

			/** start the markup */
			bool StartMarkup(char const* text, int& i);
			/** utility method to emit characters */
			void EmitCharacters(uint32_t charcode, size_t count);
			/** utility method to emit character */
			void EmitCharacter(uint32_t charcode, AtlasCharacterLayout const* layout, AtlasFontInfo const* font_info);
			/** emit a bitmap */
			void EmitBitmap(AtlasBitmapLayout const* layout, bool use_font_color);
			/** end the current line */
			void EndCurrentLine();
			/** insert a token */
			void InsertTokenInLine(Token& token);

		public:

			/** the generator in use */
			Generator const& generator;
			/** the result */
			GeneratorResult& result;
			/** the parameters from user */
			GeneratorParams const& params;

			/** the stack used for parsing */
			std::vector<Style> style_stack;

			/** current line position for a bitmap (below scanline, at descender level) */
			glm::vec2 bitmap_position = { 0.0f, 0.0f };
			/** current line position for a character (below scanline, at descender level) */
			glm::vec2 character_position = { 0.0f, 0.0f };
		};

		/**
		* Generator : the text generator
		*/

		class CHAOS_API Generator : public Object
		{
			friend class GeneratorData;

		public:

			/** constructor with atlas initialization */
			Generator(AtlasBase const& in_atlas) :
				atlas(in_atlas) {}

			/** get the texture atlas */
			AtlasBase const* GetTextureAtlas() const { return &atlas; }

			/** add a named color in the generator */
			bool AddColor(char const* name, glm::vec4 const& color);
			/** add a named bitmap in the generator */
			bool AddBitmap(char const* name, ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites");
			/** add a named bitmap in the generator */
			bool AddBitmap(char const* name, AtlasBitmapInfo const* info);

			/** add a named character set in the generator */
			bool AddFontInfo(char const* name, char const* font_name);
			/** add a named character set in the generator */
			bool AddFontInfo(char const* name, AtlasFontInfo const* font_info);

			/** the main method to generator a text */
			bool Generate(char const* text, GeneratorResult& result, GeneratorParams const& params = {}) const;

		protected:

			/** the generation internal method */
			bool DoGenerate(char const* text, GeneratorData& generator_data) const;
			/** generate the lines, without cutting them */
			bool DoGenerateLines(char const* text, GeneratorData& generator_data) const;

			/** get a color by its name */
			glm::vec4 const* GetColor(char const* name) const;
			/** get a bitmap by its name */
			AtlasBitmapInfo const* GetBitmapInfo(char const* name) const;
			/** get a character set by its name */
			AtlasFontInfo const* GetAtlasFontInfo(char const* name) const;
			/** test whether a name is a key in one of the following maps : colors, bitmaps, font_infos */
			bool IsNameValid(char const* name) const;

			/** compute the bounding box for all sprite generated */
			bool GetBoundingBox(std::vector<TokenLine> const& result, glm::vec2& min_position, glm::vec2& max_position) const;
			/** compute the bounding box for a single line */
			bool GetBoundingBox(TokenLine const& line, glm::vec2& min_line_position, glm::vec2& max_line_position) const;

			/** move all sprites in a line */
			void MoveParticles(TokenLine& line, glm::vec2 const& offset) const;
			/** move all sprites */
			void MoveParticles(GeneratorResult& result, glm::vec2 const& offset) const;
			/** apply offset for hotpoint */
			bool MoveParticlesToHotpoint(GeneratorData& generator_data) const;
			/** update lines according to justification */
			bool JustifyLines(GeneratorParams const& params, GeneratorData& generator_data) const;

		public:

			/** the colors to use, indexed by a joker name */
			std::map<std::string, glm::vec4, StringTools::RawStringILess> colors;
			/** the bitmaps to use, indexed by a joker name */
			std::map<std::string, AtlasBitmapInfo const*, StringTools::RawStringILess> bitmaps;
			/** the font_info to use, indexed by a joker name */
			std::map<std::string, AtlasFontInfo const*, StringTools::RawStringILess> font_infos;

			/** the atlas where to find entries */
			AtlasBase const& atlas;
		};

		/** transform a token into a particle */
		CHAOS_API ParticleDefault TokenToParticle(ParticleTextGenerator::Token const& token);
		/** get particles corresponding to the background */
		CHAOS_API ParticleDefault GetBackgroundParticle(GeneratorResult const& generator_result, CreateTextAllocationParams const& allocation_params);
		/** generate an allocation for a generated text */
		CHAOS_API SpawnParticleResult CreateTextAllocation(ParticleLayerBase* layer, GeneratorResult const& generator_result, bool new_allocation = true, CreateTextAllocationParams const& allocation_params = {});

		/** output primitives corresponding to generated text */
		template<typename VERTEX_TYPE>
		QuadPrimitive<VERTEX_TYPE> TextToPrimitives(GPUPrimitiveOutput<VERTEX_TYPE>& output, GeneratorResult const& generator_result, CreateTextAllocationParams const& allocation_params = {});

#else

		/** output primitives corresponding to generated text */
		template<typename VERTEX_TYPE>
		QuadPrimitive<VERTEX_TYPE> TextToPrimitives(GPUPrimitiveOutput<VERTEX_TYPE>& output, GeneratorResult const& generator_result, CreateTextAllocationParams const& allocation_params)
		{
			// early exit
			size_t token_count = generator_result.GetTokenCount();
			if (token_count == 0)
				return {};

			int extra_background = (allocation_params.create_background) ? 1 : 0;

			QuadPrimitive<VERTEX_TYPE> result = output.AddQuads(token_count + extra_background);
			QuadPrimitive<VERTEX_TYPE> current_primitive = result;

			// create the background
			if (allocation_params.create_background)
			{
				ParticleDefault particle = GetBackgroundParticle(generator_result, allocation_params);
				ParticleToPrimitive(particle, current_primitive);
				current_primitive++;
			}
			// convert the text
			for (size_t i = 0; i < generator_result.token_lines.size(); ++i)
			{
				ParticleTextGenerator::TokenLine const& line = generator_result.token_lines[i];
				for (size_t j = 0; j < line.size(); ++j)
				{
					ParticleDefault particle = TokenToParticle(line[j]);
					ParticleToPrimitive(particle, current_primitive);
					current_primitive++;
				}
			}
			return result;
		}

#endif

	}; // namespace ParticleTextGenerator

}; // namespace chaos