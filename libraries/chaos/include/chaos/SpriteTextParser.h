#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace SpriteText
	{
		/** grouping some sprite together */
		class LexicalTokenGroup
		{
		public:

			static int const TYPE_UNKNOWN = 0;

			/** the types for tokens (when breaking words) */		
			static int const TYPE_CONSONANT = 1;
			static int const TYPE_VOWEL = 2;
			static int const TYPE_MINUS = 3;

			/** the types for tokens (when breaking lines) */
			static int const TYPE_WORD = 4;
			static int const TYPE_SEPARATOR = 5;
			static int const TYPE_PUNCT = 6;

			/** the type of the token */
			int type;
			/** the first token index concerned by the group */
			int token_start;
			/** the last token index concerned by the group */
			int token_end;
		};

	}; // namespace SpriteText

};